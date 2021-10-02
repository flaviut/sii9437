/******************************************************************************
 *
 * 2017 (c) Lattice Semiconductor Corporation
 * 
 * This program is free software; you can redistribute it and/or
 * modify it only under the terms of version 2 of the GNU General Public License
 * as published by the Free Software Foundation, and not any later version.
 * 
 * This program is distributed "AS IS" WITHOUT ANY WARRANTY of any kind,
 * whether express or implied, including without limitation the implied warranty
 * of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE or NON-INFRINGEMENT.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 
 * You may contact Lattice Semiconductor Corporation at
 * 111 SW 5th Ave, Suite 700, Portland, OR 97204.
 *
 *****************************************************************************/

/**
 * @file sii9612_drv_cec.c
 *
 * @brief Driver/Application for SiI9612 CEC
 *
 *****************************************************************************/
#define SII_DEBUG	3

/***** #include statements ***************************************************/
#include "sii9612_drv_cec_api.h"
#include "sii9612_drv_api.h"
#include "sii_sys_time_api.h"

/***** Register Module name **************************************************/
SII_MODULE_NAME_SET(CEC);

/***** local macro definitions **********************************************/
Sii943xEventCallbackFunc_t SiiCecEvtCbFunc = NULL;
uint8_t CecVendorID[3] = {0x00,0x01,0x03}; /*should be SIMG vendor ID*/
static bool_t isDsPARcvd = false;

const uint8_t abort_reason[6][20] = {"Unrecog Opcode",
		"NotInCurrmode",
		"CannotProvideSource",
		"InvalidOperand",
		"Refused",
		"UnableToDetermine"};

/***** local type definitions ************************************************/
uint8_t l_devTypes [16] =
{
	CEC_LOGADDR_TV,
	CEC_LOGADDR_RECDEV1,
	CEC_LOGADDR_RECDEV1,
	CEC_LOGADDR_TUNER1,
	CEC_LOGADDR_PLAYBACK1,
	CEC_LOGADDR_AUDSYS,
	CEC_LOGADDR_PURE_SWITCH,
	CEC_LOGADDR_VIDEO_PROCESSOR,
	CEC_LOGADDR_PLAYBACK1,
	CEC_LOGADDR_RECDEV1,
	CEC_LOGADDR_TUNER1,
	CEC_LOGADDR_PLAYBACK1,
	CEC_LOGADDR_RECDEV2,
	CEC_LOGADDR_RECDEV2,
	CEC_LOGADDR_TV,
	CEC_LOGADDR_TV
};

typedef struct _CecLogicalDevice_t
{
	int_t   deviceType;	/* 0 - Device is a TV.
				   1 - Device is a Recording device
				   2 - Device is a reserved device
				   3 - Device is a Tuner
				   4 - Device is a Playback device
				   5 - Device is an Audio System*/
	int_t       port;	/* HDMI port index for this device.*/
	int_t       selected;	/* This logical addr was selected on this port*/
	uint16_t    cecPA;	/* CEC Physical address of the device.*/
	bool_t      isVirtual;	/* device is a local virtual device*/
} CecLogicalDevice_t;

typedef struct _SiiCecMsgHandlers_t
{
	bool_t      callAlways;
	bool_t      (*pMsgHandler)( SiiCecData_t * );
} SiiCecMsgHandlers_t;

typedef enum _SiiCecTasks_t
{
	SiiCECTASK_IDLE                 = 0,
	SiiCECTASK_CANCEL,
	SiiCECTASK_ENUMERATE,
	SiiCECTASK_SETLA,
	SiiCECTASK_ONETOUCH,
	SiiCECTASK_SENDMSG,

	SiiCECTASK_COUNT
} SiiCecTasks_t;

typedef struct _CecInstanceData_t
{
	int         structVersion;
	int         instanceIndex;
	int         lastResultCode;
	uint16_t    statusFlags;

	SiiCecDeviceTypes_t deviceType;	/* type of CEC device*/

	uint32_t      debugDisplayLevel;

	SiiCecLogicalAddresses_t    devLogicalAddr;
	SiiCecLogicalAddresses_t    logicalAddr;

	uint8_t     paShift;
	uint16_t    paChildMask;
	uint16_t    physicalAddr;

	uint8_t     osdName[14];
	int         osdNameLen;

	uint8_t     lastUserControlPressedSourceLa;/*For User Control Released*/
	uint8_t     lastUserControlPressedTargetLa;/*For User Control Released*/

	/* RX-only data*/

	SiiCecPowerstatus_t         sourcePowerStatus;
	SiiCecLogicalAddresses_t    activeSrcLogical;
	uint16_t                    activeSrcPhysical;
	CecLogicalDevice_t          logicalDeviceInfo [16];

	/* TX-only data*/
	bool_t      isActiveSource;

	/* Task data*/
	int32_t               taskQueueIn;
	int32_t               taskQueueOut;
	SiiCecTasks_t       currentTask;
	bool_t              enumerateComplete;
	bool_t		legacyArcEnable;

	/* External message handlers*/

	bool_t      (*pMsgHandler)( SiiCecData_t * );
} CecInstanceData_t;

CecInstanceData_t cecInstance;
CecInstanceData_t *pCec = &cecInstance;

/* Local static functions*/
static void SiiCecSendVendorId(  SiiCecLogicalAddresses_t srcLa,
		SiiCecLogicalAddresses_t destLa,uint8_t *vendorId );
static void SiiCecUpdateLogicalDeviceInfo ( uint8_t newLA, uint16_t newPA,
		bool_t isActive);
static uint16_t SiiCecGetDsPhysicalAddress (SiiCecLogicalAddresses_t srcLa,
		SiiCecLogicalAddresses_t desLa);
static uint16_t SiiCecSendReportPhysicalAddress (SiiCecLogicalAddresses_t srcLa,
		uint16_t srcPa );
static void SiiCecSacSystemAudioModeStatusSend (SiiCecLogicalAddresses_t srcLa,
		const uint8_t destLogAddr);
static bool_t SiiDrvCecSend ( SiiCecData_t *pMsg  );
static void SiiCecSetDevicePA (uint16_t devPa);
static void SiiCecRequestArcInitiationSend (SiiCecLogicalAddresses_t srcLa,
		const uint8_t destLogAddr);
static void SiiCecSendGiveDeviceVendorId(SiiCecLogicalAddresses_t srcLa,
					SiiCecLogicalAddresses_t destLa);

bool_t CecTxMsgHandler ( SiiCecData_t *pMsg )
{
	Sii943xEventFlags_t eventFlags = 0;
	SiiCecData_t cecFrame;
	bool_t isDirectAddressed;
	uint8_t sourceLA;

	isDirectAddressed = (bool_t)!((pMsg->srcDestAddr & 0x0F) ==
					CEC_LOGADDR_UNREGORBC);
	sourceLA = (pMsg->srcDestAddr >> 4) & 0xF;

	switch ( pMsg->opcode ) {
	case CECOP_REPORT_ARC_INITIATED:
		if (!isDirectAddressed)
			break;
		SII_LOG1A("", NULL, ("Rcvd <-- REPORT_ARC_INITIATED\n"));
		/*Notify application to initiate ARC*/
		eventFlags |= SII_9612_CEC_EVENT_FLAG__ARC_INIT;
		break;
	case CECOP_REPORT_ARC_TERMINATED:
		if (!isDirectAddressed)
			break;
		SII_LOG1A("", NULL, ("Rcvd <-- REPORT_ARC_TERMINATED\n"));
		/*Notify application to Terminate ARC*/
		eventFlags |= SII_9612_CEC_EVENT_FLAG__ARC_TERM;
		break;
	case CECOP_REQUEST_ARC_INITIATION:
		if (!isDirectAddressed)
			break;
		SII_LOG1A("", NULL, ("Rcvd <-- REQUEST_ARC_INITIATION\n"));
		/* send the response.*/
		cecFrame.opcode         = CECOP_INITIATE_ARC;
		cecFrame.srcDestAddr    = MAKE_SRCDEST( CEC_LOGADDR_AUDSYS,
				sourceLA );
		cecFrame.argCount       = 0;
		if (SiiDrvCecSend(&cecFrame))
			SII_LOG1A("", NULL,
				("Sent --> INITIATE_ARC\n"));
		break;
	case CECOP_REQUEST_ARC_TERMINATION:
		if (!isDirectAddressed)
			break;
		SII_LOG1A("", NULL, ("Rcvd <-- REQUEST_ARC_TERMINATION\n"));
		/* send the response.*/
		cecFrame.opcode         = CECOP_TERMINATE_ARC;
		cecFrame.srcDestAddr    = MAKE_SRCDEST( CEC_LOGADDR_AUDSYS,
				sourceLA );
		cecFrame.argCount       = 0;
		if (SiiDrvCecSend(&cecFrame))
			SII_LOG1A("", NULL,
				("Sent --> TERMINATE_ARC\n"));
		break;

	case CECOP_GIVE_PHYSICAL_ADDRESS:
		if ( isDirectAddressed)	/* Ignore as broadcast message*/
		{
			SII_LOG1A("", NULL,
				("Rcvd <-- GIVE_PHYSICAL_ADDRESS\n"));
			SiiCecSendReportPhysicalAddress( CEC_LOGADDR_AUDSYS,
					pCec->physicalAddr );
		}

		if (!isDsPARcvd)
		{
			SiiCecGetDsPhysicalAddress((pMsg->srcDestAddr & 0x0F),
					((pMsg->srcDestAddr>> 4 )& 0x0F));
		}

		break;

	case CECOP_REPORT_PHYSICAL_ADDRESS:
		isDsPARcvd= true;
		if ( !isDirectAddressed )	/* Ignore as broadcast message*/
		{
			SII_LOG1A("", NULL,
				("Rcvd <-- REPORT_PHYSICAL_ADDRESS\n"));
			if(pMsg->args[2] != 0)
				pMsg->srcDestAddr =
					(pMsg->srcDestAddr & 0x0F) |
					(pMsg->args[2] << 4) ;

			SiiCecUpdateLogicalDeviceInfo(
					/* broadcast logical address*/
					(pMsg->srcDestAddr >> 4) & 0x0F,
					/* broadcast physical address*/
					(((uint16_t)(pMsg->args[0])) << 8) |
					pMsg->args[1],
					false);

			SII_LOG1A("", NULL,
				("LA: 0x%x,  PA: 0x%02x%02x\n",
					(pMsg->srcDestAddr >> 4) & 0x0F,
					pMsg->args[0],
					pMsg->args[1]));
		}
		break;

	case CECOP_GIVE_DEVICE_VENDOR_ID:
		if (!isDirectAddressed)
			break;
		SII_LOG1A("", NULL,
			("Rcvd <-- GIVE_DEVICE_VENDOR_ID\n"));
		SiiCecSendVendorId(CEC_LOGADDR_AUDSYS,
				CEC_LOGADDR_UNREGORBC,
				CecVendorID);
		break;

	case CECOP_GIVE_DEVICE_POWER_STATUS:
		if (!isDirectAddressed)
			break;
		SII_LOG1A("", NULL, ("Rcvd <-- GIVE_DEVICE_POWER_STATUS\n"));
		cecFrame.opcode        = CECOP_REPORT_POWER_STATUS;
		cecFrame.srcDestAddr   = MAKE_SRCDEST( CEC_LOGADDR_AUDSYS,
				sourceLA );
		cecFrame.args[0]       = CEC_POWERSTATUS_ON;
		cecFrame.argCount      = 1;

		if (SiiDrvCecSend(&cecFrame))
			SII_LOG1A("", NULL,
				("Sent --> POWERSTATUS_ON\n"));
		break;
	case CECOP_GIVE_SYSTEM_AUDIO_MODE_STATUS:
		SII_LOG1A("", NULL,
			("Rcvd <-- GIVE_SYSTEM_AUDIO_MODE_STATUS\n"));
		/* Respond with "System Audio Mode Status" to sender*/
		SiiCecSacSystemAudioModeStatusSend(CEC_LOGADDR_AUDSYS,
				sourceLA);
		break;
	case CECOP_FEATURE_ABORT:
		if (isDirectAddressed)
		{
			SII_LOG1A("", NULL,
			("CEC Message %02X was Feature Aborted by LA %d\n",
					pMsg->args[0],
					(pMsg->srcDestAddr >> 4)));
			SII_LOG1A("", NULL,
			("Abort Reason: %s\n",
					abort_reason[pMsg->args[1]]));
		}
		break;
	default:
		if (!isDirectAddressed) {
			SII_LOG1A("", NULL, ("Directly Addressed Message\n"));
		} else {
			SII_LOG1A("", NULL, ("Broadcast Message\n"));
		}

		SII_LOG1A("", NULL, ("Opcode Received: 0x%x\n", pMsg->opcode));
		break;
	}

	SiiCecEvtCbFunc(eventFlags);

	return false;
}

bool_t CecRxMsgHandler ( SiiCecData_t *pMsg )
{
	Sii943xEventFlags_t eventFlags = 0;
	SiiCecData_t cecFrame;
	bool_t isDirectAddressed;
	uint8_t sourceLA;

	isDirectAddressed = (bool_t)!((pMsg->srcDestAddr & 0x0F) ==
					CEC_LOGADDR_UNREGORBC);
	sourceLA = (pMsg->srcDestAddr >> 4) & 0xF;

	switch ( pMsg->opcode ) {
	case CECOP_REPORT_PHYSICAL_ADDRESS:
		isDsPARcvd= true;
		if ( !isDirectAddressed )	/* Ignore as direct message*/
		{
			SII_LOG1A("", NULL,
				("Rcvd <-- REPORT_PHYSICAL_ADDRESS\n"));
			if(pMsg->args[2] != 0)
				pMsg->srcDestAddr =
					(pMsg->srcDestAddr & 0x0F) |
					(pMsg->args[2] << 4) ;

			SiiCecUpdateLogicalDeviceInfo(
					/* broadcast logical address*/
					(pMsg->srcDestAddr >> 4) & 0x0F,
					/* broadcast physical address*/
					(((uint16_t)(pMsg->args[0])) << 8) |
					pMsg->args[1],
					false);

			SII_LOG1A("", NULL,
				("LA: 0x%x,  PA: 0x%02x%02x\n",
					(pMsg->srcDestAddr >> 4) & 0x0F,
						pMsg->args[0],
						pMsg->args[1]));
		}

		if (pCec->legacyArcEnable){
			SiiCecSendGiveDeviceVendorId(pCec->devLogicalAddr,
					pCec->logicalAddr);

			sii_sys_time_milli_delay(100);

			SiiCecRequestArcInitiationSend(pCec->devLogicalAddr,
						pCec->logicalAddr);
			}
		break;
	case CECOP_DEVICE_VENDOR_ID:
		if (isDirectAddressed)
			break;
		SII_LOG1A("", NULL,
			("Rcvd <-- DEVICE_VENDOR_ID: 0x%02x 0x%02x 0x%02x\n",
				pMsg->args[0], pMsg->args[1], pMsg->args[2]));

		break;

	case CECOP_INITIATE_ARC:
		if (!isDirectAddressed)
			break;
		SII_LOG1A("", NULL, ("Rcvd <-- INITIATE_ARC\n"));
		/* send the response.*/
		cecFrame.opcode = CECOP_REPORT_ARC_INITIATED;
		cecFrame.srcDestAddr = MAKE_SRCDEST(pCec->devLogicalAddr,
				sourceLA);
		cecFrame.argCount  = 0;
		if (SiiDrvCecSend(&cecFrame))
			SII_LOG1A("", NULL,
				("Sent --> REPORT_ARC_INITIATED\n"));
		/* Notify application to Initiate ARC */
		eventFlags |= SII_9612_CEC_EVENT_FLAG__ARC_INIT;
		break;
	case CECOP_TERMINATE_ARC:
		if (!isDirectAddressed)
			break;
		SII_LOG1A("", NULL, ("Rcvd <-- TERMINATE_ARC\n"));
		/* send the response.*/
		cecFrame.opcode = CECOP_REPORT_ARC_TERMINATED;
		cecFrame.srcDestAddr = MAKE_SRCDEST(pCec->devLogicalAddr,
				sourceLA);
		cecFrame.argCount = 0;
		if (SiiDrvCecSend(&cecFrame))
			SII_LOG1A("", NULL,
				("Sent --> REPORT_ARC_TERMINATED\n"));
		/* Notify application to Initiate ARC */
		eventFlags |= SII_9612_CEC_EVENT_FLAG__ARC_TERM;
		break;

	case CECOP_GIVE_PHYSICAL_ADDRESS:
		if (isDirectAddressed)	/* Ignore as broadcast message*/
		{
			SII_LOG1A("", NULL,
					("Rcvd <-- GIVE_PHYSICAL_ADDRESS\n"));
			SiiCecSendReportPhysicalAddress( CEC_LOGADDR_AUDSYS,
					pCec->physicalAddr );
		}

		if (!isDsPARcvd)
		{
			SiiCecGetDsPhysicalAddress((pMsg->srcDestAddr & 0x0F),
					((pMsg->srcDestAddr>> 4 )& 0x0F));
		}
		break;

	default:
		if (!isDirectAddressed) {
			SII_LOG1A("", NULL, ("Directly Addressed Message\n"));
		} else {
			SII_LOG1A("", NULL, ("Broadcast Message\n"));
		}

		SII_LOG1A("", NULL, ("Opcode Received: 0x%x\n", pMsg->opcode));
		break;
	}

	SiiCecEvtCbFunc(eventFlags);

	return false;
}

bool_t SiiDrvCecSetLogicalAddr ( uint8_t logicalAddress )
{
	uint8_t capture_address[2];
	uint8_t capture_addr_sel = 0x01;

	SII_LOG1A("", NULL, ("CEC Logical Address: %d..", logicalAddress));
	capture_address[0] = 0;
	capture_address[1] = 0;
	if ( logicalAddress == 0xFF )
	{
		logicalAddress = 0x0F;	/* unregistered LA*/
	}
	else if ( logicalAddress < 8 )
	{
		capture_addr_sel = capture_addr_sel << logicalAddress;
		capture_address[0] = capture_addr_sel;
	}
	else
	{
		capture_addr_sel   = capture_addr_sel << ( logicalAddress - 8 );
		capture_address[1] = capture_addr_sel;
	}

	/* Set Capture Address*/
	SiiDrvCraBlockWrite8(SII_9612_I2C_ADDRESS_TX,REG_CEC_CAPTURE_ID0,
			capture_address, 2);

	/* Set device logical address for transmit*/
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX,REG_CEC_TX_INIT,
			logicalAddress);

	return( true );
}

/***** public type definitions ***********************************************/
void sCecInit(SiiCecLogicalAddresses_t devLogicalAddr)
{
	SII_LOG1A("", NULL, ("CEC Initializing.."));
	SiiDrvCraWrite8( SII_9612_I2C_ADDRESS_TX, REG_CEC_CONFIG_CPI,
			CLEAR_BITS );

	/* Clear any pre-existing junk from the RX FIFO*/
	SiiDrvCraBitsSet8( SII_9612_I2C_ADDRESS_TX, REG_CEC_RX_CONTROL,
			BIT_CLR_RX_FIFO_ALL, false );

	/* Clear any existing interrupts*/
	SiiDrvCraBitsSet8( SII_9612_I2C_ADDRESS_TX, REG_CEC_INT_STATUS_0,
		BIT_RX_MSG_RECEIVED | BIT_TX_FIFO_EMPTY | BIT_TX_MESSAGE_SENT,
		true );
	SiiDrvCraBitsSet8( SII_9612_I2C_ADDRESS_TX, REG_CEC_INT_STATUS_1,
		(BIT_FRAME_RETRANSM_OV | BIT_SHORT_PULSE_DET |
		BIT_START_IRREGULAR | BIT_RX_FIFO_OVERRUN), true );

	/* Enable the interrupts within the CEC hardware block*/
	SiiDrvCraBitsSet8( SII_9612_I2C_ADDRESS_TX,
		REG_CEC_INT_ENABLE_0,
		BIT_RX_MSG_RECEIVED | BIT_TX_FIFO_EMPTY | BIT_TX_MESSAGE_SENT,
		true );
	SiiDrvCraBitsSet8( SII_9612_I2C_ADDRESS_TX, REG_CEC_INT_ENABLE_1,
		(BIT_FRAME_RETRANSM_OV | BIT_SHORT_PULSE_DET |
		BIT_START_IRREGULAR | BIT_RX_FIFO_OVERRUN),
		true );

	SiiDrvCecSetLogicalAddr((uint8_t) devLogicalAddr);
}

static bool_t SiiDrvCecSend ( SiiCecData_t *pMsg  )
{
	uint8_t cecStatus[2];

	/* Clear Tx-related interrupts; write 1 to bits to be cleared.*/
	cecStatus[0] = BIT_TX_BUFFER_FULL | BIT_TX_MESSAGE_SENT |
			BIT_TX_FIFO_EMPTY;
	cecStatus[1] = BIT_FRAME_RETRANSM_OV;
	SiiDrvCraBlockWrite8(SII_9612_I2C_ADDRESS_TX,
			REG_CEC_INT_STATUS_0, cecStatus, 2 );

	/* Special handling for a special opcode.*/
	if ( pMsg->opcode == CECOP_SII_SENDPING )
	{
		SiiDrvCraWrite8( SII_9612_I2C_ADDRESS_TX, REG_CEC_TX_DEST,
				BIT_SEND_POLL | pMsg->srcDestAddr);
	}
	else
	{
		/* Send the command*/
		SiiDrvCraWrite8( SII_9612_I2C_ADDRESS_TX, REG_CEC_TX_DEST,
				pMsg->srcDestAddr & 0x0F );
		SiiDrvCraWrite8( SII_9612_I2C_ADDRESS_TX, REG_CEC_TX_COMMAND,
				pMsg->opcode );
		SiiDrvCraBlockWrite8( SII_9612_I2C_ADDRESS_TX,
				REG_CEC_TX_OPERAND_0,
				pMsg->args, pMsg->argCount );
		SiiDrvCraWrite8( SII_9612_I2C_ADDRESS_TX,
				REG_CEC_TRANSMIT_DATA,
				BIT_TRANSMIT_CMD | pMsg->argCount );
	}

	return TRUE;
}

/******************************************************************************/
/**
 * Function:    SiiDrvCpiRead
 *  Description: Reads a CEC message from the CPI read FIFO, if present.
 *   Returns:     true if valid message, false if an error occurred
*******************************************************************************/
void PrintRecMsg(SiiCecData_t *pMsg)
{
	uint8_t i;
	SII_LOG1A("", NULL,
			("opcode: %02X, argCount: %02X, srcDestAddr: %02X, \n",
					pMsg->opcode,
					pMsg->argCount,
					pMsg->srcDestAddr));

	for (i=0; i < pMsg->argCount; i++)
	{
		SII_LOG1A("", NULL, ("%02X.", pMsg->args[i]));
	}
	SII_LOG1A("", NULL, ("\n"));
}

bool_t SiiDrvCecReceive( SiiCecData_t *pMsg )
{
	bool_t success = true;
	uint8_t argCount;

	argCount = SiiDrvCraRead8( SII_9612_I2C_ADDRESS_TX, REG_CEC_RX_COUNT );

	if ( argCount & BIT_MSG_ERROR ) {
		SII_LOG1A("", NULL, ("Error: Wrong arg Count: 0x%x.",
				argCount));
		success = false;
	} else {
		/*SII_LOG1A("", NULL, ("argCount: 0x%x.", argCount));*/

		pMsg->argCount = argCount & 0x0F;
		pMsg->srcDestAddr = SiiDrvCraRead8(SII_9612_I2C_ADDRESS_TX,
				REG_CEC_RX_CMD_HEADER );
		pMsg->opcode = (SiiCecOpcodes_t)SiiDrvCraRead8(
				SII_9612_I2C_ADDRESS_TX,
				REG_CEC_RX_OPCODE );
		if ( pMsg->argCount ) {
			SiiDrvCraBlockRead8(SII_9612_I2C_ADDRESS_TX,
					REG_CEC_RX_OPERAND_0,
					pMsg->args,
					pMsg->argCount );
		}

		/*SII_LOG1A("", NULL, ("Opcode: 0x%x.", pMsg->opcode));*/
	}

	SiiDrvCraBitsSet8( SII_9612_I2C_ADDRESS_TX,
			REG_CEC_RX_CONTROL,
			BIT_CLR_RX_FIFO_CUR,
			true );
	
	pCec->pMsgHandler(pMsg);

	return success;
}


void sCecInt( void )
{
	uint8_t cecStatus[2];
	SiiCecData_t cecMsg;

	SiiDrvCraBlockRead8( SII_9612_I2C_ADDRESS_TX,
			REG_CEC_INT_STATUS_0,
			cecStatus,
			2);

	if ( (cecStatus[0] & 0x7F) || cecStatus[1] ) {
		/* Clear interrupts*/

		/*SII_LOG1A("", NULL, ("CEC intr: 0x%x 0x%x", cecStatus[0],
					cecStatus[1]));*/

		if ( cecStatus[1] & BIT_FRAME_RETRANSM_OV ) {
			/* Flush TX, otherwise after clearing the
			*  BIT_FRAME_RETRANSM_OV interrupt,
			*  the TX command will be re-sent. */

			SiiDrvCraBitsSet8( SII_9612_I2C_ADDRESS_TX,
					REG_CEC_DEBUG_3,
					BIT_FLUSH_TX_FIFO,
					true );
		}

		/* Clear interrupt bits that are set*/
		SiiDrvCraBlockWrite8( SII_9612_I2C_ADDRESS_TX,
				REG_CEC_INT_STATUS_0,
				cecStatus,
				2 );

		/* RX Processing*/
		if ( cecStatus[0] & BIT_RX_MSG_RECEIVED ) {
			/* pCpi->cecStatus.rxState = 1;*/
			/* Flag caller that CEC frames are present in RX FIFO*/
			SiiDrvCecReceive(&cecMsg);
		}

		/* TX Processing*/
		if ( cecStatus[0] & BIT_TX_MESSAGE_SENT ) {
			/*pCpi->cecStatus.txState = SiiTX_SENDACKED;*/
		}

		if ( cecStatus[1] & BIT_FRAME_RETRANSM_OV ) {
			/*pCpi->cecStatus.txState = SiiTX_SENDFAILED;*/
		}
		/*if ((cecStatus[1] & BIT_FRAME_RETRANSM_OV) &&
				(cecStatus[0] & 0x10)) {
			SII_LOG1A("", NULL, ("\n"));
			SII_LOG1A("", NULL, ("**** ERROR:****\n"));
			SII_LOG1A("", NULL, ("*CEC Msg Send Failed\n"));
			SII_LOG1A("", NULL, ("*CEC Line may be open\n"));
			SII_LOG1A("", NULL, ("*PLACE RESISTOR R1 On SK Board"));
			SII_LOG1A("", NULL, ("\n"));
		}*/
	}
}

void SiiDrvCecCreate(SiiCecDeviceTypes_t dev_type,
		Sii943xEventCallbackFunc_t pCallBack )
{
	uint16_t i = 0;

	SiiCecEvtCbFunc = pCallBack;

	/* Initialize the internal data structures used by CEC*/
	SII_MEMSET(pCec, 0, sizeof(CecInstanceData_t));

	pCec->deviceType = dev_type;
	pCec->legacyArcEnable = false;


	/* Mark all devices as not found */
	for ( i = 0; i <= CEC_LOGADDR_UNREGORBC; i++ )
	{
		pCec->logicalDeviceInfo[ i].cecPA = 0xFFFF;
		pCec->logicalDeviceInfo[ i].deviceType = CEC_DT_COUNT;
	}

	switch (dev_type) {
	case CEC_DT_AUDIO_SYSTEM:
		pCec->devLogicalAddr = CEC_LOGADDR_AUDSYS;
		pCec->pMsgHandler = &CecTxMsgHandler;
		SiiCecSetDevicePA(0x1000);;
		break;
	case CEC_DT_TV:
		pCec->devLogicalAddr = CEC_LOGADDR_TV;
		pCec->pMsgHandler = &CecRxMsgHandler;
		SiiCecSetDevicePA(0x0000);
		break;
	default:
		pCec->devLogicalAddr = CEC_LOGADDR_AUDSYS;
		pCec->physicalAddr = 0x1000;
		pCec->pMsgHandler = &CecTxMsgHandler;
		break;
	}

	sCecInit(pCec->devLogicalAddr);

	SII_LOG1A("", NULL,
			("CEC Create Success: DevType: %d DevLA: %d",
					dev_type, pCec->devLogicalAddr));
}

void SiiDrvCecHandle( void )
{
	sCecInt();
}

bool_t SiiDrvCecPASet( uint16_t cec_pa )
{
	pCec->physicalAddr = cec_pa;

	return true;
}

bool_t SiiDrvCecArcRxStart( void )
{
	SiiCecData_t cecFrame;
	uint8_t sourceLA;

	sourceLA = CEC_LOGADDR_TV;

	SiiCecSendReportPhysicalAddress( pCec->devLogicalAddr,
			pCec->physicalAddr );

	sii_sys_time_milli_delay(100);

	/*  send the command.*/
	cecFrame.opcode         = CECOP_INITIATE_ARC;
	cecFrame.srcDestAddr    = MAKE_SRCDEST(pCec->devLogicalAddr,
						sourceLA );
	cecFrame.argCount       = 0;
	if (SiiDrvCecSend(&cecFrame))
		SII_LOG1A("", NULL,
			("Sent --> INITIATE_ARC\n"));

	return TRUE;
}

bool_t SiiDrvCecArcRxStop( void )
{
	SiiCecData_t cecFrame;
	uint8_t sourceLA = CEC_LOGADDR_TV;

	/*  send the command.*/
	cecFrame.opcode         = CECOP_TERMINATE_ARC;
	cecFrame.srcDestAddr    = MAKE_SRCDEST(pCec->devLogicalAddr,
						pCec->logicalAddr );
	cecFrame.argCount       = 0;
	if (SiiDrvCecSend(&cecFrame))
		SII_LOG1A("", NULL,
			("Sent --> TERMINATE_ARC: srcLA: %d, destLA:%d\n",
					pCec->devLogicalAddr,
					sourceLA));
	return TRUE;
}

bool_t SiiDrvCecArcTxStart( void )
{
	pCec->legacyArcEnable = true;

	if (!isDsPARcvd) {
		SiiCecGetDsPhysicalAddress(pCec->devLogicalAddr,
			CEC_LOGADDR_AUDSYS);
	} else if (pCec->legacyArcEnable){
		SiiCecRequestArcInitiationSend(pCec->devLogicalAddr,
				pCec->logicalAddr);
	}

	return TRUE;
}

bool_t SiiDrvCecArcTxStop( void )
{
	SiiCecData_t cecFrame;

	pCec->legacyArcEnable = false;

	/* send the command.*/
	cecFrame.opcode         = CECOP_REQUEST_ARC_TERMINATION;
	cecFrame.srcDestAddr    = MAKE_SRCDEST(pCec->devLogicalAddr,
				pCec->logicalAddr);
	cecFrame.argCount       = 0;
	if (SiiDrvCecSend(&cecFrame))
		SII_LOG1A("", NULL,
			("Sent --> REQUEST_ARC_TERMINATION\n"));
	return TRUE;
}

static void SiiCecSendVendorId(  SiiCecLogicalAddresses_t srcLa,
		SiiCecLogicalAddresses_t destLa,uint8_t *vendorId )
{
	SiiCecData_t cecFrame;

	cecFrame.opcode        = CECOP_DEVICE_VENDOR_ID;
	cecFrame.srcDestAddr   = MAKE_SRCDEST( srcLa, destLa );
	cecFrame.args[0]       = vendorId[0];
	cecFrame.args[1]       = vendorId[1];
	cecFrame.args[2]       = vendorId[2];
	cecFrame.argCount      = 3;

	if (SiiDrvCecSend(&cecFrame))
		SII_LOG1A("", NULL, ("Sent --> DEVICE_VENDOR_ID\n"));
}

static void SiiCecSendGiveDeviceVendorId(SiiCecLogicalAddresses_t srcLa,
					SiiCecLogicalAddresses_t destLa)
{
	SiiCecData_t cecFrame;

	cecFrame.opcode        = CECOP_GIVE_DEVICE_VENDOR_ID;
	cecFrame.srcDestAddr   = MAKE_SRCDEST( srcLa, destLa );
	cecFrame.argCount      = 0;

	if (SiiDrvCecSend(&cecFrame))
		SII_LOG1A("", NULL,
			("Sent --> GIVE_DEVICE_VENDOR_ID\n"));
}

/******************************************************************************/
/**
* @brief      Store the LA, PA, and type of the specified device.
* @param[in]  newLA       - CEC logical address of device to add
*                         - If bit 7 is set, this is a virtual device
* @param[in]  newPA       - CEC physical address of device to add
* @param[in]  isActive    - true if this device is the active source
*******************************************************************************/
static void SiiCecUpdateLogicalDeviceInfo ( uint8_t newLA, uint16_t newPA,
		bool_t isActive )
{
	int_t   portIndex;
	bool_t  isVirtual;

	isVirtual = ((newLA & SII_BIT7) != 0);
	newLA &= ~SII_BIT7;
	if ( newLA > CEC_LOGADDR_UNREGORBC )
	{
		pCec->lastResultCode = RESULT_CEC_INVALID_LOGICAL_ADDRESS;
		return;
	}

	/* Determine actual child port.*/
	portIndex = ((newPA & pCec->paChildMask ) >> pCec->paShift) - 1;
	if (portIndex < 0)
	{
		portIndex = 0;
	}

	if ( portIndex < 1 )
	{
		pCec->logicalDeviceInfo[newLA].cecPA      = newPA;
		pCec->logicalDeviceInfo[newLA].port       = portIndex;
		pCec->logicalDeviceInfo[newLA].selected   = false;
		pCec->logicalDeviceInfo[newLA].isVirtual  = isVirtual;
		pCec->logicalDeviceInfo[newLA].deviceType = l_devTypes[newLA];
		pCec->logicalAddr = (SiiCecLogicalAddresses_t)newLA;

		pCec->logicalDeviceInfo[newLA].selected = true;
		pCec->lastResultCode = RESULT_CEC_SUCCESS;
	}
	else
	{
		pCec->logicalDeviceInfo[newLA].cecPA      = 0xFFFF;
		pCec->logicalDeviceInfo[newLA].port       = 2;
		pCec->logicalDeviceInfo[newLA].selected   = false;
		pCec->logicalDeviceInfo[newLA].isVirtual  = false;
		pCec->logicalDeviceInfo[newLA].deviceType = CEC_DT_COUNT;

		pCec->lastResultCode = RESULT_CEC_INVALID_PHYSICAL_ADDRESS;
	}
}


/******************************************************************************/
/**
* @brief      Broadcast a REPORT PHYSICAL ADDRESS message.
*              Does not wait for a reply.
* @param[in]  srcLa     - source CEC logical address
* @param[in]  srcPa     - source CEC physical address
* @return     uint16_t    16 bit message ID or 0 if CEC not enabled
*******************************************************************************/
static uint16_t SiiCecSendReportPhysicalAddress ( SiiCecLogicalAddresses_t srcLa,
		uint16_t srcPa )
{
	SiiCecData_t cecFrame;

	cecFrame.opcode         = CECOP_REPORT_PHYSICAL_ADDRESS;
	cecFrame.srcDestAddr    = MAKE_SRCDEST( srcLa, CEC_LOGADDR_UNREGORBC );
	cecFrame.args[0]        = srcPa >> 8;	/* [Physical Address] High*/
	cecFrame.args[1]        = srcPa & 0xFF;	/* [Physical Address] Low*/
	cecFrame.args[2]        = l_devTypes[srcLa];	/* [Device Type]*/
	cecFrame.argCount       = 3;

	if (SiiDrvCecSend(&cecFrame)) {
		SII_LOG1A("", NULL,
				("Sent --> REPORT_PHYSICAL_ADDRESS\n"));
		SII_LOG1A("", NULL,
				("Logical Addr:%d, PA: 0x%x\n", srcLa, srcPa));
	}
	return 0;
}

/* send GivePhysicalAddress Command to downstream.*/
static uint16_t SiiCecGetDsPhysicalAddress (SiiCecLogicalAddresses_t srcLa,
		SiiCecLogicalAddresses_t desLa)
{
	SiiCecData_t cecFrame;

	cecFrame.opcode         = CECOP_GIVE_PHYSICAL_ADDRESS;
	cecFrame.srcDestAddr    = MAKE_SRCDEST( srcLa, desLa );
	cecFrame.argCount       = 0;
	if (SiiDrvCecSend(&cecFrame))
		SII_LOG1A("", NULL,
				("Sent --> GIVE_PHYSICAL_ADDRESS\n"));
	return 0;
}

/******************************************************************************/
/**
* @brief      Sends "CEC Set System Audio Mode" message to report
*             the SAC mode (on/off)
*
* @param[in]  destLogAddr - logical address of the destination device (TV,STB)
*******************************************************************************/
static void SiiCecSacSystemAudioModeStatusSend (SiiCecLogicalAddresses_t srcLa,
		const uint8_t destLogAddr)
{
	SiiCecData_t cecFrame;
	/*Bug 43214 - LG TV is expecting this status to be 1.*/
	cecFrame.args[0] =0x01;	/* SAC mode status*/

	cecFrame.opcode          = CECOP_SYSTEM_AUDIO_MODE_STATUS;
	cecFrame.srcDestAddr     = MAKE_SRCDEST(srcLa, destLogAddr);
	cecFrame.argCount        = 1;

	if (SiiDrvCecSend(&cecFrame))
		SII_LOG1A("", NULL,
				("Sent --> SYSTEM_AUDIO_MODE_STATUS\n"));
}

/******************************************************************************/
/**
* Function:    SiiCecSetDevicePA
* Description: Set the host device physical address(initiator physical address)
*******************************************************************************/
static void SiiCecSetDevicePA (uint16_t devPa)
{
	uint8_t     index;
	uint16_t    mask;

	pCec->physicalAddr = devPa;

	/* We were given the Host, or parent, PA, so we determine   */
	/* the direct child Physical Address field (A.B.C.D).       */

	mask = 0x00F0;
	for ( index = 1; index < 4; index++ )
	{
		if (( devPa & mask ) != 0)
			break;
		mask <<= 4;
	}

	pCec->paShift = (index - 1) * 4;
	pCec->paChildMask = 0x000F << pCec->paShift;
}

/******************************************************************************/
/**
* @brief      Sends "CEC Request ARC Initiation" message.
*
* @param[in]  destLogAddr - logical address of the destination device
*******************************************************************************/
static void SiiCecRequestArcInitiationSend (SiiCecLogicalAddresses_t srcLa,
		const uint8_t destLogAddr)
{
	SiiCecData_t cecFrame;
	/*  send the response.*/
	cecFrame.opcode         = CECOP_REQUEST_ARC_INITIATION;
	cecFrame.srcDestAddr    = MAKE_SRCDEST( srcLa, destLogAddr );
	cecFrame.argCount       = 0;
	if (SiiDrvCecSend(&cecFrame)) {
		SII_LOG1A("", NULL,
				("Sent --> REQUEST_ARC_INITIATION\n"));
		SII_LOG1A("", NULL, ("srcla: %d destla: %d\n",
				srcLa, destLogAddr));
	}
}

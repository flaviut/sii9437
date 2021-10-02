
/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * $Revision: 1 $
 * $Date: 12/08/20 11:27a $
 * @brief    Main function of this demo is to test I2C master .
 *
 * @note
 * Copyright (C) 2011 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/

#include <stdio.h>
//#include "NUC123Series.h"
#include "GPIO.h"
#include "string.h"
#include "I2C.h"
#include "Vcom_Data_Proc.h"
#include "I2C_MASTER.h"
#include "LibTimer.h"

#define I2C_WR               0x00
#define I2C_RD               0x01

#define FrameSize      256

unsigned char g_txBuf[FrameSize] = {0};
unsigned char g_rxBuf[FrameSize] = {0};


static uint16_t txByte_TotalCount=0;
static uint16_t	rxByte_TotalCount=0;
static uint16_t txByte_TotalSendCount=0;
static uint16_t	rxByte_TotalRcvCount=0;
static uint8_t offset_size = 2;
uint8_t I2C_SLA = 0x40;

/* The Length of Data that already been Sent to BB */
static uint8_t u8DataSendlen;
/*-------------------------------------------*/
/*        Internal Variable Definition       */
/*-------------------------------------------*/
/*static uint8_t u8Device_Addr;
static uint8_t u8Reg_AddrH;
static uint8_t u8Reg_AddrL;*/
static uint8_t u8Reg_Addr_Len = 0;





/* The Length of Data that should be Received from BB */
// static uint8_t u8TotalRecvLen;
/* The Length of Data that already been Received from BB */
static uint16_t u8DataRecvLen;

static eI2C_Status_t volatile i2c_op_status = I2C_STATUS__NONE;

/*-------------------------------------------*/
/*              Enum Definition              */
/*-------------------------------------------*/

typedef enum
{
    eI2C_ERROR_NO_ERROR = 0,
    eI2C_ERROR_BB_READY_BIT_NOT_SET,
    eI2C_ERROR_SEQUENCE_ID_INCORRECT,
    eI2C_ERROR_CHECKSUM_ERROR,
    eI2C_ERROR_WAIT_RESPONSE_TIMEOUT,
    eI2C_ERROR_NUM_OF_ERROR
}eI2C_ERROR_TYPE;

static eI2C_ERROR_TYPE volatile eI2CErrorType = eI2C_ERROR_NO_ERROR;

uint8_t rx_tx=0;


//static int s_I2C_Proc( uint8_t u8DeviceAddr, uint8_t u8RegAddr, eI2C_PROC_TYPE eI2cProcType );

/*
int I2C_Master(void)
{
	uint8_t data[256] = {0};
	uint16_t i;


    Init_I2C();
    data[0] = 0x88;
    data[1] = 0x88;
    data[2] = 0x88;
    data[3] = 0x88;
    sii_i2c_write(0x60, 0x0004, data, 4); // Banner registers - tested and working fine
    for (i = 0; i < 256; i++)
    	data[i] = 0;
    sii_i2c_read(0x60, 0x0000, data, 256);// Banner registers - tested and working fine



    return 0;
}*/


/*********************************************
@brief:This function executes I2C Data Read/Write Main Process

@param:[IN] pu8CmdBuf : IN - I2C Command Buffer
       [IN] u32CmdLen : IN - Command Length

@return:int :
*********************************************/
void I2C_Cmd_Data_Send(uint8_t *pu8CmdBuf, uint32_t u32CmdLen )
{
	/**
	* IN MSG
	* byte 0: 0x0A - start of msg
	* byte 1: [4:0] - header len - 5
	*         [7:4] - Interface Type to be used. 0 - I2C, 1 - SPI
	*-----------------------------
	* IN header:
	* byte 2: msgCount
	* byte 3: OPCODE -
	*		0xE0 - READ BLOCK- 8 BIT ADDRESS SPACE
	*		0x60 - WRITE BLOCK- 8 BIT ADDRESS SPACE
	*		0x9F - READ BLOCK- 16 BIT ADDRESS SPACE
	*		0x1F - READ BLOCK- 16 BIT ADDRESS SPACE
	* ------------------------------------------
	* IN DATA
	* byte 4: 1 - operation success. 0 - operation failure.
	* For write it can be 0/1
	* byte 5: device id. 0x00 in case of SPI.
	* byte 6: address len
	* byte 7-8: address
	* byte 9: data len high byte
	* byte 10: data len low byte
	* ...
	* -----------------------------------------
	* byte last: 0x0D -- end of msg
	*/
 	/**
	* OUT MSG
	* byte 0: 0x0A - start of msg
	* byte 1: [4:0] - header len - 5
	*         [7:4] - Interface Type to be used. 0 - I2C, 1 - SPI
	*-----------------------------
	* OUT MSG header: Below bytes are data from TX MSG used for comparision
	* byte 2: msgCount
	* byte 3: OPCODE -
	*		0xE0 - READ BLOCK- 8 BIT ADDRESS SPACE
	*		0x60 - WRITE BLOCK- 8 BIT ADDRESS SPACE
	*		0x9F - READ BLOCK- 16 BIT ADDRESS SPACE
	*		0x1F - READ BLOCK- 16 BIT ADDRESS SPACE
	* ------------------------------------------
	* OUT MSG DATA
	* byte 4: 1 - operation success. 0 - operation failure.
	* For write it can be 0/1
	* byte 5: device id. 0x00 in case of SPI.
	* byte 6: address len
	* byte 7-8: address
	* byte 9: data len high byte
	* byte 10: data len low byte
	* byte 13: data len low byte
	* ...
	* -----------------------------------------
	* byte last: 0x0D -- end of msg
	*/
	//int i;
	//uint8_t u8DataBuf[256] = {0x00};
	//uint32_t u32PayloadLen;
	//uint32_t u32MessageLen;
	//uint8_t payloadIndex;
	//uint8_t regAddrLen;
	char rw_op = 'r';
	uint8_t hdrLen = 0;
	uint8_t dataIndex = 0;
	uint8_t devId = 0;
	uint8_t op_status = 0;
	uint8_t lRegAddrH = 0;
	uint8_t lRegAddrL = 0;
	uint16_t regAddr = 0;

	uint16_t dataLen = 0;

	uint8_t op_type;
	uint8_t packetType = pu8CmdBuf[0];
	uint8_t u8DataRecvBuf[300];
	uint8_t u8DataSendBuf[16];
	uint16_t u16Data_Req_Len = 0;
	uint32_t u32DataLen = 0;

	if (packetType == 0x0A)
	{
		hdrLen = pu8CmdBuf[1] & 0x0F;
		op_type = pu8CmdBuf[hdrLen+1];
		dataIndex = hdrLen + 2;
		op_status = pu8CmdBuf[dataIndex];
		devId = pu8CmdBuf[dataIndex+1];
		lRegAddrH = pu8CmdBuf[dataIndex+3];
		lRegAddrL = pu8CmdBuf[dataIndex+4];
		regAddr = (lRegAddrH << 8) | lRegAddrL;
		dataLen = ((pu8CmdBuf[dataIndex+5] << 8) |
				pu8CmdBuf[dataIndex+6]);
		u8DataRecvLen = dataIndex + 7;
		/*response packet intialization*/
		memcpy(u8DataRecvBuf, pu8CmdBuf, u8DataRecvLen);
		switch (op_type)
		{
		case 0xE0:
			rw_op = 'r';
			u8Reg_Addr_Len = 1;
			break;
		case 0x60:
			rw_op = 'w';
			u8Reg_Addr_Len = 1;
			memcpy(u8DataSendBuf, pu8CmdBuf+u8DataRecvLen, dataLen);
			break;
		case 0x9F:
			rw_op = 'r';
			u8Reg_Addr_Len = 2;
			break;
		case 0x1F:
			rw_op = 'w';
			u8Reg_Addr_Len = 2;
			memcpy(u8DataSendBuf, pu8CmdBuf+u8DataRecvLen, dataLen);
			break;
		default:
			return;
		}
	}
	else if (packetType == 0xFF)
	{
		hdrLen = 0;
		op_type = pu8CmdBuf[hdrLen+1];
		dataLen = pu8CmdBuf[hdrLen+2];
		devId = pu8CmdBuf[hdrLen+3];
		u8DataRecvLen = 3;
		/*response packet intialization*/
		memcpy(u8DataRecvBuf, pu8CmdBuf, u8DataRecvLen);
		switch (op_type)
		{
		case 0xE0:
			rw_op = 'r';
			u8Reg_Addr_Len = 1;
			lRegAddrH = 0;
			lRegAddrL = pu8CmdBuf[hdrLen+4];
			dataLen = pu8CmdBuf[hdrLen+5];
			break;
		case 0x60:
			rw_op = 'w';
			u8Reg_Addr_Len = 1;
			lRegAddrH = 0;
			lRegAddrL = pu8CmdBuf[hdrLen+4];
			dataLen = pu8CmdBuf[hdrLen+5];
			u8DataRecvLen = 2+pu8CmdBuf[hdrLen+2];
			memcpy(u8DataSendBuf, pu8CmdBuf+u8DataRecvLen, dataLen);
			break;
		case 0x9F:
			rw_op = 'r';
			u8Reg_Addr_Len = 2;
			lRegAddrH = pu8CmdBuf[hdrLen+4];
			lRegAddrL = pu8CmdBuf[hdrLen+5];
			dataLen = pu8CmdBuf[hdrLen+6];
			break;
		case 0x1F:
			rw_op = 'w';
			u8Reg_Addr_Len = 2;
			lRegAddrH = pu8CmdBuf[hdrLen+4];
			lRegAddrL = pu8CmdBuf[hdrLen+5];
			dataLen = pu8CmdBuf[hdrLen+6];
			u8DataRecvLen = 2+pu8CmdBuf[hdrLen+2];
			memcpy(u8DataSendBuf, pu8CmdBuf+u8DataRecvLen, dataLen);
			break;
		case 0x11:
		case 0x91:
			break;
		default:
			return;
		}
		regAddr = (lRegAddrH << 8) | lRegAddrL;
	}
	else
		return;

	/* Initilize ERROR Type */
	eI2CErrorType = eI2C_ERROR_NO_ERROR;
	u16Data_Req_Len = dataLen;


	switch (op_type)
	{
	case 0xE0:
	case 0x60:
	case 0x9F:
	case 0x1F:
		if(rw_op == 'w')
		{
			u8DataSendlen = 0;
			if( sii_i2c_write( devId, regAddr,
					u8Reg_Addr_Len,
					u8DataSendBuf,
					u16Data_Req_Len) == FALSE )
			{
				op_status = 0;
			}
			else
			{
				op_status = 1;
			}
		}
		else
		{
			if( sii_i2c_read( devId, regAddr,
					u8Reg_Addr_Len,
					u8DataRecvBuf+u8DataRecvLen,
					u16Data_Req_Len) == FALSE )
			{
				op_status = 0;
			}
			else
			{
				op_status = 1;
				u8DataRecvLen += u16Data_Req_Len;
			}
		}

		if (packetType == 0x0A)
		{
			u8DataRecvBuf[dataIndex] = op_status;
			u32DataLen = u8DataRecvLen + 1; // +1 for end of packet
			u8DataRecvBuf[u32DataLen - 1] = 0x0D;
		}
		else if (packetType == 0xFF)
		{
			/*no response for write*/
			if(rw_op == 'w')
			{
				u8DataRecvBuf[2] = 	1;
				u8DataRecvBuf[3] = 	dataLen;
				u32DataLen = 4;
			}
			else
			{
				u8DataRecvBuf[2] = 	dataLen;
				u32DataLen = u8DataRecvLen;
			}
		}
		break;
	case 0x11:
		Vcom_Firmware_comm_mode_set(&devId);

		u8DataRecvBuf[2] = 	1;
		u8DataRecvBuf[3] = 	devId;
		u32DataLen = 4;
		break;
	case 0x91:
		Vcom_Firmware_comm_mode_query(&devId);
		u8DataRecvBuf[2] = 	devId;
		u32DataLen = 3;
		break;
	default:
		return;
	}

	/*send response packet*/
	Vcom_Data_Return_Proc( u8DataRecvBuf, u32DataLen );
	return;
}

void I2C_Port_Open(I2C_T *I2cPort, uint32_t u32BusClock)
{
	if( I2cPort == I2C0 )
	{
		GPIO_SET_MODE(PF, 2, GPIO_OPEN_DRAIN );
		GPIO_SET_MODE(PF, 3, GPIO_OPEN_DRAIN );
		GPIO_I2C0();
		DrvI2C_Init(I2C0, u32BusClock, TRUE);
	}
	else if( I2cPort == I2C1 )
	{
		GPIO_I2C1();	//configure PA.10 and PA.11 as I2C function
		DrvI2C_Init(I2C1, u32BusClock, TRUE);
	}
	else
	{
		/* Do Nothing */
	}

	return;
}

static int s_I2C_Proc( uint8_t u8DeviceAddr, uint16_t u8RegAddr,
		uint8_t offsetsize,
		eI2C_PROC_TYPE eI2cProcType )
{
	uint8_t i2c_retry = 3;

	offset_size = offsetsize;
	do {
		i2c_op_status = I2C_STATUS__NONE;
		switch(offset_size) {
		case 2:
			g_txBuf[0]= (u8RegAddr >> 8) & 0xFF;
			g_txBuf[1]= (u8RegAddr) & 0xFF;
			break;
		case 1:
			g_txBuf[0]= (u8RegAddr) & 0xFF;
			break;
		default:
			break;
		}
		I2C_SLA = u8DeviceAddr >> 1;
		LibTimerSetTimer(eTIMER0_COUNT_USE_FOR_I2C, 1800);
		I2C_START(I2C0);
		while( (i2c_op_status == I2C_STATUS__NONE) &&
			(!LibTimerIfTimerExpired(eTIMER0_COUNT_USE_FOR_I2C)));
		if (i2c_op_status == I2C_STATUS__NONE)
		{
			I2C_STOP(I2C0);
			i2c_op_status = I2C_STATUS__TIMEOUT;
		}
		else if (i2c_op_status == I2C_STATUS__SUCCESS)
		{
			return TRUE;
		}
		printf("i2c failed RegAddr  = 0x%X%X VALUE = %X\n",
				u8DeviceAddr,
		u8RegAddr, i2c_op_status);
		i2c_retry--;
	} while(i2c_retry);
	return FALSE;
}


BOOL sii_i2c_write(uint8_t devId, uint16_t offset, uint8_t offset_size,
		const uint8_t *pData, uint16_t len)
{
	int count =0;
	rx_tx = 0;
	for(count = 0;count<len;count++)
		g_txBuf[count + offset_size]=pData[count];
	txByte_TotalCount = len + offset_size;
	txByte_TotalSendCount = 0;
	rxByte_TotalCount = 0;
	rxByte_TotalRcvCount = 0;
	if( s_I2C_Proc( devId, offset, offset_size, eI2C_DATA_SEND ) == FALSE )
	{
		return FALSE;
	}
	return TRUE;
}

BOOL sii_i2c_read(uint8_t devId, uint16_t offset, uint8_t offset_size,
		uint8_t *pData, uint16_t len)
{
	int count =0;
	rx_tx = 1;
	for (;count < len; count++)
		g_rxBuf[count] = 0;
	txByte_TotalCount = offset_size;
	txByte_TotalSendCount = 0;
	rxByte_TotalCount = len;
	rxByte_TotalRcvCount = 0;
	if( s_I2C_Proc( devId, offset, offset_size, eI2C_DATA_RECV ) == FALSE )
	{
		return FALSE;
	}
	else
	{
		for (count = 0; count < len; count++)
			pData[count] = g_rxBuf[count];
	}
	return TRUE;
}
/**
 * @brief       I2C1 ISR routine
 *
 * @param       None
 *
 * @return      None
 *
 * @details     transmit and recieve data in ISR routine
 */
void I2C1IntHandler(void)
{

	uint32_t status =I2C_GET_I2CSTATUS(I2C1);   //中：先读出状态字节//en：Read the status first

	switch(status)
	{
	case 0x08 :	//en:START has been sent
		I2C_SET_DATA(I2C1,(I2C_SLA<<1) | I2C_WR);
		I2C_CLEAR_SI(I2C1);
		break;
	case 0x48:
	case 0x20 :	//en:SLA+W has been sent,No ACK bit will be received
		i2c_op_status = I2C_STATUS__NACK;
		I2C_STOP(I2C1);
		I2C_CLEAR_SI(I2C1);
		break;
	case 0x18 :	//en:SLA+W has been sent,ACK bit will be received
		I2C_SET_DATA(I2C1,g_txBuf[txByte_TotalSendCount++]);   //en: sent first data
		I2C_CLEAR_SI(I2C1);
		break ;
	case 0x28 :	//en: data has sent, ack back
		if((rx_tx==0) && (txByte_TotalCount == txByte_TotalSendCount))
		{
			I2C_STOP(I2C1);
			I2C_CLEAR_SI(I2C1); //address transmission finished in write op
			i2c_op_status = I2C_STATUS__SUCCESS;
			break;
		}
		if((rx_tx==1) && (txByte_TotalSendCount == (offset_size - 1)))
		{
			rx_tx = 2;
			I2C_SET_DATA(I2C1,g_txBuf[txByte_TotalSendCount++]);
			I2C_CLEAR_SI(I2C1); //address transmission finished in read op
		}
		else if(( (rx_tx==1) && (txByte_TotalCount == txByte_TotalSendCount)) || (rx_tx==2))
		{
			I2C_START(I2C1); //repeated start in read op
			I2C_CLEAR_SI(I2C1);
		}
		else
		{
			I2C_SET_DATA(I2C1,g_txBuf[txByte_TotalSendCount++]);
			I2C_CLEAR_SI(I2C1); //address transmission finished in write op
		}
		break;
	case 0x30 :	//en: last data	has sent,No ack back
		I2C_STOP(I2C1);
		I2C_CLEAR_SI(I2C1);
		i2c_op_status = I2C_STATUS__NACK;
		break ;
	case 0x10 :	//en: A repeat start has been transmitted
		I2C_SET_DATA(I2C1,(I2C_SLA<<1) | I2C_RD);
		I2C_CLEAR_SI(I2C1);
		break ;
	case 0x38 :		//en: Arbitration lost in SLA+R/W or data byte
		i2c_op_status = I2C_STATUS__BUS_BUSY;
		break ;
	//---------------------------------------------------------------
	case 0x40 :
	#if 1
		//en:SLA+R has been sent
		if(rxByte_TotalCount == 1)
		{
			I2C_CLEAR_AA(I2C1);	//en:  last data,no ack back
		}
		else
		{
			I2C_SET_AA(I2C1);
		}
			I2C_CLEAR_SI(I2C1);	//中:清零SI//en: Clear SI
		break ;
	#endif
	case 0x50 : 			//en: data has received, ack back
		g_rxBuf[rxByte_TotalRcvCount++]=I2C_GET_DATA(I2C1);
		if((rxByte_TotalCount - rxByte_TotalRcvCount) == 1)
		{
			I2C_CLEAR_AA(I2C1);	//en:  last data,no ack back
		}
		else
		{
			I2C_SET_AA(I2C1);
		}
		I2C_CLEAR_SI(I2C1);
		break ;

	case 0x58 :	//en: last data has received, no ack back
		g_rxBuf[rxByte_TotalRcvCount++]=I2C_GET_DATA(I2C1);
		I2C_STOP(I2C1);
		i2c_op_status = I2C_STATUS__SUCCESS;
		break ;

	default :
		/*i2c_op_status = I2C_STATUS__NACK;*/
		 I2C_CLEAR_SI(I2C1);
		 break ;
	}
}

/**
 * @brief       I2C0 ISR routine
 *
 * @param       None
 *
 * @return      None
 *
 * @details     transmit and recieve data in ISR routine
 */
void I2C0IntHandler(void)
{

	uint32_t status =I2C_GET_I2CSTATUS(I2C0);   //中：先读出状态字节//en：Read the status first

	switch(status)
	{
	case 0x08 :	//en:START has been sent
		I2C_SET_DATA(I2C0,(I2C_SLA<<1) | I2C_WR);
		I2C_CLEAR_SI(I2C0);
		break;
	case 0x48:
	case 0x20 :	//en:SLA+W has been sent,No ACK bit will be received
		i2c_op_status = I2C_STATUS__NACK;
		I2C_STOP(I2C0);
		I2C_CLEAR_SI(I2C0);
		break;
	case 0x18 :	//en:SLA+W has been sent,ACK bit will be received
		I2C_SET_DATA(I2C0,g_txBuf[txByte_TotalSendCount++]);   //en: sent first data
		I2C_CLEAR_SI(I2C0);
		break ;
	case 0x28 :	//en: data has sent, ack back
		if((rx_tx==0) && (txByte_TotalCount == txByte_TotalSendCount))
		{
			I2C_STOP(I2C0);
			I2C_CLEAR_SI(I2C0); //address transmission finished in write op
			i2c_op_status = I2C_STATUS__SUCCESS;
			break;
		}
		if((rx_tx==1) && (txByte_TotalSendCount == (offset_size - 1)))
		{
			rx_tx = 2;
			I2C_SET_DATA(I2C0,g_txBuf[txByte_TotalSendCount++]);
			I2C_CLEAR_SI(I2C0); //address transmission finished in read op
		}
		else if(( (rx_tx==1) && (txByte_TotalCount == txByte_TotalSendCount)) || (rx_tx==2))
		{
			I2C_START(I2C0); //repeated start in read op
			I2C_CLEAR_SI(I2C0);
		}
		else
		{
			I2C_SET_DATA(I2C0,g_txBuf[txByte_TotalSendCount++]);
			I2C_CLEAR_SI(I2C0); //address transmission finished in write op
		}
		break;
	case 0x30 :	//en: last data	has sent,No ack back
		I2C_STOP(I2C0);
		I2C_CLEAR_SI(I2C0);
		i2c_op_status = I2C_STATUS__NACK;
		break ;
	case 0x10 :	//en: A repeat start has been transmitted
		I2C_SET_DATA(I2C0,(I2C_SLA<<1) | I2C_RD);
		I2C_CLEAR_SI(I2C0);
		break ;
	case 0x38 :		//en: Arbitration lost in SLA+R/W or data byte
		i2c_op_status = I2C_STATUS__BUS_BUSY;
		break ;
	//---------------------------------------------------------------
	case 0x40 :
	#if 1
		//en:SLA+R has been sent
		if(rxByte_TotalCount == 1)
		{
			I2C_CLEAR_AA(I2C0);	//en:  last data,no ack back
		}
		else
		{
			I2C_SET_AA(I2C0);
		}
			I2C_CLEAR_SI(I2C0);	//中:清零SI//en: Clear SI
		break ;
	#endif
	case 0x50 : 			//en: data has received, ack back
		g_rxBuf[rxByte_TotalRcvCount++]=I2C_GET_DATA(I2C0);
		if((rxByte_TotalCount - rxByte_TotalRcvCount) == 1)
		{
			I2C_CLEAR_AA(I2C0);	//en:  last data,no ack back
		}
		else
		{
			I2C_SET_AA(I2C0);
		}
		I2C_CLEAR_SI(I2C0);
		break ;

	case 0x58 :	//en: last data has received, no ack back
		g_rxBuf[rxByte_TotalRcvCount++]=I2C_GET_DATA(I2C0);
		I2C_STOP(I2C0);
		i2c_op_status = I2C_STATUS__SUCCESS;
		break ;

	default :
		/*i2c_op_status = I2C_STATUS__NACK;*/
		 I2C_CLEAR_SI(I2C0);
		 break ;
	}
}

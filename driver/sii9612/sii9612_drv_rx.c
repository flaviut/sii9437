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
/*************************************************************************
* si_drv_9612_rx.c
*************************************************************************/
#define SII_DEBUG 3

#include "sii_system_api.h"
#include "sii_sys_time_api.h"
#include "sii9612_drv_api.h"
#include "sii_sys_log_api.h"
#include "sii_sys_assert_api.h"

/********* Module name **************************************************/

SII_MODULE_NAME_SET(9612Rx);

typedef struct
{
	uint8_t 			gEdidDs[SII_EDID_LENGTH];
	bool_t 				prvClkdet;
	bool_t 				prvSyncdet;

	bool_t 				AC3;
	bool_t 				HBRA;
	bool_t 				DSD;
	uint8_t 			FS;
	uint8_t 			LAYOUT;
	uint8_t				word_length;

	SiiDrvCallBack 		pDrvCallBack;
	enum SiiAudioFormat gPhalanxRxAudType;
	bool_t 		gCompressedAudio;
	SiiChannelStatus_t 	channelStatus;
	SiiAifExtract		aifExt;
} Obj_t;


/***** Local macro definitions **********************************************/

#if defined(SII9612_INT_METHOD)
#define SII9612_TOP_INTR_STATE			0x01
#define SII9612_INTR3_AVIF_CHANGE		0x01
#define SII9612_INTR3_AIF_CHANGE		0x04
#endif

FsTableStruc fsTable[] = 
{
	{ 0x00, "44.1Khz" },
	{ 0x02, "48Khz" },
	{ 0x03, "32Khz" },
	{ 0x0B, "64 KHz" },
	{ 0x08, "88.2Khz" },
	{ 0x0A, "96Khz" },
	{ 0x0C, "176.4Khz" },
	{ 0x0E, "192Khz" },
	{ 0x2B, "128 KHz" },
	{ 0x09, "4x192Khz" }
};

/***** local functions definitions **********************************************/
static void sRxInit(void);
/*static void printEdidData(uint8_t *pEdid);*/
static void sRxHandler( void );
static void sGetAudioType( void );
static void sGetChannelStatus( void );
static bool_t sRxGetAudioStatus( void );
static bool_t sGetClkStatus( void );
static bool_t sIsClkChange( void );
static void PrintChannelStatus (SiiChannelStatus_t *pChStatus);
static void sAifExtractGet( void );
static bool_t sRxGetAifStatus( void );
static void sSetHpd(bool_t en);
static void sAviInfoFrameSet( void );
void sInitTitan( void );
static void sResetState( void );
static void sii9612_rx_hpd_set(bool_t en);

/****************** static variables definitions ******************************/
static Obj_t* spObj = NULL;

static void sii9612_cec_tx_notification(uint32_t evt_flgs_notify)
{
	SiiDrv9612Event_t rxEvent=0;

	if (evt_flgs_notify & SII_9612_CEC_EVENT_FLAG__ARC_INIT) {
		rxEvent |= SII_9612_EVENT_FLAG__ARC_INIT;
	}

	if (evt_flgs_notify & SII_9612_CEC_EVENT_FLAG__ARC_TERM) {
		rxEvent |= SII_9612_EVENT_FLAG__ARC_TERM;
	}

	if (rxEvent)
	{
		spObj->pDrvCallBack(rxEvent);
	}
}

uint32_t sii9612_rx_cec_arc_tx_start(void)
{
	SiiDrvCecArcTxStart();

	return 0;
}

uint32_t sii9612_rx_cec_arc_tx_stop(void)
{
	SiiDrvCecArcTxStop();

	return 0;
}

uint32_t sii9612_rx_cec_pa_set(uint16_t cec_pa)
{
	SiiDrvCecPASet(cec_pa);

	return 0;
}

/*****************************************************************************/
/**
* @brief 9612 Rx driver constructor
*
* @param[in]  pNameStr   Name of instance
* @param[in]  pCallBack  call back function
*
* @retval                Handle to instance
*
*****************************************************************************/
void sii9612_rx_create(SiiDrvCallBack pCallBack)
{
	/* Enforce module to be used as singleton only */
	SII_ASSERT(!spObj);

	/* Allocate memory for object */
	spObj = (Obj_t*)SiiSysObjSingletonCreate(SII_MODULE_NAME_GET(),
				SII_INST_NULL, sizeof(Obj_t));
	SII_ASSERT(spObj);

	spObj->pDrvCallBack	= pCallBack;
	sResetState();

	sRxInit();
	sInitTitan();

	sii9612_rx_hpd_set(true);

	SiiDrvCecCreate(CEC_DT_TV,
			&sii9612_cec_tx_notification);
}
void sii9612_rx_delete( void )
{
	if (spObj) {
		sii_sys_obj_singleton_delete(spObj);
		spObj = NULL;
	}
}

void sii9612_rx_i2s_channel_status_query(SiiChannelStatus_t *pChannelStatus)
{
	sGetAudioType();
	memcpy(pChannelStatus, &spObj->channelStatus.b[0], sizeof(SiiChannelStatus_t)) ;
	PrintChannelStatus(pChannelStatus);
}

void sii9612_rx_edid_get(uint8_t *pEdid)
{
	SiiDrvCraBlockWrite8(SII_9612_I2C_ADDRESS_RX, 0x1B00, pEdid, SII_EDID_LENGTH);
}

void sii9612_rx_edid_audio_set(  SiiEdidAudioData_t *pAudEdid  )
{
	sSetHpd(FALSE);
	SII_LOG3A("", NULL, ("Set 9612 Rx EDID\n"));
	SiiLibEdidAudioDataSet(gEdidData, pAudEdid);
	SiiDrvCraBlockWrite8(SII_9612_I2C_ADDRESS_RX, 0x1B00, gEdidData, SII_EDID_LENGTH);
	sSetHpd(TRUE);
}

static void sii9612_rx_hpd_set(bool_t en)
{
	sSetHpd(en);
}

void sii9612_rx_handle( void )
{
	sRxHandler();
#if !defined(SII9612_INT_METHOD)
	SiiDrvCecHandle();
#else
}

void sii9612_rx_cec_handle( void )
{
	SiiDrvCecHandle();
#endif
}

/* Testing Purpose */
void SiiDrvRxPhalanxAudioMuteSet(bool_t en)
{
	SiiDrvCraBitsSet8(SII_9612_I2C_ADDRESS_RX, 0x1129, 0x04, en);
}

void sii9612_rx_aif_extraction_query(SiiAifExtract *pAifExt)
{
	*pAifExt = spObj->aifExt;
}

void sii9612_rx_i2s_audio_format_query(enum SiiAudioFormat *pAudType)
{
	*pAudType = spObj->gPhalanxRxAudType;
}

void sii9612_rx_comp_audio_format_query(bool_t *is_compressed_audio)
{
	*is_compressed_audio = spObj->gCompressedAudio;
}

void SiiDrvRxPhalanxAudioMclkSet(enum sii_mclk_mode mclk_mode)
{
	switch (mclk_mode) {

	case SII_MCLK_MODE__NONE:
		SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x2020, 0x90);
		SiiDrvCraBitsMod8(SII_9612_I2C_ADDRESS_RX, 0x1101, 0x70,  0x00);
		/*SII_LOG3A("", NULL, ("Input Mclk Mode: NONE\n"));*/
		break;
	case SII_MCLK_MODE__FS128:
		SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x2020, 0x80);
		SiiDrvCraBitsMod8(SII_9612_I2C_ADDRESS_RX, 0x1101, 0x70,  0x00);
		/*SII_LOG3A("", NULL, ("Input Mclk Mode: FS128\n"));*/
		break;
	case SII_MCLK_MODE__FS256:
		SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x2020, 0x90);
		SiiDrvCraBitsMod8(SII_9612_I2C_ADDRESS_RX, 0x1101, 0x70,  0x10);
		/*SII_LOG3A("", NULL, ("Input Mclk Mode: FS256\n"));*/
		break;
	case SII_MCLK_MODE__FS512:
		SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x2020, 0xB0);
		SiiDrvCraBitsMod8(SII_9612_I2C_ADDRESS_RX, 0x1101, 0x70,  0x30);
		/*SII_LOG3A("", NULL, ("Input Mclk Mode: FS512\n"));*/
		break;
	default:
		/* Default 256 FS*/
		SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x2020, 0x90);
		SiiDrvCraBitsMod8(SII_9612_I2C_ADDRESS_RX, 0x1101, 0x30,  0x10);
		/*SII_LOG3A("", NULL, ("Input Mclk Mode: Default\n"));*/
		break;
	}

	/* reg_mclk_en = 1*/
	SiiDrvCraBitsSet8(SII_9612_I2C_ADDRESS_RX, 0x1027, 0x08, TRUE);
}

/********************************** local  functions *******************************************************/
static void sRxInit(void)
{
#if defined(SII9612_INT_METHOD)
	//Enable mask for rewuired interupt registers
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1076, 0x18); //ckdt, skdt
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1077, 0x05); //avif, aif
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x107E, 0x12); //hbr, dsd
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1093, 0x80); //avif, aif
#endif

	// enable internal osc
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x2606, 0x10);    // enable ACLK and CEC OSC clock

	// enable video path
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1005, 0x01);	// b0=1: assert rx (sequoia) software reset

	// initialize rx phy (kangag)
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1f13, 0x02);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1f14, 0xc4);	// program eq settings
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1f15, 0xf4);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1f16, 0xe4);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1f00, 0xd4);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1f01, 0xc4);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1f02, 0xa4);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1f03, 0x94);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1f04, 0xf5);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1f12, 0x42);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1f60, 0x42);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1f61, 0x1c);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1f28, 0x8a);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1f29, 0x84);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1fa7, 0x7c);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1f9f, 0x7f);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1fa1, 0x33);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1fa6, 0x61);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1f7a, 0x29);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1f7b, 0x74);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1f7d, 0x05);

	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1F79, 0x00);	// b2.0=0x0: disable deep color mode for rx phy
	//SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1F79, 0x20);	// b5=1: enable h/w control of deep color mode for rx phy

	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x0010, 0x30);	// b5.4=0: video data for tx is from rx 

	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x0090, 0x4c);	// b7.5=0x2: source for voutclk is rx clock

	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x0014, 0x00);	// b1.0=0x0: +ve hs & vs polarity

	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1081, 0x30);	// b2.0=0x0: select 8-bit color depth mode for rx
															// b4=1: use s/w override to select deep color mode
	//SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1081, 0x20);	// b4=0: enable h/w to select deep color mode
	 
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1049, 0x00);	// b7.6=0x0: select 8-bit output for rx

	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x104b, 0x10);	// set blank level of blue ch to 0x10 (16 decimal) for rgb inputs
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x104c, 0x10);	// set blank level of green ch to 0x10 (16 decimal) for rgb inputs
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x104d, 0x10);	// set blank level of red ch to 0x10 (16 decimal) for rgb inputs

	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1009, 0x91);	// enable rx input and ddc i/f

	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1008, 0x07);	// power up rx

	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1005, 0x40);	// b0=0: de-assert rx software reset
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1007, 0x40);	// b0=0: de-assert rx software reset

	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1102, 0x02);

	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x20c7, 0x00);	// switch to h/w tpi mode

	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x2040, 0x01);	// b0=1: assert tx (ekki) software reset

	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x2600, 0xb1);	// initialize tx phy (roog) [wait 10us]
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x2601, 0x38);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x2603, 0x00);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x260c, 0x37);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x260d, 0x01);

	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x2602, 0xa9);	// b2.1=0x0: disable deep color clock

	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x2009, 0x00);	// set input color depth to 8-bit
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x200a, 0x00);	// set output color depth to 8-bit

	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x2040, 0x00);	// disable dc packet

	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x20bf, 0x50);	// select avi infoframe
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x20bf, 0xd0);

	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x20c0, 0x82);	// set up avi infoframe
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x20c1, 0x02);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x20c2, 0x0d);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x20c3, 0x5f);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x20c4, 0x00);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x20c5, 0x00);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x20c6, 0x00);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x20c7, 0x10);	// vic=0x10: 1080p 60hz
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x20c8, 0x00);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x20c9, 0x00);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x20ca, 0x00);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x20cb, 0x00);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x20cc, 0x00);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x20cd, 0x00);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x20ce, 0x00);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x20cf, 0x00);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x20d0, 0x00);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x20d1, 0x00);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x20d2, 0x00);

	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x201a, 0x01);	// enable hdmi mode

	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x2040, 0x00);	// b0=0: de-assert tx (ekki) software reset
					// b2=0: disable dc packet

	// enable audio path
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1127, 0xFC);	// enable multi-channel i2s & spdif & mclk output
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1129, 0x1D);	// enable spdif & i2s audio output
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x112e, 0x02);	// b1: map dsd ch to i2s & spdif output signal pins


	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x300C, 0x00);    // set rx i2s output to tx i2s input

	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x2026, 0x90);    // i2s select, layout 0, mute
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x201F, 0x80);    // fifo0 set to sd0
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x2021, 0x00);          
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x2022, 0x00);         
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x2023, 0x00);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x2024, 0x02);    // audio fs=48khz
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x2025, 0x0B);    // 24 bits
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x2020, 0x90);    // mclk=256, sck invert 
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x2027, 0x18);    // audio fs=48kHz
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x2028, 0x13);

	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x20BF, 0xC2);    // enabled tpi audio infoframe and repeat
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x20C0, 0x84);    // audio infoframe
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x20C1, 0x01);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x20C2, 0x0A);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x20C3, 0x70);    // byte 0 - check sum (c0:cD)
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x20C4, 0x01);    // byte 1 - set to 2ch
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x20C5, 0x1C);    // byte 2 
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x20C6, 0x00);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x20C7, 0x00);    // byte 4 - speaker placement
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x20C8, 0x00);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x20C9, 0x00);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x20CA, 0x00);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x20CB, 0x00);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x20CC, 0x00);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x20CD, 0x00);

	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x2026, 0x80);    // i2s, layout 0, Unmute
			
	SiiDrvCraBlockWrite8(SII_9612_I2C_ADDRESS_RX, 0x1B00, gEdidData, SII_EDID_LENGTH);

}

bool_t sSamplingFreqValid(uint8_t fs)
{
	uint8_t i;
	uint8_t length = 8;
	for (i=0; i < length; i++)
	{
		if (fsTable[i].fs == (fs & 0x0F))
			return TRUE;
	}
	return FALSE;
}

char *sGettingFreqName(uint8_t fs)
{
	uint8_t i;
	uint8_t length = 8; 
	for (i=0; i < length; i++)
	{
		if (fsTable[i].fs == fs)
			return fsTable[i].fsName;
	}
	return "Invalid Freq";
}


static void PrintChannelStatus (SiiChannelStatus_t *pChStatus)
{
	SII_LOG3A("", spObj, ("(%s).%02x.%02x.%02x.%02x.%02x.%02x.%02x.%02x.%02x", sGettingFreqName(pChStatus->b[3]),
		pChStatus->b[0],  pChStatus->b[1],  pChStatus->b[2],  pChStatus->b[3],
		pChStatus->b[4],  pChStatus->b[5],  pChStatus->b[6],  pChStatus->b[7], pChStatus->b[8]));
}

static void sAviInfoChange( void )
{
#if !defined(SII9612_INT_METHOD)
	uint8_t tmp = SiiDrvCraRead8(SII_9612_I2C_ADDRESS_RX, 0x1073) & 0x01;

	if (tmp)
	{
#endif
		sAviInfoFrameSet();
#if !defined(SII9612_INT_METHOD)
		SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1073, 0x01);
	}
#endif
}

static bool_t sIsClkChange( void )
{
	uint8_t tmp = SiiDrvCraRead8(SII_9612_I2C_ADDRESS_RX, 0x1006);
	bool_t syncdet = tmp & 0x01 ? true : false;
	bool_t clkdet = tmp & 0x02 ? true : false;
	bool_t resutl = false;
	if ((spObj->prvSyncdet != syncdet) ||  (spObj->prvClkdet != clkdet))
	{
		spObj->prvSyncdet = syncdet;
		spObj->prvClkdet = clkdet;
		sResetState();
		SII_LOG2A("", spObj, ("9612 clk is %s", syncdet & clkdet ? "hi" : "lo"));
		resutl = true;
	}

	return resutl;
}

static bool_t sGetClkStatus( void )
{
	uint8_t tmp;
	bool_t syncdet;
	bool_t clkdet;
	tmp = SiiDrvCraRead8(SII_9612_I2C_ADDRESS_RX, 0x1006);
	syncdet = tmp & 0x01 ? true : false;
	clkdet = tmp & 0x02 ? true : false;
	return (clkdet &&  syncdet);
}

static void sGetChannelStatus( void )
{
    spObj->channelStatus.b[0] = SiiDrvCraRead8(SII_9612_I2C_ADDRESS_RX, 0x112A);
    spObj->channelStatus.b[1] = SiiDrvCraRead8(SII_9612_I2C_ADDRESS_RX, 0x112B);
    spObj->channelStatus.b[2] = SiiDrvCraRead8(SII_9612_I2C_ADDRESS_RX, 0x112C);
    spObj->channelStatus.b[3] = SiiDrvCraRead8(SII_9612_I2C_ADDRESS_RX, 0x1117) & 0x0F; 
    spObj->channelStatus.b[4] = SiiDrvCraRead8(SII_9612_I2C_ADDRESS_RX, 0x1131); 
    spObj->channelStatus.b[5] = 0;
    spObj->channelStatus.b[6] = 0;
	spObj->channelStatus.b[7] = 0;
	spObj->channelStatus.b[8] = 0;

	if (FALSE == sSamplingFreqValid(spObj->channelStatus.b[3])) {

		/* If wrong channel status was read, give 100ms delay and
		 *  read channel status again*/
		sii_sys_time_milli_delay(100);

	    spObj->channelStatus.b[0] = SiiDrvCraRead8(SII_9612_I2C_ADDRESS_RX, 0x112A);
	    spObj->channelStatus.b[1] = SiiDrvCraRead8(SII_9612_I2C_ADDRESS_RX, 0x112B);
	    spObj->channelStatus.b[2] = SiiDrvCraRead8(SII_9612_I2C_ADDRESS_RX, 0x112C);
	    spObj->channelStatus.b[3] = SiiDrvCraRead8(SII_9612_I2C_ADDRESS_RX, 0x1117) & 0x0F;
	    spObj->channelStatus.b[4] = SiiDrvCraRead8(SII_9612_I2C_ADDRESS_RX, 0x1131);
	    spObj->channelStatus.b[5] = 0;
	    spObj->channelStatus.b[6] = 0;
		spObj->channelStatus.b[7] = 0;
		spObj->channelStatus.b[8] = 0;
	}

	sAviInfoFrameSet();
}

static void sAifExtractGet( void )
{
	/*spObj->aifExt.cc = SiiDrvCraRead8(SII_9612_I2C_ADDRESS_RX, 0x1184) & 0x7;*/
	spObj->aifExt.ca = SiiDrvCraRead8(SII_9612_I2C_ADDRESS_RX, 0x1187);
	spObj->aifExt.lfe = SiiDrvCraRead8(SII_9612_I2C_ADDRESS_RX, 0x1188) & 0x3;
	spObj->aifExt.lsv = (SiiDrvCraRead8(SII_9612_I2C_ADDRESS_RX, 0x1188) & 0x78) >> 3;
	spObj->aifExt.dm_inh = (SiiDrvCraRead8(SII_9612_I2C_ADDRESS_RX, 0x1188) & 0x80) >> 7;
}

static bool_t sRxGetAifStatus( void )
{
#if !defined(SII9612_INT_METHOD)

	bool_t aif;

	aif	 = SiiDrvCraRead8(SII_9612_I2C_ADDRESS_RX, 0x1073) & 0x04;
	if (aif)
	{
		SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1073, 0x04);
#endif
		sAifExtractGet();
		return true;
#if !defined(SII9612_INT_METHOD)
	}
	return false;
#endif
}


static bool_t sRxGetAudioStatus( void )
{
	bool_t hbra;
	bool_t ac3;
	bool_t dsd;
	uint8_t aud;
	uint8_t fs;
	/*uint8_t layout;*/
	uint8_t word_length;

	aud	= SiiDrvCraRead8(SII_9612_I2C_ADDRESS_RX, 0x1134);
	dsd	= (aud & 0x20) ? true : false;
	hbra	= (aud & 0x40) ? true : false;
	/*layout	= aud & 0x18;*/

	fs = SiiDrvCraRead8(SII_9612_I2C_ADDRESS_RX, 0x1117) & 0xF;
	ac3 = (SiiDrvCraRead8(SII_9612_I2C_ADDRESS_RX, 0x112A) & 0x2) ?
			true : false;

	word_length = (SiiDrvCraRead8(SII_9612_I2C_ADDRESS_RX, 0x1131)) & 0x0F;

	if ((spObj->AC3 != ac3) || (spObj->HBRA != hbra) ||
		(spObj->FS != fs) || (spObj->DSD != dsd) ||
		/*(spObj->LAYOUT != layout) ||*/
		(spObj->word_length != word_length))
	{
		sii_sys_time_milli_delay(25);
		/* Read once again to get updated values */
		aud	= SiiDrvCraRead8(SII_9612_I2C_ADDRESS_RX, 0x1134);
		dsd	= (aud & 0x20) ? true : false;
		hbra	= (aud & 0x40) ? true : false;
		/*layout	= aud & 0x18;*/

		fs = SiiDrvCraRead8(SII_9612_I2C_ADDRESS_RX, 0x1117) & 0xF;
		ac3 = (SiiDrvCraRead8(SII_9612_I2C_ADDRESS_RX, 0x112A) & 0x2) ?
				true : false;
		word_length = (SiiDrvCraRead8(SII_9612_I2C_ADDRESS_RX,
				0x1131)) & 0x0F;

		if ((spObj->AC3 != ac3) || (spObj->HBRA != hbra) ||
				(spObj->FS != fs) || (spObj->DSD != dsd) ||
				/*(spObj->LAYOUT != layout) ||*/
				(spObj->word_length != word_length))
			{
				spObj->AC3 	= ac3;
				spObj->HBRA 	= hbra;
				spObj->FS 	= fs;
				spObj->DSD     = dsd;
				/*spObj->LAYOUT  = layout;*/
				spObj->word_length = word_length;

				return true;
			}
	}

	return false;
}

static void sAviInfoFrameSet( void )
{
	uint8_t i;

	for (i=0; i < 14; i++)
	{
		SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x200c+i, SiiDrvCraRead8(SII_9612_I2C_ADDRESS_RX, 0x1143+i));
	}
}

static void sGetAudioType( void )
{
	uint8_t aud		= SiiDrvCraRead8(SII_9612_I2C_ADDRESS_RX, 0x1134);
	bool_t dsd		= aud & 0x20;
	bool_t hbr		= aud & 0x40;
	uint8_t layout	= aud & 0x18;

	if (dsd)
	{
		SII_LOG2A("", spObj, ("9612 dsd"));
		spObj->gPhalanxRxAudType = SII_AUDIO_FORMAT__DSD;
		sGetChannelStatus();
	}
	if (hbr)
	{
		SII_LOG2A("", spObj, ("9612 hbra"));
		spObj->gPhalanxRxAudType = SII_AUDIO_FORMAT__HBRA;
		sGetChannelStatus();
	}
	if (!dsd & !hbr)
	{
		uint8_t aud = SiiDrvCraRead8(SII_9612_I2C_ADDRESS_RX, 0x112A);	
		spObj->gPhalanxRxAudType = SII_AUDIO_FORMAT__LAYOUT_0;
		if (aud & 0x2)
		{
			SII_LOG2A("", spObj, ("9612 ac3"));
			spObj->gCompressedAudio = 1;

		}
		else
		{
			SII_LOG2A("", spObj, ("9612 lpcm"));
			spObj->gCompressedAudio = 0;
		}
		sGetChannelStatus();
		if (layout)
			spObj->gPhalanxRxAudType = SII_AUDIO_FORMAT__LAYOUT_1;
	}
}

static void sRxHandler( void)
{
	SiiDrv9612Event_t rxEvent=0;
#if !defined(SII9612_INT_METHOD)
	if (sIsClkChange())
		rxEvent |= SII_9612_EVENT_FLAG__CLK_CHANGE;

	if (sGetClkStatus()) {
		if (sRxGetAudioStatus())
			rxEvent |= SII_9612_EVENT_FLAG__AUD_CHANGE;

		if (sRxGetAifStatus())
			rxEvent |= SII_9612_EVENT_FLAG__AIF_CHANGE;

		sAviInfoChange();
	}
#else
	uint8_t top_intr = 0;
	uint8_t intr_sckdt = 0;
	uint8_t intr_avchange = 0;
	uint8_t intr_hbrdsd = 0;
	uint8_t intr_aud_chastate = 0;

	top_intr = SiiDrvCraRead8(SII_9612_I2C_ADDRESS_RX, 0x1070);

	if(top_intr & SII9612_TOP_INTR_STATE) {
		SII_LOG3A("", NULL, ("__Processing 9612 interrupts__\n"));

		intr_sckdt = SiiDrvCraRead8(SII_9612_I2C_ADDRESS_RX, 0x1072);
		SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1072, intr_sckdt);
		if (intr_sckdt & 0x18) {
			SII_LOG3A("", NULL, ("CKDT or SCDT Changed = 0x%x\n",
					(intr_sckdt & 0x18)));
			if (sIsClkChange()) {
				// b0=1: assert rx (sequoia) software reset
				SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX,
						0x1005, 0x01);
				SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX,
						0x1005, 0x40);

				rxEvent |= SII_9612_EVENT_FLAG__CLK_CHANGE;
			}
		}

		intr_hbrdsd = SiiDrvCraRead8(SII_9612_I2C_ADDRESS_RX, 0x107C);
		SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x107C, intr_hbrdsd);
		intr_aud_chastate = SiiDrvCraRead8(SII_9612_I2C_ADDRESS_RX,
				0x1091);
		SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1091,
				intr_aud_chastate);
		if((intr_hbrdsd & 0x12) || (intr_aud_chastate & 0x80)) {
			SII_LOG3A("", NULL,
				("hbr_dsd = 0x%02x, AudChState = 0x%02x\n",
				(intr_hbrdsd & 0x12),
				(intr_aud_chastate & 0x80)));
			if (sGetClkStatus()) {
				if (sRxGetAudioStatus())
					rxEvent |=
						SII_9612_EVENT_FLAG__AUD_CHANGE;
			}
		}

		intr_avchange = SiiDrvCraRead8(SII_9612_I2C_ADDRESS_RX, 0x1073);
		SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x1073, intr_avchange);
		if(intr_avchange & SII9612_INTR3_AIF_CHANGE) {
			SII_LOG3A("", NULL, ("AIF changed\n"));
			if (sGetClkStatus()) {
				if (sRxGetAifStatus())
				rxEvent |= SII_9612_EVENT_FLAG__AIF_CHANGE;
			}
		}
		if(intr_avchange & SII9612_INTR3_AVIF_CHANGE) {
			SII_LOG3A("", NULL, ("AVI IF changed\n"));
			if (sGetClkStatus()) {
				sAviInfoChange();
			}
		}
	}
#endif
	if (rxEvent)
		 spObj->pDrvCallBack(rxEvent);
}

static void sSetHpd(bool_t en)
{
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_RX, 0x19EC, en ? 0x01 : 0x00);
	SII_LOG2A("", spObj, ("9612 hpd %s", en ? "hi" : "lo"));
}

static void sResetState( void )
{
	spObj->AC3		= false;
	spObj->HBRA		= false;
	spObj->FS		= false;
	spObj->DSD 		= false;
	spObj->LAYOUT		= false;
	spObj->word_length	= 0x00;
	/*spObj->aifExt.cc	= 0x00;*/
	spObj->aifExt.ca	= 0x00;
	spObj->aifExt.lfe	= 0x00;
	spObj->aifExt.lsv	= 0x00;
	spObj->aifExt.dm_inh	= 0x00;
}
/*
static void printEdidData(uint8_t *pEdid)
{
	uint16_t i;
	return;
	for (i=0; i <SII_EDID_LENGTH ; i++)
	{
		if ((i % 16) == 0)
			printf("\n%02X", i);
		printf(" %02X", pEdid[i]);
	}
}
*/

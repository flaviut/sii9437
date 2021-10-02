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
* si_drv_phalanx.c
*************************************************************************/
#define SII_DEBUG 3

#define BIT_MASK__TWO_CHANNEL_WITHOUT_PRE_EMPHASIS	0x00
#define BIT_MASK__TWO_CHANNEL_WITH_PRE_EMPHASIS		0x01
#define BIT_MASK__MULTI_CHANNEL_LPCM			0x20
#define BIT_MASK__NON_LPCM				0x00
#define BIT_MASK__DSD					0x12

#include "sii_system_api.h"
#include "sii_drv_cra_api.h"
#include "sii9612_drv_api.h"
#include "sii_api_wrap.h"
#include "sii9437_api.h"
#include "sii_sys_assert_api.h"
#include "sii_sys_time_api.h"
#include "sii9612_drv_cec_api.h"

/***** Register Module name **************************************************/
SII_MODULE_NAME_SET(9612Tx);

/*Default EDID */

static uint8_t default_edid[SII_EDID_LENGTH] =
{
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
	0x4D, 0x29, 0x12, 0x96, 0x01, 0x00, 0x00, 0x00,
	0x20, 0x15, 0x01, 0x03, 0x80, 0x52, 0x2E, 0x78,
	0x0A, 0x0D, 0xC9, 0xA0, 0x57, 0x47, 0x98, 0x27,
	0x12, 0x48, 0x4C, 0x21, 0x09, 0x00, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3A,
	0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C,
	0x45, 0x00, 0xC4, 0x8E, 0x21, 0x00, 0x00, 0x1E,
	0x01, 0x1D, 0x00, 0x72, 0x51, 0xD0, 0x1E, 0x20,
	0x6E, 0x28, 0x55, 0x00, 0xC4, 0x8E, 0x21, 0x00,
	0x00, 0x1E, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x43,
	0x50, 0x39, 0x36, 0x38, 0x37, 0x0A, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFD,

	0x00, 0x17, 0x78, 0x0F, 0x85, 0x1E, 0x00, 0x0A,
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0x6A,
    0x02, 0x03, 0x37, 0x72, 0x55, 0x90, 0x84, 0x03,
    0x02, 0x0E, 0x0F, 0x07, 0x23, 0x24, 0x05, 0x94,
    0x13, 0x12, 0x11, 0x1D, 0x1E, 0x20, 0x21, 0x22,
    0x01, 0x1F, 0x26, 0x09, 0x7F, 0x07, 0x15, 0x07,
    0x50, 0x83, 0x01, 0x00, 0x00, 0x6E, 0x03, 0x0C,
    0x00, 0x10, 0x00, 0xF8, 0x3C, 0x21, 0x84, 0x80,
    0x01, 0x02, 0x03, 0x04, 0xE2, 0x00, 0xFF, 0x8C,
    0x0A, 0xD0, 0x90, 0x20, 0x40, 0x31, 0x20, 0x0C,
    0x40, 0x55, 0x00, 0xC4, 0x8E, 0x21, 0x00, 0x00,
    0x18, 0x01, 0x1D, 0x80, 0x18, 0x71, 0x1C, 0x16,
    0x20, 0x58, 0x2C, 0x25, 0x00, 0xC4, 0x8E, 0x21,
    0x00, 0x00, 0x9E, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90
};


typedef struct
{
	SiiDrvCallBack 				pDrvCallBack;
	SiiChannelStatus_t 			channelstatus_input;
	SiiChannelStatus_t 			channelstatus;
	uint8_t 				edid[SII_EDID_LENGTH];
	bool_t 					pHpd;
	sii9437_extraction_mode			audExt;
	SiiAifExtract 				aif;
	bool_t 					hbra;
	enum sii_arc_mode 			arc_mode;
	uint8_t					cc;
} Obj_t;

/***** public macro definitions **********************************************/

static void sTxInit( void );
static bool_t sTxHpd(void);

/***** local functions definitions **********************************************/
static void sTxInit( void );
static void sGetDownStreamEdid(uint8_t *pEdid);
static void sAviInfoFrameSet( void );
static void sAudioInfoFrameSet( enum SiiAudioFormat aType );
static void sUpdateInfoFrame( enum SiiAudioFormat aType );
static void sUpdateChannelStatus( void );
static void sTxHandler(void);
static bool_t sTxHpd(void);
static void sUpdateAudio( void );
static bool_t s_get_audio_type( enum SiiAudioFormat aud_type);
static bool_t s_is_hbra( void );
static bool_t s_is_dsd( void );
static bool_t s_is_non_lpcm( void );

/****************** static variables definitions ******************************/
static Obj_t* spObj = NULL;

static void sii9612_cec_rx_notification(uint32_t evt_flgs_notify)
{
	SiiDrv9612Event_t txEvent=0;


	if (evt_flgs_notify & SII_9612_CEC_EVENT_FLAG__ARC_INIT) {
		txEvent |= SII_9612_EVENT_FLAG__ARC_INIT;
	}

	if (evt_flgs_notify & SII_9612_CEC_EVENT_FLAG__ARC_TERM) {
		txEvent |= SII_9612_EVENT_FLAG__ARC_TERM;
	}

	if (txEvent)
	{
		spObj->pDrvCallBack(txEvent);
	}
}

uint32_t sii9612_tx_cec_arc_rx_start(void)
{
	SiiDrvCecArcRxStart();

	return 0;
}

uint32_t sii9612_tx_cec_arc_rx_stop(void)
{
	SiiDrvCecArcRxStop();

	return 0;
}

uint32_t sii9612_tx_cec_pa_set(uint16_t cec_pa)
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
void sii9612_tx_create(SiiDrvCallBack pCallBack)
{
    /* Enforce module to be used as singleton only */
    SII_ASSERT(!spObj);

    /* Allocate memory for object */
    spObj = (Obj_t*)SiiSysObjSingletonCreate(SII_MODULE_NAME_GET(),
    		SII_INST_NULL, sizeof(Obj_t));
    SII_ASSERT(spObj);

	spObj->pDrvCallBack 	= pCallBack;
	spObj->pHpd		= false;
	spObj->audExt		= SII9437_EXTRACTION_MODE__I2S8;
	spObj->arc_mode		= SII_ARC_MODE__EARC;
	sTxInit();

	SiiDrvCecCreate(CEC_DT_AUDIO_SYSTEM,
			&sii9612_cec_rx_notification);
}

void sii9612_tx_delete( void )
{
	if (spObj) {
		sii_sys_obj_singleton_delete(spObj);
		spObj = NULL;
	}
}

void sii9612_tx_channel_status_set(SiiChannelStatus_t *pChannelStatus)
{
	memcpy(&spObj->channelstatus_input, pChannelStatus, sizeof(SiiChannelStatus_t));
	memcpy(&spObj->channelstatus, pChannelStatus, sizeof(SiiChannelStatus_t));
	/*sUpdateChannelStatus();*/
}

void sii9612_tx_handle( void )
{
	sTxHandler();
	SiiDrvCecHandle();
}

void sii9612_tx_edid_audio_query(  SiiEdidAudioData_t *pAudEdid )
{
	SiiLibEdidAudioDataGet(spObj->edid, pAudEdid);
}

bool_t sii9612_tx_edid_hpd_query( void )
{
	return spObj->pHpd;
}


void sii9612_tx_i2s_audio_format_set( sii9437_extraction_mode audExt )
{
	spObj->audExt = audExt;
	sUpdateChannelStatus();
}

void sii9612_tx_arc_mode_set( enum sii_arc_mode arc_mode )
{
	spObj->arc_mode = arc_mode;
}

const char ch_count_map[50] =
		{2, 3, 3, 4, 3, 4, 4, 5, 4, 5,
		5, 6, 5, 6, 6, 7, 5, 6, 7, 8};

void sii9612_tx_aif_extraction_status_set(SiiAifExtract *p_aif)
{
	spObj->aif = *p_aif;
	spObj->cc = ch_count_map[p_aif->ca] - 1;
	SII_LOG1A("", spObj, ("ca: 0x%x, cc: 0x%x, lfe: %x\n",
			spObj->aif.ca, (spObj->cc + 1), spObj->aif.lfe));
}

void sii9612_tx_set_hbra ( bool_t hbra)
{
	spObj->hbra = hbra;
}
/********************************** local  functions *******************************************************/
static void sAviInfoFrameSet( void )
{
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x200c, 0x2b);	// checksum for avi infoframe
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x200d, 0x40);	// ycbcr color space
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x200e, 0x00);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x200f, 0x00);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x2010, 0x04);	// vic=0x04: 720p 60Hz
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x2011, 0x00);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x2012, 0x00);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x2013, 0x00);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x2014, 0x00);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x2015, 0x00);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x2016, 0x00);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x2017, 0x00);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x2018, 0x00);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x2019, 0x00);
}

static uint8_t sCalCheckSum(uint8_t *pBuf, uint8_t length)
{
	uint8_t i;
	uint8_t chSum=0;

	for (i=0; i < length; i++)
	{
		chSum += pBuf[i];
	}
	chSum = 0 - chSum;
	return chSum;
}

static void sAudioInfoFrameSet( enum SiiAudioFormat aType )
{	
	uint8_t audBuffer[15] = {
			0xC2, 0x84, 0x01, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00
							};
	if (s_get_audio_type(SII_AUDIO_TYPE__DSD6))
	{
		audBuffer[4] = 0x00;		// checksum
		audBuffer[5] = 0x05;		// 6-ch audio
		audBuffer[6] = 0x08;
		audBuffer[7] = 0x00;
		audBuffer[8] = 0x0B;		//  8-ch speaker allocation
		audBuffer[9] = 0x00;
	}
	else if (s_get_audio_type(SII_AUDIO_TYPE__I2S8))
	{
		audBuffer[4] = 0x00;		// checksum
		audBuffer[5] = spObj->cc;
		audBuffer[6] = 0x00;
		audBuffer[7] = 0x00;
		audBuffer[8] = spObj->aif.ca;
		audBuffer[9] = (spObj->aif.lfe) | (spObj->aif.lsv << 3) |
				(spObj->aif.dm_inh << 7);
	}
	audBuffer[4] = sCalCheckSum(&audBuffer[1], sizeof(audBuffer)-1);
	SiiDrvCraBlockWrite8(SII_9612_I2C_ADDRESS_TX, 0x20bf,
			audBuffer, sizeof(audBuffer));
}

void sUpdateInfoFrame( enum SiiAudioFormat aType )
{
	sAviInfoFrameSet();
	sAudioInfoFrameSet(aType);
}


struct fs_table_struc {
	uint8_t		fs;
	char		*fsName;
};

static struct fs_table_struc fs_table[] = {
	{ 0x00, "44.1Khz" },
	{ 0x02, "48Khz" },
	{ 0x03, "32Khz" },
	{ 0x0B, "64 KHz" },
	{ 0x08, "88.2Khz" },
	{ 0x0A, "96Khz" },
	{ 0x2B, "128 KHz" },
	{ 0x0C, "176.4Khz" },
	{ 0x0E, "192Khz" },
	{ 0x09, "4x192Khz" }
};

static char *s_getting_freq_name(uint8_t fs)
{
	uint8_t i;
	uint8_t length = 8;
	for (i = 0; i < length; i++) {
		if (fs_table[i].fs == fs)
			return fs_table[i].fsName;
	}
	return "Invalid Freq";
}

static void PrintChannelStatus (SiiChannelStatus_t *pChStatus)
{
	pChStatus=pChStatus;
	 SII_LOG1A("", spObj,
			("(%s).%02x.%02x.%02x.%02x.%02x",
			s_getting_freq_name(pChStatus->b[3] & 0x0F),
		pChStatus->b[0],  pChStatus->b[1],  pChStatus->b[2],
		pChStatus->b[3], pChStatus->b[4]));
}

static uint8_t sAudioSampleFreqConvert( uint8_t fs )
{
	uint8_t freq=0;
	switch(fs)
	{
		case 0x03:			//32Khz
			freq = (1<<3);
			break;
		case 0x00:			//44.1Khz
			freq =  (2<<3);
			break;
		case 0x02:			//48Khz
			freq =  (3<<3);
			break;
		case 0x08:			//88.2Khz
			freq =  (4<<3);
		case 0x0A:			//96Khz
			freq =  (5<<3);
			break;
		case 0x0C:			//176.4Khz
			freq =  (6<<3);
			break;
		case 0x0E:			//192Khz
			freq =  (7<<3);
			break;
	}
	return freq;
}

static bool_t s_is_hbra( void )
{
	return spObj->hbra;
}

static bool_t s_is_dsd( void )
{
	return ((spObj->channelstatus_input.b[0] & BIT_MASK__DSD) == BIT_MASK__DSD) ? TRUE : FALSE;
}

static bool_t s_is_non_lpcm( void )
{
	bool_t is_nonlpcm;

	if (s_is_hbra())
		return FALSE;
	if (s_is_dsd())
		return FALSE;
	/* is ac3? */
	is_nonlpcm = spObj->channelstatus_input.b[0] & 0x2 ? TRUE : FALSE;

	return is_nonlpcm;
}
/* Note: Need to be Fixed
 * We should use the channel status bits for detecting audio type based on new proposal for Sony.
 * */
static bool_t s_get_audio_type( enum SiiAudioFormat aud_type)
{
	enum SiiAudioFormat aud;

	if (s_is_dsd()) {
		aud = SII_AUDIO_TYPE__DSD6;
		SII_LOG1A("", spObj, ("9612 dsd\n"));
	}
	else if (s_is_hbra()) {
		aud = SII_AUDIO_TYPE__HBRA;
		SII_LOG1A("", spObj, ("9612 hbra\n"));
	}
	else {

		aud = SII_AUDIO_TYPE__I2S8;
		if (s_is_non_lpcm()) {
			SII_LOG1A("", spObj, ("9612 ac3\n"));
		}
		else {
			SII_LOG1A("", spObj, ("9612 lpcm\n"));
		}
	}

	if (aud == aud_type)
		return TRUE;
	return false;
}

static void sChannelStatusConvert( void )
{
	spObj->channelstatus.b[0] &= ~BIT_MASK__MULTI_CHANNEL_LPCM;
	if (s_is_dsd()) {
		spObj->channelstatus.b[0] &= ~BIT_MASK__DSD;
	}
}

static void sUpdateChannelStatus( void )
{
	bool_t layout=false;

	sChannelStatusConvert();

	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x300c, 0x02);	// set i2s 8-ch input to tx
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x2026, 0xB0);	// mute on, layout 1, select i2s mode
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x2606, 0x10);    // enable ACLK and CEC OSC clock

	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x201f, 0x80);	// fifo 0 <- sd0
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x201f, 0x91);	// fifo 1 <- sd1
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x201f, 0xa2);	// fifo 2 <- sd2
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x201f, 0xb3);	// fifo 3 <- sd3
 
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x2020, 0x90);	//Configure i2s bus format

    /* Update channel status in HDMI-Tx */
    SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x2021, spObj->channelstatus.b[0]);
    SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x2022, spObj->channelstatus.b[1]);
    SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x2023, spObj->channelstatus.b[2]);
    SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x2024, spObj->channelstatus.b[3]); // audio fs=48khz
    SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x2025, spObj->channelstatus.b[4]); // 24-bit sample length

	PrintChannelStatus(&spObj->channelstatus);
	
	if ((s_get_audio_type(SII_AUDIO_TYPE__HBRA)) || (spObj->channelstatus.b[3] == 0x09))		//hbra
	{
		SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x2027, 0x3C);	// HBR enabled		
	}
	else if (s_get_audio_type(SII_AUDIO_TYPE__I2S8))	 //lpcm
	{
		uint8_t fs;
		fs = SiiDrvCraRead8(SII_9612_I2C_ADDRESS_TX, 0x2027) & 0xC3;	//set freq
		fs |= sAudioSampleFreqConvert(spObj->channelstatus.b[3]) | 0xC0;
		SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x2027, fs); //PCM
		layout = (spObj->cc == 1) ? false : true;  //channelcount == 1 mean two channels
		if (!s_is_non_lpcm()) {
			if (spObj->cc == 1) {
				SII_LOG1A("", spObj, ("lpcm 2.0 channel"));
			} else {
				SII_LOG1A("", spObj, ("lpcm %d.1 channel", spObj->cc));
			}
		}
		SiiDrvCraBitsSet8(SII_9612_I2C_ADDRESS_TX, 0x2026, 0x20, layout);

		sUpdateAudio();
	}
	else if (s_get_audio_type(SII_AUDIO_TYPE__DSD6))			//dsd
	{
		SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x2026, 0xE0);
		SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x2027, 0x10);	
		SiiDrvCraBitsSet8(SII_9612_I2C_ADDRESS_TX, 0x2026, 0x20, true);
	}
	else									//ac3
	{
		SII_LOG1A("", spObj, ("9612 Default\n"));
		SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x201f, 0x80);
		SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x2027, 0x00);
	}

	SiiDrvCraBitsSet8(SII_9612_I2C_ADDRESS_TX, 0x2026, 0x10, false); //unmute
	sUpdateInfoFrame((enum SiiAudioFormat )spObj->channelstatus.b[SII_EXTRA_CHSTATUS_INDEX]);
}

static void sTxInit( void )
{
	spObj->channelstatus.b[0] = 0x00;
	spObj->channelstatus.b[1] = 0x00;
	spObj->channelstatus.b[2] = 0x00;
	spObj->channelstatus.b[3] = 0x02;
	spObj->channelstatus.b[4] = 0x00;
	spObj->channelstatus.b[5] = 0x00;
	spObj->channelstatus.b[6] = 0x00;
	spObj->channelstatus.b[7] = 0x00;
	spObj->channelstatus.b[8] = 0x00;
	SiiDrvPhalanxTpgCreate();
	sUpdateChannelStatus();
}

static void sGetDownStreamEdid(uint8_t *pEdid)
{
	uint16_t cnt;
	uint16_t x;
	uint8_t i;
	uint8_t k;
	uint8_t val;
	bool_t edid_read_timeout = FALSE;
	uint32_t milli_to;
	uint32_t milli_to1;

	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x20F8, 0x8F); //Abort Cmd
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x20F1, 0xA0); //ddc address
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x20F2, 0x00); //ddc segment addr
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x20F3, 0x00); //ddc offset
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x20F5, 0x00); //ddc Cnt1
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x20F6, 0x01); //ddc Cnt2
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x20F8, 0x89); //ddc cmd ; clear fifo=9
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x20F8, 0x82);  //ddc cmd ;enhance = 4; sequential=2

	x = 0;
	k = 0;

	sii_sys_time_out_milli_set(&milli_to, 1600);

	while (x < SII_EDID_LENGTH)
	{
		cnt = SiiDrvCraRead8(SII_9612_I2C_ADDRESS_TX, 0x20F9);
		if (cnt != 0)
		{
			x = x + cnt;
			i = 0;

			sii_sys_time_out_milli_set(&milli_to1, 100);

			while (i < cnt)
			{
				val = SiiDrvCraRead8(SII_9612_I2C_ADDRESS_TX, 0x20F4);	
				i = i + 1;
				pEdid[k++] = val;
				/* Check if EDID Read Timer Count is expired */
				if( sii_sys_time_out_milli_is(&milli_to1) !=
						FALSE) {
					SII_LOG3A("", NULL, ("\n9612: DS EDID Read1 Timeout"));
					/*Timeout in reading EDID*/
					edid_read_timeout = TRUE;
					break;
				}
			}

			if (edid_read_timeout) {
				break;
			}
		}

		if( sii_sys_time_out_milli_is(&milli_to) != FALSE) {
			SII_LOG3A("", NULL, ("\n9612: DS EDID Read Timeout"));
			/*Timeout in reading EDID*/
			edid_read_timeout = TRUE;
			break;
		}
	}

	/* copy default EDID */
	if (edid_read_timeout) {
		SII_LOG3A("", NULL, ("\n9612: Set Default EDID"));
		memcpy(pEdid, default_edid, SII_EDID_LENGTH);
	}
}

void sTxHandler(void)
{
	SiiDrv9612Event_t txEvent=0;

	if (sTxHpd())
	{
		txEvent |= SII_9612_EVENT_FLAG__HPD_CHANGE;
	}
	
	if (txEvent)
	{
		spObj->pDrvCallBack(txEvent);
	}
}

static bool_t sTxHpd(void)
{
	bool_t hpd=false;

	hpd = (SiiDrvCraRead8(SII_9612_I2C_ADDRESS_TX, 0x203D) & 0x04) ? true : false;
	if (hpd != spObj->pHpd)
	{
		SII_LOG1A("", spObj, ("hpd %s\n", hpd ? "hi" : "lo"));
		spObj->pHpd = hpd;
		if (hpd)
		{
			sii_sys_time_milli_delay(100);
			sGetDownStreamEdid(spObj->edid);
		}
		return true;
	}
	return false;
}

static void sUpdateAudio( void )
{
	if ((spObj->audExt == SII9437_EXTRACTION_MODE__SPDIF2) || (spObj->audExt == SII9437_EXTRACTION_MODE__SPDIF8 || s_is_non_lpcm()))
	{
		SII_LOG1A("", spObj, ("9612 SPDIF\n"));
		SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x2027, 0xC0);
		SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x300C, 0x04);
		SiiDrvCraBitsMod8(SII_9612_I2C_ADDRESS_TX, 0x2026, 0xC0, 0x40);
	}
	else if ((spObj->audExt == SII9437_EXTRACTION_MODE__I2S2) || (spObj->audExt == SII9437_EXTRACTION_MODE__I2S8))
	{
		SII_LOG1A("", spObj, ("9612 I2S\n"));
		SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x300C, 0x02);
		SiiDrvCraBitsMod8(SII_9612_I2C_ADDRESS_TX, 0x2026, 0xC0, 0x80);
	}
}

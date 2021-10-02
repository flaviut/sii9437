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
* sii9612_drv_api.h
*************************************************************************/
#ifndef SII9612_DRV_API_H
#define SII9612_DRV_API_H

#include "sii_datatypes.h"
#include "sii_drv_cra_api.h"
#include "sii9612_drv_tpg_api.h"
#include "si_lib_edid_api.h"
#include "si_drv_aud_api.h"
#include "sii9612_drv_cec_api.h"

#define SII_9612_EVENT_FLAG__CLK_CHANGE		0x000000000000001
#define SII_9612_EVENT_FLAG__HPD_CHANGE		0x000000000000002
#define SII_9612_EVENT_FLAG__EDID_CHANGE	0x000000000000004
#define SII_9612_EVENT_FLAG__AUD_CHANGE		0x000000000000008
#define SII_9612_EVENT_FLAG__AIF_CHANGE		0x000000000000010
#define SII_9612_EVENT_FLAG__ARC_INIT		0x000000000000020
#define SII_9612_EVENT_FLAG__ARC_TERM		0x000000000000040

#define SII_9612_I2C_ADDRESS_RX			0x30
#ifdef SII_BUILD_FPGA
#define SII_9612_I2C_ADDRESS_TX			0x32
#else
#define SII_9612_I2C_ADDRESS_TX			0x30
#endif
#define SII_EDID_LENGTH				256

#define SII_EXTRA_CHSTATUS_LAYOUT			0x80
#define SII_EXTRA_CHSTATUS_NON_LPCM			0x08
#define SII_EXTRA_CHSTATUS_DSD				0x04
#define SII_EXTRA_CHSTATUS_HBRA				0x02
#define SII_EXTRA_CHSTATUS_MUTE				0x01
#define SII_EXTRA_CHSTATUS_LAYOUT_SHIFT		0x04
#define SII_EXTRA_CHSTATUS_LAYOUT_MASK		0x07
#define SII_EXTRA_CHSTATUS_INDEX			0x08

typedef uint32_t  SiiDrv9612Event_t;
typedef void (*SiiDrvCallBack)(SiiDrv9612Event_t event);


enum SiiRxAudioFormat {
	SII_AUDIO_FORMAT__NONE,
	SII_AUDIO_FORMAT__LAYOUT_0,
	SII_AUDIO_FORMAT__LAYOUT_1,
	SII_AUDIO_FORMAT__DSD,
	SII_AUDIO_FORMAT__HBRA
};

enum sii_mclk_mode {
	SII_MCLK_MODE__NONE,
	SII_MCLK_MODE__FS128,
	SII_MCLK_MODE__FS256,
	SII_MCLK_MODE__FS512
};

typedef struct
{
	uint8_t 			ca;
	uint8_t 			lfe;
	/* The values of attenuation associated with the Level Shift Values
	 * in dB. Refer to "Table 32 Audio InfoFrame Data Byte 5, Level Shift
	 * Value" of CEA-861-G standard*/
	uint8_t lsv;
	/* Down Mixed Stereo output is permitted or not
	 * Refer to "Table 33 Audio InfoFrame Data Byte 5, Down-mix Inhibit
	 * Flag" of CEA-861-G standard */
	bool_t dm_inh;
} SiiAifExtract;

/************************************************************************/
/*  Type Definitions                                                    */
/************************************************************************/
void sii9612_rx_create(SiiDrvCallBack pCallBack);
void sii9612_rx_delete( void );
void sii9612_rx_handle( void );
void sii9612_rx_i2s_channel_status_query( SiiChannelStatus_t *pChannelStatus );
void sii9612_rx_edid_audio_set(  SiiEdidAudioData_t *pAudEdid );
void sii9612_rx_spdif_i2s_enable_set( void );
void sii9612_rx_cec_cmd_send( void );
void sii9612_rx_cec_cmd_receive( void );
void sii9612_rx_aif_extraction_status_query(SiiAifExtract *pAifExt);
void sii9612_rx_aif_extraction_query(SiiAifExtract *pAifExt);
void sii9612_rx_i2s_audio_format_query(enum SiiAudioFormat *pAudType);
void sii9612_rx_comp_audio_format_query(bool_t *is_compressed_audio);
void sii9612_tx_create(SiiDrvCallBack pCallBack);
void sii9612_tx_delete( void );
void sii9612_tx_handle( void );
void sii9612_tx_audio_source_set( void );
void sii9612_tx_i2s_audio_format_set( sii9437_extraction_mode audExt );
void sii9612_tx_channel_status_set( SiiChannelStatus_t *pChannelStatus );
void sii9612_tx_hpd_query( void );
void sii9612_tx_edid_audio_query(  SiiEdidAudioData_t *pEdid );
void sii9612_tx_cec_cmd_send( void );
void sii9612_tx_cec_cmd_receive( void );
bool_t sii9612_tx_edid_hpd_query( void );
void sii9612_tx_aif_extraction_status_set(SiiAifExtract *p_aif);
void sii9612_tx_set_hbra ( bool_t hbra);
void sii9612_tx_arc_mode_set( enum sii_arc_mode arc_mode );
void SiiDrvRxPhalanxAudioMclkSet(enum sii_mclk_mode mclk_mode);
uint32_t sii9612_tx_cec_arc_rx_start(void);
uint32_t sii9612_tx_cec_arc_rx_stop(void);
uint32_t sii9612_tx_cec_pa_set(uint16_t cec_pa);
uint32_t sii9612_rx_cec_arc_tx_start(void);
uint32_t sii9612_rx_cec_arc_tx_stop(void);
#if defined(SII9612_INT_METHOD)
void sii9612_rx_cec_handle( void );
#endif
/***************************************************************/
/* Testing Purpose: Remove from prodution */
/***************************************************************/

void SiiDrvRxPhalanxAudioMuteSet( bool_t en);
typedef struct
{
    uint8_t 	fs;
	char 		*fsName;
} FsTableStruc;

extern FsTableStruc fsTable[];
#endif /* SII9612_DRV_API_H*/

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
 * @file si_drv_aud_api.h
 *
 * @brief Time Library
 *
 *****************************************************************************/

#ifndef SI_DRV_AUD_API_H
#define SI_DRV_AUD_API_H

/***** #include statements ***************************************************/

#include "sii_datatypes.h"
#include "sii_drv_cra_api.h"
#include "si_lib_edid_api.h"
#include "sii_api_wrap.h"
/* #include "sii943x_func.h" */

/***** public type definitions ***********************************************/
typedef SiiInst_t SiiDrvTimerInst_t;

/***** public macro definitions **********************************************/

typedef enum {
	SII_AUDIO_FS__22_05KHZ,
	SII_AUDIO_FS__24KHZ,
	SII_AUDIO_FS__32KHZ,
	SII_AUDIO_FS__44_1KHZ,
	SII_AUDIO_FS__48KHZ,
	SII_AUDIO_FS__64KHZ,
	SII_AUDIO_FS__88_2KHZ,
	SII_AUDIO_FS__96KHZ,
	SII_AUDIO_FS__128KHZ,
	SII_AUDIO_FS__176_4KHZ,
	SII_AUDIO_FS__192KHZ,
	SII_AUDIO_FS__256KHZ,
	SII_AUDIO_FS__352_8KHZ,
	SII_AUDIO_FS__384KHZ,
	SII_AUDIO_FS__512KHZ,
	SII_AUDIO_FS__705_6KHZ,
	SII_AUDIO_FS__768KHZ,
	SII_AUDIO_FS__1024KHZ,
	SII_AUDIO_FS__1411_2KHZ,
	SII_AUDIO_FS__1536KHZ,
} SiiAudioFs_t;

typedef enum {
	SII_AUDIO_I2S__16,
	SII_AUDIO_I2S__17,
	SII_AUDIO_I2S__18,
	SII_AUDIO_I2S__19,
	SII_AUDIO_I2S__20,
	SII_AUDIO_I2S__21,
	SII_AUDIO_I2S__22,
	SII_AUDIO_I2S__23,
	SII_AUDIO_I2S__24,
} SiiAudioI2sLength_t;


typedef enum {
	SII_AUDIO_LAYOUT_STEREO,
	SII_AUDIO_LAYOUT_MULTICHANNEL,
} SiiAudioLayout_t;

enum SiiAudioFormat {
	SII_AUDIO_TYPE__NONE,
	SII_AUDIO_TYPE__SPDIF2,
	SII_AUDIO_TYPE__SPDIF8,
	SII_AUDIO_TYPE__I2S2,
	SII_AUDIO_TYPE__I2S8,
	SII_AUDIO_TYPE__DSD2,
	SII_AUDIO_TYPE__DSD6,
	SII_AUDIO_TYPE__HBRA
};

typedef struct {
	uint8_t b[9];
} SiiChannelStatus_t;

typedef enum {
	SII_AUDIO_SRC_I2S,
	SII_AUDIO_SRC_SPDIF,
} SiiAudioSrc_t;

#endif /* SI_DRV_AUD_API_H */

/***** end of file ***********************************************************/

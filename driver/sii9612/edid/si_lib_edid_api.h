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
 * @file si_lib_edid_api.h
 *
 * @brief Time Library
 *
 *****************************************************************************/

#ifndef SI_LIB_EDID_API_H
#define SI_LIB_EDID_API_H

/***** #include statements ***************************************************/

#include "sii_datatypes.h"
#include "sii_drv_cra_api.h"

#define SII_EDID_LENGTH						256
#define SII_EDID_AUD_SIZE					256
#define SII_EDID_BLOCK_SIZE					128

#define EDID_TAG(a)		((a >> 5) & 0x7)
#define EDID_LENGTH(a)		(a & 0x1F)

/***** public type definitions ***********************************************/
/* EDID Data Block Tag Codes */
typedef enum {
	EDB_RESERVED0_TAG = 0,	/* 0 */
	EDB_AUDIO_TAG = 1,	/* 1 */
	EDB_VIDEO_TAG,		/* 2 */
	EDB_VSDB_TAG,		/* 3 */
	EDB_SPEAKER_TAG,	/* 4 */
	EDB_VESA_DTC_TAG,	/* 5 */
	EDB_RESERVED6_TAG,	/* 6 */
	EDB_USE_EXTENDED_TAG,	/* 7 */
	EDB_RESERVED_TAG,	/* 8 */
} SII_EDID_TAG;

/* EDID Data Block Extended Tag Codes */
typedef enum {
	EDB_VIDEO_CAP_ETAG = 0,			/* 0 */
	EDB_AUDIO_VS_VIDEO_ETAG = 1,		/* 1 */
	EDB_VESA_DISP_ETAG,			/* 2 */
	EDB_VESA_VIDEO_TIM_ETAG,		/* 3 */
	EDB_RESERVED_VIDEO_ETAG,		/* 4 */
	EDB_COLOR_ETAG,				/* 5 */
	EDB_HDR_STAT_MD_ETAG,			/* 6 */
	EDB_HDR_DYN_MD_ETAG,			/* 7 */
	EDB_RESERVED8_ETAG,			/* 8 ... 12 */
	EDB_VIDEO_FORMAT_PREF_ETAG = 13,	/* 13 */
	EDB_YCBCR420_VDB_ETAG = 14,		/* 14 */
	EDB_YCBCR420_CAP_MAP_ETAG = 15,		/* 15 */
	EDB_RES_FOR_MISC_AUDIO_ETAG = 16,	/* 16 */
	EDB_VS_AUDIO_ETAG = 17,			/* 17 */
	EDB_RES_FOR_HDMI_AUDIO_ETAG = 18,	/* 18 */
	EDB_ROOM_CONF_ETAG = 19,		/* 19 */
	EDB_SPK_LOCATION_ETAG = 20,		/* 20 */
	EDB_RES_FOR_AUDIO_ETAG = 21,		/* 21 ... 31 */
	EDB_IFDB_ETAG = 32,			/* 32 */
	EDB_RESERVED33_ETAG = 33,		/* 33 ... 255 */
} SII_EDID_EX_TAG;

/* Need to move to right header */
typedef struct {
	uint8_t b[SII_EDID_AUD_SIZE];
} SiiEdidAudioData_t;


uint8_t SiiLibEdidAudioDataGet(uint8_t *pEdid, SiiEdidAudioData_t *pAudEdid);
bool_t SiiLibEdidAudioDataSet(uint8_t *pEdid, SiiEdidAudioData_t *pAudEdid);
extern uint8_t gEdidData[];

#endif /* SI_LIB_EDID_API_H */

/***** end of file ***********************************************************/

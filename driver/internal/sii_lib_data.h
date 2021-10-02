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
* @file sii_lib_data.h
*
* @brief Common data structures
*
*******************************************************************************/

#ifndef SII_LIB_DATA_H
#define SII_LIB_DATA_H

#include "sii_datatypes.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Channel status size in bytes */
#define SII_CHANNEL_STATUS_SIZE	14

/* Offsets for frame rate to sampling frequency conversion -  */
#define	SII_LAYOUT_2CH_SHIFT_OFFSET			0
#define	SII_LAYOUT_8CH_SHIFT_OFFSET			2
#define	SII_LAYOUT_16CH_SHIFT_OFFSET			3
#define	SII_LAYOUT_32CH_SHIFT_OFFSET			4

#define	SII_LAYOUT_A_SHIFT_OFFSET			0
#define	SII_LAYOUT_B_SHIFT_OFFSET			2

/* Channel status masks - Bits 0,1,3,4,5 */
#define	SII_CHANSTATUS_PATTERN__2CH_LPCM		0x00
#define	SII_CHANSTATUS_PATTERN__MULTI_LPCM		0x20
#define	SII_CHANSTATUS_PATTERN__NON_PCM			0x02

#define	SII_CHANSTATUS_PATTERN__MASK			0x3B

/* Channel status layouts - Bits 44, 45, 46, 47: Byte 5 */
#define	SII_CHANSTATUS_LAYOUT__2CH			0x00
#define	SII_CHANSTATUS_LAYOUT__8CH			0x70
#define	SII_CHANSTATUS_LAYOUT__16CH			0xB0
#define	SII_CHANSTATUS_LAYOUT__32CH			0x30

/* Channel status layouts for compressed audio - Bits 44, 45, 46, 47: Byte 5 */
#define	SII_CHANSTATUS_COMPR_LAYOUT__A			0x00
#define	SII_CHANSTATUS_COMPR_LAYOUT__B			0x70

/* Channel layout mask for byte 5 */
#define	SII_CHANSTATUS_LAYOUT__MASK			0xF0

/* eARC Rx Capabilities Data Structure Maximum Length */
#define SII_EARC_CAPS_DS_MAX_LENGTH			256

/* eARC Rx Capability data structure Version */
#define SII_EARC_CAP_DATA_STRUCT_VERSION		0x01

/* eARC Rx Capability data structure End Marker */
#define SII_EARC_CAP_DATA_STRUCT_END_MARKER		0x00

enum sii_arc_mode {
	SII_ARC_MODE__NONE,
	SII_ARC_MODE__ARC,
	SII_ARC_MODE__EARC
};

enum sii_chan_layout {
	SII_CHAN_LAYOUT__A,
	SII_CHAN_LAYOUT__B
};

enum sii_earc_cap_block_id {
	SII_EARC_CAP_BLOCK_ID__0 = 0,
	SII_EARC_CAP_BLOCK_ID__1 = 1,
	SII_EARC_CAP_BLOCK_ID__2 = 2,
	SII_EARC_CAP_BLOCK_ID__3 = 3
};

struct sii_channel_status {
	uint8_t data[SII_CHANNEL_STATUS_SIZE];
};

struct sii_audio_info {
	/* Speaker Allocation.
	 * Refer to "Table Audio InfoFrame Data Byte 4" of CEA-861-G Standard*/
	uint8_t speaker_alloc;
	/* LFE Playback level.
	 * Refer to "Table 34 Audio InfoFrame Data Byte 5, LFE Playback Level
	 * Information" of CEA-861-G Standard*/
	uint8_t lfepbl;
	/* The values of attenuation associated with the Level Shift Values
	 * in dB. Refer to "Table 32 Audio InfoFrame Data Byte 5, Level Shift
	 * Value" of CEA-861-G standard*/
	uint8_t lsv;
	/* Down Mixed Stereo output is permitted or not
	 * Refer to "Table 33 Audio InfoFrame Data Byte 5, Down-mix Inhibit
	 * Flag" of CEA-861-G standard */
	bool_t dm_inh;
};

/* The current audio latency in milliseconds from
 the eARC RX audio input to the speakers*/
struct sii_erx_latency {
	uint8_t data;
};

/* Request to minimize eARC RX Latency in milliseconds
The default value after entereing the [RX eARC] state is 0
0 to 250: The requested audio latency in milliseconds
 from eARC inhput to the speakers
251 to 253: Reserved
254: eARC audio is not synced with video (no synchronisation required)
255: eARC TX does not have latency information (unknown latency) */
struct sii_erx_latency_req {
	uint8_t data;
};

#ifdef __cplusplus
}
#endif
#endif /* SII_LIB_DATA_H */

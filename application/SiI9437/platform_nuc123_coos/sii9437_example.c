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
 * @file sii9437_example.c
 *
 * @brief example application for sii9437 Rx
 *
 *****************************************************************************/
 /* Note: This Macro SII_DEBUG definition should be before the include
  * statements.Do not move down.*/
#define SII_DEBUG	3

/***** #include statements ***************************************************/

#include <stdio.H>

#include "sii943x_user_def.h"
#include "sii_drv_cra_api.h"
#include "sii9437_api.h"
#include "sii9612_drv_api.h"
#include "sii9437_example.h"
#include "sii_platform_api.h"
#include "sii_sys_log_api.h"

#if defined(SII_DEBUG_UART1)
#include <UART.h>
#elif defined(SII_DEBUG_USB_VCOM)
#include <Vcom_Data_Proc.h>
#endif

#define SII_9612_ONLY			0

/***** Register Module name **************************************************/
SII_MODULE_NAME_SET(9437RxApp);

/***** global variables *******************************************************/
#if (!SII_9612_ONLY)
static sii_inst_t		s_sii9437_rx_inst = SII_INST_NULL;
#endif
static uint8_t			s_aud_mode = 0x00;
static enum sii_arc_mode	s_arc_mode = SII_ARC_MODE__EARC;
static bool_t			s_legacy_arc_link_status = FALSE;
static uint16_t			s_dev_pa = 0x1000;

static bool_t is_sii9437_int = FALSE;
static bool_t is_intr_poll_mode = FALSE;
static bool_t s_assert_status = FALSE;

static OS_TCID intr_poll_timer;

#ifndef SII_BUILD_COOS
static bool_t b_coos_started = FALSE;
#endif

/***** local macro definitions ***********************************************/
#define GPIO_CONFIG_MASK	0x2C	/* 00101100 */
					/* 1      - mean output,
					 * 0      - mean input
					 * In	  - GPIO1
					 * Out	  - GPIO2
					 * Reserved in StarterKit -
					 *	GPIO0 - In	- SPDIF IN
					 *	GPIO3 - Out	- SPDIF CLK OUT
					 *	GPIO4 - In	- HPD IN
					 *	GPIO5 - Out	- HPD OUT
					 * */

#define INTR_POLL_INTERVAL	37	/* Interrupt polling interval in ms */

#define RETURN_CODE__SUCCESS	0
#define RETURN_CODE__FAIL	1

#define SII_FREQ_44_1_KHZ		0x00	/* "44.1Khz" */
#define SII_FREQ_48_KHZ			0x02	/* "48Khz" */
#define SII_FREQ_32_KHZ			0x03	/* "32Khz" */
#define SII_FREQ_88_2_KHZ		0x08	/* "88.2Khz" */
#define SII_FREQ_96_KHZ			0x0A	/* "96Khz" */
#define SII_FREQ_176_4_KHZ		0x0C	/* "176.4Khz" */
#define SII_FREQ_192_KHZ		0x0E	/* "192Khz" */
#define SII_FREQ_4x192_KHZ		0x09	/* "4x192Khz" */
#define SII_FREQ_384_KHZ		0x05	/* "384Khz" */
#define SII_FREQ_1536_KHZ		0x45	/* "1536Khz" */
#define SII_FREQ_705_6_KHZ		0x8D	/* "705.6Khz" */
#define SII_FREQ_352_8_KHZ		0x0D	/* "352.8Khz" */
#define SII_FREQ_1411_2_KHZ		0x4D	/* "1411.2Khz" */
#define SII_FREQ_256_KHZ		0x4B	/* "256Khz" */
#define SII_FREQ_512_KHZ		0xCB	/* "512Khz" */
#define SII_FREQ_1024_KHZ		0xC5	/* "1024Khz" */

#define SII_ERX_LATENCY_MAX		100	/* Min value as per spec */
/***** local functions *******************************************************/
static void sii_gpio_config(void);
static enum sii9437_extraction_mode s_aud_mode_get(uint8_t aud_mode);
static void led_aud_mode_set(uint8_t aud_mode);
static void sii_aud_mode_config(uint32_t dev_id);
static void s_audio_mode_set(void);
static void s_audio_mode_update(void);
static enum sii9437_pref_extraction_mode s_pref_aud_mode_get(
		enum sii9437_extraction_mode mode);
static void sii_arc_mode_config(uint32_t dev_id);
static void s_mlk_out_mode_set(struct sii_channel_status channel_status);
static void s_hbra_mode_enable_query(struct sii_channel_status channel_status,
		bool_t *b_hbra);
static uint32_t s_compose_earc_cap_ds(uint8_t *p_raw_edid_in,
		uint8_t *p_earc_caps_ds_out);

/***** local variables *******************************************************/

uint8_t block_2[SII_EARC_CAPS_DS_MAX_LENGTH] = {0};
uint8_t earc_caps_ds[SII_EARC_CAPS_DS_MAX_LENGTH] = {0};
SiiEdidAudioData_t audEdid;

void EINT0IntHandler(void)
{
#ifdef SII_BUILD_COOS
	CoEnterISR();
#endif
	NVIC_DisableIRQ(EINT0_IRQn);
	is_sii9437_int = TRUE;
	/*NVIC_EnableIRQ(EINT0_IRQn);*/
#ifdef SII_BUILD_COOS
	CoExitISR();
#endif
}

void enable_int0(void)
{
	/* Interrupt Configuration */
	GPIO_SET_MODE(PB, 14, GPIO_INPUT);
	GPIO_EINT0();
	GPIO_EnableEINT0(GPIO_INT_LOW);
	/* Set Interrupt Priority */
	NVIC_SetPriority(EINT0_IRQn, 0x01);
	/* Enable Interrupt */
	NVIC_EnableIRQ(EINT0_IRQn);
}

void intr_poll_timer_callback(void *input)
{
	is_sii9437_int = TRUE;
}

#if (!SII_9612_ONLY)
static void s_audio_mode_set(void)
{
	enum sii9437_extraction_mode mode;
	enum sii9437_pref_extraction_mode pref_mode;
	bool_t hbra;
	struct sii_channel_status channel_status;
	mode = s_aud_mode_get(s_aud_mode);
	led_aud_mode_set(s_aud_mode);
	pref_mode = s_pref_aud_mode_get(mode);
#if (!SII_9612_ONLY)
	sii9437_preferred_extraction_mode_set(s_sii9437_rx_inst, &pref_mode);
	sii9437_channel_status_query(s_sii9437_rx_inst,
					&channel_status);
	s_hbra_mode_enable_query(channel_status, &hbra);
	sii9612_tx_set_hbra(hbra);

	/* For HBRA, extraction mode should be I2S8 only */
	if (hbra) {
		SII_LOG1A("", NULL,
			("HBRA - Sii9612 audio format is set to I2S8\n"));
		mode = SII9437_EXTRACTION_MODE__I2S8;
	}
	sii9612_tx_i2s_audio_format_set(mode);
#else
	sii9612_tx_i2s_audio_format_set(mode);
#endif
}

static void s_audio_mode_update(void)
{
	enum sii9437_extraction_mode mode;
	bool_t hbra;
	struct sii_channel_status channel_status;

	mode = s_aud_mode_get(s_aud_mode);
	led_aud_mode_set(s_aud_mode);
#if (!SII_9612_ONLY)
	sii9437_channel_status_query(s_sii9437_rx_inst,
						&channel_status);
	s_hbra_mode_enable_query(channel_status, &hbra);
	sii9612_tx_set_hbra(hbra);

	/* For HBRA, extraction mode should be I2S8 only */
	if (hbra) {
		SII_LOG1A("", NULL,
			("HBRA - Sii9612 audio format is set to I2S8\n"));
		mode = SII9437_EXTRACTION_MODE__I2S8;
	}

	sii9612_tx_i2s_audio_format_set(mode);
#else
	sii9612_tx_i2s_audio_format_set(mode);
#endif
}

static void s_hbra_mode_enable_query(struct sii_channel_status channel_status,
		bool_t *b_hbra)
{
	uint8_t samp_freq  = channel_status.data[3] & 0xCF;

	switch (samp_freq) {
	case SII_FREQ_4x192_KHZ:
	case SII_FREQ_384_KHZ:
	case SII_FREQ_1536_KHZ:
	case SII_FREQ_705_6_KHZ:
	case SII_FREQ_352_8_KHZ:
	case SII_FREQ_1411_2_KHZ:
	case SII_FREQ_256_KHZ:
	case SII_FREQ_512_KHZ:
	case SII_FREQ_1024_KHZ:
		*b_hbra = TRUE;
		break;
	case SII_FREQ_192_KHZ:
	case SII_FREQ_176_4_KHZ:
	case SII_FREQ_96_KHZ:
	case SII_FREQ_88_2_KHZ:
	case SII_FREQ_32_KHZ:
	case SII_FREQ_48_KHZ:
	case SII_FREQ_44_1_KHZ:
		*b_hbra = FALSE;
		break;
	default:
		*b_hbra = FALSE;
		break;
	}
}

static enum sii9437_pref_extraction_mode s_pref_aud_mode_get(
		sii9437_extraction_mode mode)
{
	switch (mode) {
	case SII9437_EXTRACTION_MODE__I2S2:
	case SII9437_EXTRACTION_MODE__I2S8:
		return SII9437_PREF_EXTRACTION_MODE__I2S;
	case SII9437_EXTRACTION_MODE__SPDIF2:
	case SII9437_EXTRACTION_MODE__SPDIF8:
		return SII9437_PREF_EXTRACTION_MODE__SPDIF;
	default:
		return SII9437_PREF_EXTRACTION_MODE__I2S;
	}
}

/* Sii9437 Rx notification callback routine */
static void sii9437_rx_app_notification(uint32_t *p_event_flags)
{
	struct sii_channel_status channel_status;
	struct sii_erx_latency_req erx_latency_req;
	bool_t earc_status;

	if (*p_event_flags & SII9437_EVENT_FLAGS__LOG_FIFO_CHNG) {
		uint32_t size = 0;
		uint32_t i = 0;
		char log_str[50+1] = {0};
		char s[2] = {0};
		sii9437_log_queue_size(s_sii9437_rx_inst, &size);
		printf("\nLog queue size - %lu\n", size);
		while (size) {
			size = size < 50 ? size : 50;
			sii9437_log_queue_query(s_sii9437_rx_inst,
					log_str, size);

			for (i = 0; i < size; i++) {
				if (log_str[i] != '\0') {
					s[0] = log_str[i];
					printf("%s", s);
				}
			}

			sii9437_log_queue_size(s_sii9437_rx_inst, &size);
		}
		printf("\n");
	}

	if (*p_event_flags & SII9437_EVENT_FLAGS__CHAN_STAT_CHNG) {

		struct sii_audio_info aifExt;
		sii9437_audio_info_query(s_sii9437_rx_inst, &aifExt);
		sii9437_channel_status_query(s_sii9437_rx_inst,
				&channel_status);
		s_mlk_out_mode_set(channel_status);
		sii9612_tx_aif_extraction_status_set((SiiAifExtract *)&aifExt);
		sii9612_tx_channel_status_set(
				(SiiChannelStatus_t *) &channel_status);
		s_audio_mode_update();
	}

	if (*p_event_flags & SII9437_EVENT_FLAGS__EXTRACT_MODE_CHNG)
		;

	if (*p_event_flags & SII9437_EVENT_FLAGS__HPD_CHNG) {
		bool_t hpd_status;
		sii9437_hpd_query(s_sii9437_rx_inst, &hpd_status);
		SII_LOG1A("", NULL, ("HPD changed to %s\n",
				hpd_status ? "hi" : "lo"));

		if (s_arc_mode == SII_ARC_MODE__ARC) {
			/* if already ARC link is established,
			 *  Don't do anything.*/
			if (!s_legacy_arc_link_status) {
				enum sii_arc_mode arc_mode;
				arc_mode = hpd_status ? SII_ARC_MODE__ARC :
						SII_ARC_MODE__NONE;
				sii9437_arc_mode_set(s_sii9437_rx_inst,
						&arc_mode);

				/* On HPD high, Start Legacy ARC negotiation
				 *  through CEC commands*/
				if (hpd_status) {
					s_audio_mode_set();
					sii9612_tx_cec_arc_rx_start();
				} else
					sii9612_tx_cec_arc_rx_stop();
			}
		} else if (s_arc_mode == SII_ARC_MODE__EARC) {
			enum sii_arc_mode arc_mode;

			/* if already ARC link is established, Stop ARC.*/
			if (s_legacy_arc_link_status) {
				sii9612_tx_cec_arc_rx_stop();
				s_legacy_arc_link_status = FALSE;
			}
			/* if already eARC link is established,
			 *  Don't do anything.
			 * if eARC link is not established,
			 *  Then set eARC mode (try with) if HPD is high*/
			sii9437_earc_link_query(s_sii9437_rx_inst,
					&earc_status);
			if (!earc_status) {
				s_legacy_arc_link_status = FALSE;
				arc_mode = SII_ARC_MODE__EARC;
				sii9437_arc_mode_set(s_sii9437_rx_inst,
						&arc_mode);
				if (hpd_status)
					s_audio_mode_set();
			}
		}
	}

	if (*p_event_flags & SII9437_EVENT_FLAGS__GPIO_CHNG) {
		uint16_t gpio_status;
		sii9437_gpio_query(s_sii9437_rx_inst, &gpio_status);
		SII_LOG1A("", NULL, ("GPIO input status changed - 0x%02x\n",
				gpio_status));
	}

	if (*p_event_flags & SII9437_EVENT_FLAGS__ERX_LATENCY_REQ_CHNG) {

		struct sii_erx_latency erx_latency;
		sii9437_erx_latency_req_query(s_sii9437_rx_inst,
				&erx_latency_req);
		SII_LOG1A("", NULL, ("Audio Latency Req from Tx: %d msec\n",
						erx_latency_req.data));
		if (erx_latency_req.data > SII_ERX_LATENCY_MAX) {
			erx_latency.data = SII_ERX_LATENCY_MAX;
			sii9437_erx_latency_set(s_sii9437_rx_inst,
					&erx_latency);
			SII_LOG1A("", NULL,
				("Audio1 Latency Set By Rx: %d msec\n",
							erx_latency.data));

		} else {
			erx_latency.data = erx_latency_req.data;
			sii9437_erx_latency_set(s_sii9437_rx_inst,
					&erx_latency);
			SII_LOG1A("", NULL,
				("Audio Latency Set By Rx: %d msec\n",
							erx_latency_req.data));

		}
	}

	if (*p_event_flags & SII9437_EVENT_FLAGS__EARC_LINK_CHNG) {

		sii9437_earc_link_query(s_sii9437_rx_inst, &earc_status);
		SII_LOG1A("", NULL, ("eARC Link status changed to: %s\n",
				earc_status ? "Connected" : "Disconnected"));

		/* LED2 to indicate eARC status */
		sii_platform_gpio_set(SII9437_INST_1, SII_NU_LED2, earc_status);

		/* Turn OFF LED4 (Discovery timeout) when eARC is established
		 * OR when legacy ARC mode is selected on board */
		if (earc_status || (s_arc_mode == SII_ARC_MODE__ARC))
			sii_platform_gpio_set(SII9437_INST_1,
				SII_NU_LED4,
				FALSE);
	}

	if (*p_event_flags & SII9437_EVENT_FLAGS__EARC_DISC_TIMEOUT) {
		SII_LOG1A("", NULL, ("eARC Discovery Failed: TIMEOUT\n"));

		/* LED4 ON to indicate discovery timeout in eARC mode */
		if (s_arc_mode == SII_ARC_MODE__EARC)
			sii_platform_gpio_set(SII9437_INST_1,
					SII_NU_LED4, TRUE);

		/* Start Legacy ARC negotiation through CEC commands*/
		sii9612_tx_cec_arc_rx_start();
	}

	if (*p_event_flags & SII9437_EVENT_FLAGS__ASSERT) {
		uint32_t assert_val = 0;
		sii9437_assert_query(s_sii9437_rx_inst, &assert_val);
		SII_LOG1A("", NULL, ("Assertion failure: %d\n",
				assert_val));
		s_assert_status = TRUE;
	}
}

#endif

static void s_mlk_out_mode_set(struct sii_channel_status channel_status)
{
	uint8_t samp_freq = channel_status.data[3] & 0xCF;
	enum sii9437_mclk_mode mclk_mode =  SII9437_MCLK_MODE__FS128;

	switch (samp_freq) {
	case SII_FREQ_4x192_KHZ:
	case SII_FREQ_384_KHZ:
	case SII_FREQ_1536_KHZ:
	case SII_FREQ_705_6_KHZ:
	case SII_FREQ_352_8_KHZ:
	case SII_FREQ_1411_2_KHZ:
	case SII_FREQ_256_KHZ:
	case SII_FREQ_512_KHZ:
	case SII_FREQ_1024_KHZ:
	case SII_FREQ_192_KHZ:
	case SII_FREQ_176_4_KHZ:
		mclk_mode = SII9437_MCLK_MODE__FS128;
		break;
	case SII_FREQ_96_KHZ:
	case SII_FREQ_88_2_KHZ:
		mclk_mode = SII9437_MCLK_MODE__FS256;
		break;
	case SII_FREQ_32_KHZ:
	case SII_FREQ_48_KHZ:
	case SII_FREQ_44_1_KHZ:
		mclk_mode = SII9437_MCLK_MODE__FS512;
		break;
	default:
		break;
	}

	SII_LOG1A("", NULL, ("samp freq: 0x%x mclk mode set: 0x%x\n",
			samp_freq, mclk_mode));

	sii9437_i2s_mclk_mode_set(s_sii9437_rx_inst, &mclk_mode);
}

static uint32_t s_compose_earc_cap_ds(uint8_t *p_raw_edid_in,
		uint8_t *p_earc_caps_ds_out)
{
	uint8_t edid_byte;
	uint8_t block2_length = 0;
	uint8_t index = 0, raw_block_length, caps_ds_index = 0;
	uint8_t tag, subtag;
	uint32_t status = SII_RETURN_VALUE__SUCCESS;
	uint16_t i = 0;

	if ((!p_earc_caps_ds_out) || (!p_raw_edid_in))
		return SII_RETURN_VALUE_ASSERTION_ERR;

	p_earc_caps_ds_out[caps_ds_index++] = SII_EARC_CAP_DATA_STRUCT_VERSION;
	/* Block ID = 1*/
	p_earc_caps_ds_out[caps_ds_index++] = SII_EARC_CAP_BLOCK_ID__1;
	/* Block Length = 0*/
	p_earc_caps_ds_out[caps_ds_index++] = 0;

	edid_byte = p_raw_edid_in[index];
	while (edid_byte &&
		(caps_ds_index < (SII_EARC_CAPS_DS_MAX_LENGTH - 1)) &&
		(index < (SII_EARC_CAPS_DS_MAX_LENGTH - 1))) {
		tag = EDID_TAG(edid_byte);
		raw_block_length = EDID_LENGTH(edid_byte) + 1;
		switch (tag) {
		case EDB_AUDIO_TAG:
		case EDB_SPEAKER_TAG:
			memcpy(&p_earc_caps_ds_out[caps_ds_index],
					&p_raw_edid_in[index],
					raw_block_length);
			/*Increment Block length*/
			p_earc_caps_ds_out[2] += raw_block_length;
			caps_ds_index += raw_block_length;
			index += raw_block_length;
			edid_byte = p_raw_edid_in[index];
			break;

		case EDB_USE_EXTENDED_TAG:
			subtag = p_raw_edid_in[index + 1];
			if (subtag == EDB_VS_AUDIO_ETAG ||
					subtag == EDB_ROOM_CONF_ETAG) {
				/* Copy to block ID 1*/
				memcpy(&p_earc_caps_ds_out[caps_ds_index],
						&p_raw_edid_in[index],
						raw_block_length);
				/*Increment Block length*/
				p_earc_caps_ds_out[2] += raw_block_length;
				caps_ds_index += raw_block_length;
				index += raw_block_length;
			} else if (subtag == EDB_SPK_LOCATION_ETAG) {
				/* Copy to block ID 2*/
				memcpy(&block_2[block2_length],
						&p_raw_edid_in[index],
						raw_block_length);
				block2_length += raw_block_length;
				index += raw_block_length;
			} else if ((subtag >= 8 && subtag <= 12) ||
					(subtag >= 21 && subtag <= 31) ||
					(subtag >= 33)) {
				/* Error: Reserved sub-tag */
				edid_byte = 0;
				break;
			} else {
				/* Invalid sub-tags. Ignore*/
				index += raw_block_length;
			}
			edid_byte = p_raw_edid_in[index];
			break;
		case EDB_RESERVED0_TAG:
			/* Error: Reserved Tag. Stop Parsing */
			edid_byte = 0;
			status = SII_RETURN_VALUE__INVALID_ARG_ERR;
			break;
		default:
			/* Invalid Tags. Ignore*/
			index += raw_block_length;
			break;
		}
	}

	/* Check if block ID 1 length is 0 and remove Block ID 1 from DS */
	if (caps_ds_index == 3)
		caps_ds_index -= 2;

	/* Copy Block ID 2 */
	if (block2_length) {
		p_earc_caps_ds_out[caps_ds_index++] = SII_EARC_CAP_BLOCK_ID__2;
		p_earc_caps_ds_out[caps_ds_index++] = block2_length;
		memcpy(&p_earc_caps_ds_out[caps_ds_index],
				block_2,
				block2_length);
		caps_ds_index += block2_length;
	}

	/* Write END Marker if DS size is less than 256 */
	if (caps_ds_index < 256)
		p_earc_caps_ds_out[caps_ds_index++] =
				SII_EARC_CAP_DATA_STRUCT_END_MARKER;

	SII_LOG1A("", NULL, ("CAPS DATA Stucture Size: %d\n",
			caps_ds_index));

	for (i = 0; i < SII_EDID_LENGTH; i += 8) {
		if (i >= caps_ds_index)
			break;
		SII_LOG1A("", NULL,
			("%02X.%02X.%02X.%02X.%02X.%02X.%02X.%02X\n",
			p_earc_caps_ds_out[i], p_earc_caps_ds_out[i+1],
			p_earc_caps_ds_out[i+2], p_earc_caps_ds_out[i+3],
			p_earc_caps_ds_out[i+4], p_earc_caps_ds_out[i+5],
			p_earc_caps_ds_out[i+6], p_earc_caps_ds_out[i+7]));
	}

	return status;
}

static void sii9612_app_tx_notification(uint32_t evt_flgs_notify)
{
	if (evt_flgs_notify & SII_9612_EVENT_FLAG__HPD_CHANGE) {
		if (sii9612_tx_edid_hpd_query()) {
			memset(audEdid.b, 0x00, SII_EARC_CAPS_DS_MAX_LENGTH);
			sii9612_tx_edid_audio_query(&audEdid);
#if (!SII_9612_ONLY)
			if (!s_compose_earc_cap_ds(audEdid.b, earc_caps_ds)) {

				sii9437_earc_caps_ds_set(s_sii9437_rx_inst, 0,
					earc_caps_ds,
					SII_EDID_BLOCK_SIZE);
			} else
				SII_LOG1A("", NULL,
						("Error: EDID Compose\n"));
#endif
		} else {
			bool_t en = FALSE;
			sii9437_earc_hpd_set(s_sii9437_rx_inst, &en);
		}
	}

	if (evt_flgs_notify & SII_9612_EVENT_FLAG__ARC_INIT) {
		bool_t hpd_status;
		enum sii_arc_mode arc_mode;
		enum sii9437_extraction_mode mode;
		enum sii9437_pref_extraction_mode pref_mode =
				SII9437_PREF_EXTRACTION_MODE__SPDIF;

		sii9437_hpd_query(s_sii9437_rx_inst, &hpd_status);

		/* In legacy ARC mode, if HPD becomes low set ARC_NONE mode
		 * and if HPD becomes high, set ARC mode */
		arc_mode = hpd_status ? SII_ARC_MODE__ARC :
				SII_ARC_MODE__NONE;
		sii9437_arc_mode_set(s_sii9437_rx_inst,
				&arc_mode);
		sii9612_tx_arc_mode_set(arc_mode);
		sii9437_preferred_extraction_mode_set(s_sii9437_rx_inst,
				&pref_mode);

		s_legacy_arc_link_status = TRUE;

		SII_LOG1A("", NULL,
			("CEC ARC Init Event: Arc Mode set: %d\n", arc_mode));

		if (arc_mode == SII_ARC_MODE__ARC) {
			sii9612_tx_set_hbra(FALSE);
			mode = SII9437_EXTRACTION_MODE__SPDIF2;
			sii9612_tx_i2s_audio_format_set(mode);
		}
	}

	if (evt_flgs_notify & SII_9612_EVENT_FLAG__ARC_TERM) {
		bool_t hpd_status;
		enum sii_arc_mode arc_mode;

		sii9437_hpd_query(s_sii9437_rx_inst, &hpd_status);

		/* In legacy ARC mode, if HPD becomes low set ARC_NONE mode
		 * and if HPD becomes high, set ARC mode */
		arc_mode = hpd_status ? s_arc_mode :
				(s_arc_mode ==  SII_ARC_MODE__EARC) ?
						SII_ARC_MODE__EARC :
						SII_ARC_MODE__NONE;

		SII_LOG1A("", NULL, ("CEC ARC Term: Arc Mode: %d\n", arc_mode));

		sii9437_arc_mode_set(s_sii9437_rx_inst,
				&arc_mode);
		sii9612_tx_arc_mode_set(arc_mode);
		s_legacy_arc_link_status = FALSE;
	}
}

static enum sii9437_extraction_mode s_aud_mode_get(uint8_t aud_mode)
{
	enum sii9437_extraction_mode mode = SII9437_EXTRACTION_MODE__I2S8;
	if (aud_mode) {
		mode = SII9437_EXTRACTION_MODE__SPDIF8;
		SII_LOG1A("", NULL, ("Extraction mode selected: SPDIF\n"));
	} else {
		mode = SII9437_EXTRACTION_MODE__I2S8;
		SII_LOG1A("", NULL, ("Extraction mode selected: I2S\n"));
	}
	return mode;
}

static void led_aud_mode_set(uint8_t aud_mode)
{
	bool_t led3 = aud_mode & SII_AUD_MODE_VAL0 ? TRUE : FALSE;

	/* LED3 ON for SPDIF audio mode */
	sii_platform_gpio_set(SII9437_INST_1,
			SII_NU_LED3,
			led3);
}

static void sii_aud_mode_config(uint32_t dev_id)
{
	uint8_t aud_mode = 0;

	aud_mode |= sii_platform_gpio_status_get(dev_id,
				SII_AUD_MODE_0) ? SII_AUD_MODE_VAL0 : 0x00;

	if (aud_mode != s_aud_mode) {
		s_aud_mode = aud_mode;
		s_audio_mode_set();
	}
}

static void sii_dev_pa_set(uint32_t dev_id)
{
	uint16_t dev_pa = 0;

	dev_pa = sii_platform_gpio_status_get(dev_id,
			SII_DEV_PA) ? 0x2000 : 0x1000;

	if (dev_pa != s_dev_pa) {
		s_dev_pa = dev_pa;
		SII_LOG1A("", NULL, ("dip4: 0x%x\n", dev_pa));
		sii9612_tx_cec_pa_set(dev_pa);
	}
}

static void sii_arc_mode_config(uint32_t dev_id)
{
	enum sii_arc_mode arc_mode = 0;

	arc_mode |= sii_platform_gpio_status_get(dev_id,
				SII_ARC_MODE) ?
				SII_ARC_MODE__ARC : SII_ARC_MODE__EARC;

	arc_mode = sii_platform_gpio_status_get(dev_id,
				SII_ARC_MODE_NONE) ?
				SII_ARC_MODE__NONE : arc_mode;

	if (arc_mode != s_arc_mode) {
		enum sii9437_extraction_mode mode;
		s_arc_mode = arc_mode;

		if (arc_mode == SII_ARC_MODE__ARC) {
			if (s_legacy_arc_link_status)
				return;
			sii9612_tx_cec_arc_rx_start();
			sii9612_tx_set_hbra(FALSE);
			mode = SII9437_EXTRACTION_MODE__SPDIF2;
			sii9612_tx_i2s_audio_format_set(mode);
		} else if (arc_mode == SII_ARC_MODE__EARC) {
			if (s_legacy_arc_link_status) {
				sii9612_tx_cec_arc_rx_stop();
				s_legacy_arc_link_status = FALSE;
			}
			sii9437_arc_mode_set(s_sii9437_rx_inst, &arc_mode);
			sii9612_tx_arc_mode_set(arc_mode);
			SII_LOG3A("", NULL,
				("TOGGLE HPD to trigger eARC Discovery\n"));
		} else {
			if (s_legacy_arc_link_status) {
				sii9612_tx_cec_arc_rx_stop();
				s_legacy_arc_link_status = FALSE;
			}
			sii9437_arc_mode_set(s_sii9437_rx_inst, &arc_mode);
		}

		/* LED4 ON to indicate discovery timeout in eARC mode */
		sii_platform_gpio_set(SII9437_INST_1, SII_NU_LED4,
			(arc_mode == SII_ARC_MODE__ARC) ? FALSE : TRUE);
	}
}

static void sii_intr_mode_config(uint32_t dev_id)
{
	bool_t intr_mode = sii_platform_gpio_status_get(dev_id,
			SII_INTR_POLL_MODE);

	/* No change in the intr mode */
	if (intr_mode == is_intr_poll_mode)
		return;

	SII_LOG1A("", NULL, ("Interrupt mode changed\n"));
	is_intr_poll_mode = intr_mode;

	if (is_intr_poll_mode) {
		/* Disable Nuvoton interrupt */
		NVIC_DisableIRQ(EINT0_IRQn);

		SII_LOG1A("", NULL,
			("Interrupt Handling Mode: Polling every %d ms\n",
			INTR_POLL_INTERVAL));

		/* Initialise CoOS if it is not done yet*/
		#ifndef SII_BUILD_COOS
		if (!b_coos_started) {
			/* Initialise CoOS */
			CoInitOS();
			CoStartOS();
			b_coos_started = TRUE;
		}
		#endif

		if (!intr_poll_timer) {
			/* Create interrupt poll timer for CoOS*/
			intr_poll_timer = CoCreateTmr(TMR_TYPE_PERIODIC,
					INTR_POLL_INTERVAL, INTR_POLL_INTERVAL,
					intr_poll_timer_callback, NULL);
		}

		/* Start periodic interrupt timer */
		CoStartTmr(intr_poll_timer);
	} else {
		SII_LOG1A("", NULL, ("Interrupt Handling Mode: Nuvoton ISR\n"));
		if (intr_poll_timer) {
			CoSetTmrCnt(intr_poll_timer, 0, 0);
			CoDelTmr(intr_poll_timer);
			intr_poll_timer = 0;
		}
		enable_int0();
	}
}

static bool_t sii_is_com_mode(uint32_t dev_id)
{
	bool_t is_fw_comm_mode;
	bool_t is_dip_comm_mode;

	Vcom_Firmware_comm_mode_query(&is_fw_comm_mode);

#ifndef SII_BUILD_FPGA
	is_dip_comm_mode = sii_platform_gpio_status_get(dev_id,
				SII_COMM_MODE);
#else
	dev_id = dev_id;
	return FALSE;
#endif

	return (is_dip_comm_mode | is_fw_comm_mode) ? TRUE : FALSE;
}

static void sii_gpio_config(void)
{

	/* Config GPIO for Hard Reset */
	sii_platform_gpio_config(SII9437_INST_1,
			SII_UC_RST,
			SII_PLATFORM_GPIO_DIRECTION__OUTPUT);
	/* Config GPIO for SII9437 Interrupt */
	sii_platform_gpio_config(SII9437_INST_1,
			SII9437_INT,
			SII_PLATFORM_GPIO_DIRECTION__INPUT);
	/* Config GPIO for SiI9612 Interrupt */
	sii_platform_gpio_config(SII9437_INST_1,
			SII9612_INT,
			SII_PLATFORM_GPIO_DIRECTION__INPUT);

	/* Config GPIO for LEDs */
	sii_platform_gpio_config(SII9437_INST_1,
			SII_NU_LED1,
			SII_PLATFORM_GPIO_DIRECTION__OUTPUT);
	sii_platform_gpio_config(SII9437_INST_1,
			SII_NU_LED2,
			SII_PLATFORM_GPIO_DIRECTION__OUTPUT);
	sii_platform_gpio_config(SII9437_INST_1,
			SII_NU_LED3,
			SII_PLATFORM_GPIO_DIRECTION__OUTPUT);
	sii_platform_gpio_config(SII9437_INST_1,
			SII_NU_LED4,
			SII_PLATFORM_GPIO_DIRECTION__OUTPUT);

	/* Turn off Leds */
	sii_platform_gpio_set(SII9437_INST_1,
			SII_NU_LED2,
			FALSE);
	sii_platform_gpio_set(SII9437_INST_1,
			SII_NU_LED3,
			FALSE);
	sii_platform_gpio_set(SII9437_INST_1,
			SII_NU_LED4,
			FALSE);
}
/**
 * @brief       System initialisation routine
 *
 * @param       None
 *
 * @return      None
 */
static void system_init(void)
{
	/* Unlock Protected Registers */
	SYS_UnlockReg();

	/* Enable XTAL PINs */
	GPIO_XT1();
	/* Enable External XTAL (4~24 MHz) */
	CLK_HXT_ENABLE();
	/* Waiting for 12MHz clock ready */
	while (!CLK_IS_STABLE(CLK_HXT_STB))
		;

	/* Switch HCLK clock source to XTAL */
	SET_HCLK_CLKSRC(CLK_HCLK_HXT);

	/* PLL = FIN(12MHz) * NF / NR / NO. */
	/* Settings for the following config:
		Input External Xtal: 12MHz
		PLL Output Frequency: 72MHz */
	/* Set PLL Output Divider Number */
	CLK_SET_PLL_NO(CLK_PLLCON_NO_4);

#if defined(SII_DEBUG_UART1)
	/* Set PLL Input Divider Number */
	CLK_SET_PLL_NR(5);
	/* Set PLL feedback divider number */
	CLK_SET_PLL_NF(120);
#elif defined(SII_DEBUG_USB_VCOM)
	/* Set PLL Input Divider Number */
	CLK_SET_PLL_NR(7);
	/* Set PLL feedback divider number */
	CLK_SET_PLL_NF(112);
#endif
	/* Disable PLL power down mode */
	CLK_PLL_PD_DISABLE();
	/* Enable PLL output */
	CLK_PLL_OUTP_EN();
	/* Wait PLL Stable */
	while (!CLK_IS_STABLE(CLK_PLL_STB))
		;
	/* Configure HCLK to be PLL output */
	SET_HCLK_CLKSRC(CLK_HCLK_PLL);

	/* Update system core clock */
	SystemCoreClockUpdate();

	/* Enable FMC ISP */
	FMC_ENABLE_ISP();

	return;
}

/**
 * @brief       Platform initialisation routine
 *
 * @param       None
 *
 * @return      None
 */
static void platform_init(void)
{
	struct i2c_adapter adapter;

#if defined(SII_WORKAROUND_I2C)
	/* Please Do Not try to Modify this
	 * This is required for Nuvoton to load from Flash
	 */
	unsigned long i = 7000000;
	while (i--)
		;
#endif

#if defined(SII_DEBUG_UART1)
	/* Initialise UART*/
	UART1_Init();
#elif defined(SII_DEBUG_USB_VCOM)
	/* Initialise Virtual Port Function */
	Vcom_Config_Proc();
	/* Execute M3RT Host Core Process */
	Host_Core_Proc();
#endif

	/* Initialise Timer */
	LibTimerInit();

	/* Initialise I2C Port 0 - 400K */
	adapter.port = I2C0;
	adapter.clock = 400000;
	sii_platform_i2c_init(&adapter);

	sii_gpio_config();
}


static void sii_led_firmware_blinking_callback(void *arg)
{
	static bool_t toggle = TRUE;

	if (FALSE == sii_is_com_mode(SII9437_INST_1)) {
		sii_platform_gpio_set(SII9437_INST_1, SII_NU_LED1, toggle);
		toggle = !toggle;
		/* SII_LOG1A("", NULL, (" LED %s\n",
		 * (toggle ? "ON" : "OFF"))); */
	}
}

#ifdef SII_BUILD_COOS
	void *s_blinking_timer_handle;
#endif
static void sii_led_firmware_blinking(void)
{
#ifdef SII_BUILD_COOS
	sii_timer_create(sii_led_firmware_blinking_callback, NULL,
		&s_blinking_timer_handle, 500, true);
	sii_timer_start(&s_blinking_timer_handle);
#else
	LibTimerSetTimer(eTIMER0_COUNT_USE_FOR_LED, 500);
#endif
}

int main(int argc, char *argv[])
{
#if (!SII_9612_ONLY)
	uint32_t chip_id;
	uint8_t chip_version;
	char sw_version[30];
	uint32_t return_code = SII_RETURN_VALUE__SUCCESS;
	uint16_t gpio_config_mask = GPIO_CONFIG_MASK;
	struct sii_erx_latency erx_latency = {SII_ERX_LATENCY_MAX};
	bool_t b_com_mode_led_op_is = FALSE;
	uint32_t i;

	struct sii9437_config rx_config = {SII9437_INST_1,
				sii9437_rx_app_notification,
				2000,
				SII9437_INST_1_I2C_ADDR_PAGE1,
				SII9437_INST_1_I2C_ADDR_PAGE2,
				erx_latency};
#endif

	/* Initialise the system */
	system_init();

	/* Configure the platform with required settings */
	platform_init();

#ifdef SII_BUILD_COOS
	 /* Initialise CoOS */
	CoInitOS();
#endif

	/* Initialise debug log*/
	/*dbglog_init();*/

	/* Enable all levels for module DBG_MOD_NONE*/
	/*dbglog_mask_set(DBG_MOD_NONE, DBG_ALL);*/

	SII_LOG1A("", NULL, (" SII9437 Rx NUC123/CoOS Application\n"));
	SII_LOG1A("", NULL, (" Copyright Lattice Semiconductor 2017\n"));
	SII_LOG1A("", NULL, (" Build Time: " __DATE__ "-" __TIME__ "\n"));

	/* Apply Hard Reset to SiI9437 and SiI9612 chips */
	sii_platform_device_reset_set(SII9437_INST_1, TRUE);
	/* Soft delay as timer is not consistent
	 * Do not call sii_platform_sleep_msec() function */
	i = 20000; /* 2 msec */
	while (i--)
		;
	/* Release Hard Reset to SiI9437 and SiI9612 chips */
	sii_platform_device_reset_set(SII9437_INST_1, FALSE);
	/* Soft delay as timer is not consistent
	 * Do not call sii_platform_sleep_msec() function */
	i = 100000; /*10 msec*/
	while (i--)
		;

	/* Wait loop for COMM Mode */
	while (sii_is_com_mode(SII9437_INST_1)) {
#if defined(SII_DEBUG_USB_VCOM)
		sii_vcom_command_process();
#endif
	};

	sii9612_tx_create(&sii9612_app_tx_notification);

#if (!SII_9612_ONLY)

	return_code = sii9437_create(&rx_config, &s_sii9437_rx_inst);
	if (return_code != SII_RETURN_VALUE__SUCCESS) {
		SII_LOG1A("", NULL,
		("SII9437 Rx Create failed with error - %d\n", return_code));
		sii9612_tx_delete();
		return RETURN_CODE__FAIL;
	}

	sii9437_chip_id_query(s_sii9437_rx_inst, &chip_id);
	SII_LOG1A("", NULL, ("Chip ID: 0x%x\n", chip_id));

	sii9437_chip_version_query(s_sii9437_rx_inst, &chip_version);
	SII_LOG1A("", NULL, ("Chip Revision: 0x%x\n", chip_version));

	sii9437_software_version_query(sw_version, 30);
	SII_LOG1A("", NULL, ("Software Version: %s\n", sw_version));

	/* Set GPIO configuration */
	sii9437_gpio_config_set(s_sii9437_rx_inst, &gpio_config_mask);
	SII_LOG1A("", NULL, ("GPIO configuration set to - 0x%02x",
			gpio_config_mask));
#endif
#if (!SII_9612_ONLY)
#ifndef SII_BUILD_FPGA
	sii_intr_mode_config(SII9437_INST_1);
	/* Initially, if it is not in the polling mode,
	*Enable Nuvoton interrupt handler */
	if (!is_intr_poll_mode) {
		SII_LOG1A("", NULL, ("Interrupt Handling Mode: Nuvoton ISR\n"));
		enable_int0();
	}
#endif
#endif

#ifdef SII_BUILD_COOS
	CoStartOS();
#endif
	/* System infinite while loop */
	SII_LOG1A("", NULL, ("System is running...\n"));

	sii_led_firmware_blinking();

	do {
		if (s_assert_status) {
			SII_LOG1A("", NULL, ("Assert status - %s\n",
					s_assert_status ? "TRUE" : "FALSE"));
			break;
		}

#if defined(SII_DEBUG_USB_VCOM)
		sii_vcom_command_process();
#endif
		if (FALSE == sii_is_com_mode(SII9437_INST_1)) {
#ifndef SII_BUILD_COOS
			if (LibTimerIfTimerExpired(eTIMER0_COUNT_USE_FOR_LED)) {
				sii_led_firmware_blinking_callback(NULL);
				LibTimerSetTimer(eTIMER0_COUNT_USE_FOR_LED,
						500);
			}
#endif
			sii9612_tx_handle();
#if (!SII_9612_ONLY)
			sii_aud_mode_config(SII9437_INST_1);
			sii_dev_pa_set(SII9437_INST_1);

			if (is_sii9437_int) {
				/*SII_LOG1A("", NULL,
					("Sii9437 Interrupt\n"));*/
				is_sii9437_int = FALSE;
				sii9437_handle(s_sii9437_rx_inst);
				if (!is_intr_poll_mode)
					NVIC_EnableIRQ(EINT0_IRQn);
			}
			sii_arc_mode_config(SII9437_INST_1);
			sii_intr_mode_config(SII9437_INST_1);
#endif
			if (b_com_mode_led_op_is)
				b_com_mode_led_op_is = FALSE;
		} else {
			/* Keep LED1 ON in COM mode of operation */
			if (!b_com_mode_led_op_is) {
				b_com_mode_led_op_is = TRUE;
				sii_platform_gpio_set(SII9437_INST_1,
						SII_NU_LED1, TRUE);
			}
		}
	} while (1);

	sii9437_delete(s_sii9437_rx_inst);
	sii9612_tx_delete();

	SII_LOG1A("", NULL, ("System is stopped\n"));

	return RETURN_CODE__SUCCESS;
}

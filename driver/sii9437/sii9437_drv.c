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
 * @file sii9437_drv.c
 *
 * @brief Driver for SiI9437 (eARC/ARC receiver)
 *
 *****************************************************************************/

/************************* Debug Flags ***************************************/
/* Debug flag should be set before inclusion of logging hdeader */
#define SII_DEBUG	3

/***** #include statements ***************************************************/

#include "sii9437_version.h"
#include "sii9437_api.h"
#include "sii9437_drv.h"
#include "sii_platform_api.h"
#include "sii_system_api.h"
#include "sii_sys_obj_api.h"
#include "sii_sys_log_api.h"
#include "sii9437_regs.h"
#include "sii_sys_malloc_api.h"
#include "sii_sys_seq_api.h"
#include "sii_sys_assert_api.h"
#include "sii_lib_pipe_api.h"
#include "sii_lib_pipe_api.h"

/***** Register Module name **************************************************/
SII_MODULE_NAME_SET(SiI9437);

/***** local macro definitions ***********************************************/
#if (SII_ENV_BUILD_ASSERT)
#define SII_ASSERT_CHECK()	{if (b_asserted)\
					return SII_RETURN_VALUE_ASSERTION_ERR; }
#else
#define SII_ASSERT_CHECK()
#endif


#define SII9437_CHIP_ID		0x9437
#define SII9437_CHIP_REVISION	0x00

/* Maximum length of Software version string */
#define MAX_SW_VERSION_LENGTH	65

#define NUM_BKSV	5

#define SII9437_CHANNEL_STATUS_SIZE		SII_CHANNEL_STATUS_SIZE

#define SII_HEARTBEAT_TIME_MAX			120
#define	SII_HPD_LOW_MIN				100

#define	SII9437_COMMA_INIT_TIMER_VAL		50

#define SII_FREQ_44_1_KHZ		0x00		/* "44.1Khz" */
#define SII_FREQ_48_KHZ			0x02		/* "48Khz" */
#define SII_FREQ_32_KHZ			0x03		/* "32Khz" */
#define SII_FREQ_64_KHZ			0x0B		/* "64Khz" */
#define SII_FREQ_88_2_KHZ		0x08		/* "88.2Khz" */
#define SII_FREQ_96_KHZ			0x0A		/* "96Khz" */
#define SII_FREQ_128_KHZ		0x8B		/* "128 KHz " */
#define SII_FREQ_176_4_KHZ		0x0C		/* "176.4Khz" */
#define SII_FREQ_192_KHZ		0x0E		/* "192Khz" */
#define SII_FREQ_4x192_KHZ		0x09		/* "4x192Khz" */
#define SII_FREQ_384_KHZ		0x05		/* "384Khz" */
#define SII_FREQ_1536_KHZ		0x45		/* "1536Khz" */
#define SII_FREQ_705_6_KHZ		0x8D		/* "705.6Khz" */
#define SII_FREQ_352_8_KHZ		0x0D		/* "352.8Khz" */
#define SII_FREQ_1411_2_KHZ		0x4D		/* "1411.2Khz" */
#define SII_FREQ_256_KHZ		0x4B		/* "256Khz" */
#define SII_FREQ_512_KHZ		0xCB		/* "512Khz" */
#define SII_FREQ_1024_KHZ		0xC5		/* "1024Khz" */

/* Channel count settings in channel status extra byte 1 */
#define SII_CHANNEL_COUNT_2_DOT_0	0x00	/* 2-ch */
#define SII_CHANNEL_COUNT_2_DOT_1	0x20	/* use 6-ch setting */
#define SII_CHANNEL_COUNT_5_DOT_1	0x40	/* 6-ch */
#define SII_CHANNEL_COUNT_6_DOT_1	0x60	/* Use 8-ch setting */
#define SII_CHANNEL_COUNT_7_DOT_1	0x60	/* 8-ch */

/* Wait time to allow IEC audio format code to be received by eARC Rx */
#define	SII_RX_AUDIO_WAIT_TIME			100

/***** local type definitions ************************************************/

struct fs_table_struc {
	uint16_t	fs_val;
	uint8_t		fs;
	char		*fsName;
};

/* Frequencies are stored as fs*10
 * as some of them contain float values */
static struct fs_table_struc fs_table[] = {
	{ 441,		0x00, "44.1 KHz" },
	{ 480,		0x02, "48 KHz" },
	{ 320,		0x03, "32 KHz" },
	{ 3840,		0x05, "384 KHz" },
	{ 882,		0x08, "88.2 KHz" },
	{ 7680,		0x09, "768 KHz" },
	{ 960,		0x0A, "96 KHz" },
	{ 640,		0x0B, "64 KHz" },
	{ 1764,		0x0C, "176.4 KHz" },
	{ 3528,		0x0D, "352.8 KHz" },
	{ 1920,		0x0E, "192 KHz" },
	{ 15360,	0x45, "1536 KHz" },
	{ 2560,		0x4B, "256 KHz" },
	{ 14112,	0x4D, "1411.2 KHz" },
	{ 1280,		0x8B, "128 KHz" },
	{ 7056,		0x8D, "705.6 KHz" },
	{ 10240,	0xC5, "1024 KHz" },
	{ 5120,		0xCB, "512 KHz" },
};

struct sii9437_channel_status {
	uint8_t data[SII9437_CHANNEL_STATUS_SIZE];
};

enum sii9437_sm_state {
	SII9437_SM_STATE__IDLE1,
	SII9437_SM_STATE__DISC1,
	SII9437_SM_STATE__IDLE2,
	SII9437_SM_STATE__DISC2,
	SII9437_SM_STATE__ARC,
	SII9437_SM_STATE__EARC
};

const char *sm_states[] = {"IDLE1", "DISC1", "IDLE2", "DISC2", "ARC", "EARC"};

struct obj_t {

	/* Hardware device instantiation identification number */
	uint32_t dev_id;

	sii9437_event_callback_func event_cb_func;

	uint32_t log_fifo_size;

	uint32_t chip_id;

	uint8_t chip_version;

	/* Controls which events will cause notification handler
	 * to be called */
	uint32_t event_flags_mask;

	/* Event Flags status*/
	uint32_t event_flags_status;
	/* Controls whether SiI9437 is in standby mode or
	 * full operational mode.
	 * In standby mode SiI9437 is configured for low power consumption
	 * and TMDS impedance termination is turned off */

	bool_t b_standby_mode;

	bool_t b_earc_connected_status;

	/* eARC_HPD when in eARC mode Set by API*/
	bool_t b_earc_hpd;

	uint32_t earc_hpd_toggle_time;

	/* Timer sequence instance */
	sii_inst_t seq_inst;

	/* eARC HPD low timer instance needed to maintain eARC HPD low
	 * for earc_hpd_toggle_time amount of time */
	sii_inst_t earc_hpd_timer;

	/* HPD low timer instance needed to maintain HPD low
	 * for 100ms */
	sii_inst_t hpd_timer;

	/* Timer to wait until input channel status is received */
	sii_inst_t audio_wait_timer;

	/* Timer to wait until comma is initiated */
	sii_inst_t comma_init_timer;

	/* eARC_HPD to be updated on timer expiration */
	bool_t b_earc_hpd_pending;

	/* Flag for eARC capability data structure ready status */
	bool_t b_earc_caps_ds_ready;

	/* HPD to be updated on timer expiration */
	bool_t b_hpd_pending;

	bool_t b_hpd_in;

	/* Current HPD level HiGH/LOW*/
	bool_t b_hpd_out;

	/* ARC mode status (NONE, Legacy ARC, or EARC) */
	enum sii_arc_mode arc_mode_status;

	/* Configured ARC mode */
	enum sii_arc_mode arc_mode_configured;

	/* Current state in State machine */
	enum sii9437_sm_state sm_state;

	/* Configured preferred audio extraction mode (I2S, SPDIF) in case
	 * audio is received as PCM or compressed audio formats. Set by API */
	enum sii9437_pref_extraction_mode pref_extraction_mode;

	/* extraction mode status
	 * (NONE, SPDIF2, SPDIF8, I2S2, I2S8, DSD2, DSD6) */
	enum sii9437_extraction_mode extraction_mode_status;

	/* Configured mode of MCLK output signal.
	 * (NONE, FS128, FS256, FS512) Set by API */
	enum sii9437_mclk_mode mclock_mode;

	/* channel status data (only applicable to SPDIF2, SPDIF8, I2S2, I2S8
	 * extraction modes). */
	struct sii9437_channel_status channel_status;

	/* channel status data (only applicable to SPDIF2, SPDIF8, I2S2, I2S8
	 * extraction modes). */
	struct sii_channel_status channel_status_out;

	/* Sample rate of audio data in Hz */
	uint32_t sample_rate;

	/* Mute Status for audio data. Set by API */
	bool_t b_audio_mute_user_set;

	/* GPIO Input/Output configuration */
	/*Each bit represents 1 GPIO pin based on the following assignment:
		Bit 0   : GPIO0
		Bit 1   : GPIO1
		Bit 2   : GPIO2
		Bit 3   : GPIO3
		Bit 4   : GPIO4
		Bit 5   : GPIO5
		Bit 6..7: unused*/
	/*
	 * '0' enables GPIO to be an INPUT,
	 * '1' enables GPIO to be an OUTPUT
	 *  By default all GPIOs are configured as INPUT
	 * */
	uint16_t gpio_pin_io_config;

	/* Mask related to GPIO event control */
	uint16_t gpio_pin_event_mask;

	uint32_t mutex;

	uint8_t caps_ds[SII_EARC_CAPS_DS_MAX_LENGTH];

	struct sii_erx_latency erx_latency;

	struct sii_erx_latency_req erx_latency_req;

	struct sii_audio_info audio_info;

	/* Config params from application */
	bool_t b_caps_ds_stress_test;

	/* Different sources of Mute*/
	uint8_t audio_mute_disable;

	bool_t earc_lock_detected;

	bool_t earc_clock_detected;

	bool_t ch_status_rcvd;

	bool_t is_input_ch_status_valid;

	bool_t mute_state;

	bool_t b_ouput_io_en_state;

	bool_t is_clk_det_intr_mask_enable;

	bool_t is_lock_det_intr_mask_enable;

	bool_t is_fs_chng_intr_mask_enable;

	bool_t is_cs_chng_intr_mask_enable;

	bool_t is_pkt_ecc_err_fix_intr_mask_en;

	bool_t is_pkt_ecc_err_intr_mask_en;

	bool_t is_ecc_err_intr_mask_en;

#ifdef SII_LOG_QUEUE
	/* Logging FIFO queue object */
	sii_inst_t log_fifo;
#endif
};

/***** local data objects ****************************************************/

static sii_inst_t inst_list = SII_INST_NULL;

/***** local prototypes ******************************************************/

static void s_int_handle(struct obj_t *p_obj);

static void s_gpio_config_update(struct obj_t *p_obj);

static void s_gpio_output_update(struct obj_t *p_obj,
		uint16_t *p_gpio_pin_mask, bool_t b_set);

static void s_sii9437_init(struct obj_t *p_obj,
		const struct sii9437_config *p_config);

static void s_sii9437_init_stage_2(struct obj_t *p_obj);

static void s_audio_update(struct obj_t *p_obj);

static void s_standby_set(struct obj_t *p_obj, bool_t *p_on);

static void s_user_notification_handler(struct obj_t *p_obj,
			uint32_t evt_flgs_notify);

static void s_print_channel_status(struct obj_t *p_obj,
			uint8_t *p_data, bool_t b_extra_bytes);

static void s_gpio_event_mask_set(struct obj_t *p_obj);

static void s_audio_mute(struct obj_t *p_obj, bool_t p_on);

static void s_arc_mode_update(struct obj_t *p_obj,
		enum sii_arc_mode arc_mode_status);

static void s_print_mem_usage(struct obj_t *p_obj, const char *log_str);

static void s_hpd_update(struct obj_t *p_obj,
		bool_t b_earc_conn_stat, bool_t b_hpd_in,
		uint32_t *evt_flgs_notify);

static void s_vhpd_read(struct obj_t *p_obj, uint32_t *evt_flgs_notify);

static void s_hpd_init(struct obj_t *p_obj);

static void s_earc_hpd_set(struct obj_t *p_obj, bool_t b_earc_hpd);

static void s_output_io_enable(struct obj_t *p_obj, bool_t b_en);

static void s_earc_hpd_timer_start(struct obj_t *p_obj);

static void s_hpd_set(struct obj_t *p_obj, bool_t b_hpd,
		uint32_t *evt_flgs_notify);

static void s_hpd_timer_start(struct obj_t *p_obj);

static void s_audio_wait_timer_start(struct obj_t *p_obj);

static uint32_t s_aud_mode_set(struct obj_t *p_obj,
			enum sii9437_extraction_mode audio_mode);

static uint32_t s_aud_mode_rev0_set(struct obj_t *p_obj,
		enum sii9437_extraction_mode audio_mode);

static void s_config_update(struct obj_t *p_obj);
static void s_update_channel_status_pattern(struct obj_t *p_obj);
static void s_aif_extraction_get(struct obj_t *p_obj,
		uint8_t *p_ch_stat,
		struct sii_audio_info *p_audio_info);
static bool_t is_multi_channel(struct obj_t *p_obj);
static bool_t s_is_non_lpcm(struct obj_t *p_obj);

static void s_sm_state_update(struct obj_t *p_obj, uint8_t *int_regs);

static void s_arc_change_sm_update(struct obj_t *p_obj);

static void s_state_change_sm_update(struct obj_t *p_obj);
static void s_erx_latency_req(struct obj_t *p_obj,
				uint32_t *evt_flgs_notify);

static void s_rx_earc_connect_config(struct obj_t *p_obj);
static bool_t s_is_hbra(struct obj_t *p_obj);
static uint32_t s_cs_change_check(struct obj_t *p_obj, bool_t *is_aud_update);
static void s_status_reset(struct obj_t *p_obj);
static uint32_t s_clk_lock_mode_set(struct obj_t *p_obj, uint8_t *ch_data);
static uint32_t s_earc_only_intr_mask_en(struct obj_t *p_obj, bool_t en);
static uint32_t s_earc_lock_chng_intr_mask_en(struct obj_t *p_obj, bool_t en);
static uint32_t s_get_layout(struct obj_t *p_obj);
static void s_print_layout(struct obj_t *p_obj);

/*static bool_t s_is_freq(struct obj_t *p_obj, uint8_t freq);*/

/***** call-back functions ***************************************************/

/* Timer value callback routine to be used by seq timers */
void sii_timer_value_callback(sii_inst_t inst,
	sii_sys_seq_time_ms_t *timer_value, bool_t b_set)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
				inst);
	SII_ASSERT(p_obj);
	SII_OSAL_MUTEX_SET(p_obj->mutex);

	if (!timer_value)
		return;

	if (b_set) {
		SII_LOG1A("", NULL,
			(" Timer value setting to - %d\n", *timer_value));
		sii9437_hal_timer_value_set(p_obj->dev_id, *timer_value);
	} else {
		sii9437_hal_timer_value_get(p_obj->dev_id, timer_value);
		SII_LOG1A("", NULL,
			(" Timer value retrieved - %d\n", *timer_value));
	}

	SII_OSAL_MUTEX_CLR(p_obj->mutex);
}

/* Callback routine for eARC HPD low timer */
void earc_hpd_timer_callback(sii_inst_t inst)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);
	SII_ASSERT(p_obj);

	/* Send pending eARC HPD on timer expiration */
	s_earc_hpd_set(p_obj, p_obj->b_earc_hpd_pending);
}

/* Callback routine for HPD low timer */
void hpd_timer_callback(sii_inst_t inst)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);
	uint32_t evt_flgs_notify = 0;

	SII_ASSERT(p_obj);

	/* Send pending HPD on timer expiration */
	s_hpd_set(p_obj, p_obj->b_hpd_pending, &evt_flgs_notify);

	if (evt_flgs_notify)
		s_user_notification_handler(p_obj, evt_flgs_notify);
}

/* Callback routine for audio wait timer */
void audio_wait_timer_callback(sii_inst_t inst)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);
	SII_ASSERT(p_obj);

	if (p_obj->audio_mute_disable) {
		sii9437_hal_output_channel_status_set(p_obj->dev_id,
				p_obj->channel_status_out.data,
				&p_obj->channel_status.data[5]);

		/* Enable output */
		s_output_io_enable(p_obj, TRUE);

		/* Unmute output audio */
		s_audio_mute(p_obj, FALSE);

		/* Enable lock detection change interrupt */
		s_earc_lock_chng_intr_mask_en(p_obj, TRUE);

		s_user_notification_handler(p_obj,
			SII9437_EVENT_FLAGS__EXTRACT_MODE_CHNG |
			SII9437_EVENT_FLAGS__CHAN_STAT_CHNG);
	}
}

/* Callback routine for comma initialisation wait timer */
void comma_init_timer_callback(sii_inst_t inst)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
				inst);
	SII_ASSERT(p_obj);

	s_sii9437_init_stage_2(p_obj);
}

/***** public functions ******************************************************/
uint32_t sii9437_create(const struct sii9437_config *p_config,
			sii_inst_t *p_inst)
{
	struct obj_t *p_obj = NULL;
	sii_inst_t parent_inst = SII_INST_NULL;
	uint16_t i;

	SII_ASSERT_CHECK();
	SII_ASSERT(p_config);
	SII_ASSERT(p_inst);
	SII_ASSERT_CHECK();

	/* Chip reset */
	sii9437_hal_chip_reset(p_config->dev_id);

	/* Disable Comma first */
	SII_LOG1A("", NULL, ("Disabling COMMA...\n"));
	sii9437_hal_rx_comma_enable(p_config->dev_id, FALSE);

	s_print_mem_usage(p_obj, "Before Sii9437 Create");

	/* Create instance list if not exist */
	if (!inst_list) {
		inst_list = sii_sys_obj_list_create(SII_MODULE_NAME_GET(),
				sizeof(struct obj_t));
		SII_ASSERT(inst_list);
	} else {
		/* Check if there is any instance existing with the same
		 * device ID */
		struct obj_t *p_obj_iter = sii_sys_obj_first_get(inst_list);

		while (p_obj_iter) {
			if (p_obj_iter->dev_id == p_config->dev_id)
				return SII_RETURN_VALUE__DEV_ID_ERR;

			p_obj_iter = sii_sys_obj_next_get(p_obj_iter);
		}
	}

	/* Create object instance */
	p_obj = (struct obj_t *) sii_sys_obj_instance_create(inst_list,
			parent_inst,
			"RX");
	SII_ASSERT(p_obj);

	/* Initialise instance */
	p_obj->dev_id = p_config->dev_id;
	p_obj->event_cb_func = p_config->callback_func;
	p_obj->log_fifo_size = p_config->log_fifo_size;
	p_obj->chip_id = SII9437_CHIP_ID;
	p_obj->chip_version = SII9437_CHIP_REVISION;
	p_obj->event_flags_mask = 0xFFFFFFFF;
	p_obj->event_flags_status = 0;
	p_obj->b_standby_mode = FALSE;
	p_obj->b_earc_hpd = FALSE;
	p_obj->b_earc_hpd_pending = FALSE;
	p_obj->b_hpd_pending = FALSE;
	p_obj->earc_hpd_toggle_time = 100; /* Default 100ms */
	p_obj->b_hpd_in = FALSE;
	p_obj->b_hpd_out = FALSE;
	p_obj->b_earc_connected_status = FALSE;
	p_obj->sm_state = SII9437_SM_STATE__IDLE2;
	p_obj->b_earc_caps_ds_ready = FALSE;

	/* Default arc mode is eARC */
	p_obj->arc_mode_status = SII_ARC_MODE__EARC;
	p_obj->arc_mode_configured = SII_ARC_MODE__EARC;
	p_obj->pref_extraction_mode = SII9437_PREF_EXTRACTION_MODE__I2S;
	p_obj->extraction_mode_status = SII9437_EXTRACTION_MODE__I2S8;
	p_obj->mclock_mode = SII9437_MCLK_MODE__NONE;
	for (i = 0; i < SII9437_CHANNEL_STATUS_SIZE; i++)
		p_obj->channel_status.data[i] = 0;
	for (i = 0; i < SII_CHANNEL_STATUS_SIZE; i++)
		p_obj->channel_status_out.data[i] = 0;
	p_obj->b_audio_mute_user_set = FALSE;
	p_obj->gpio_pin_io_config = 0;
	p_obj->gpio_pin_event_mask = 0;

	p_obj->b_caps_ds_stress_test = FALSE;
	p_obj->audio_mute_disable = 0x00;
	p_obj->earc_lock_detected = TRUE;
	p_obj->earc_clock_detected = FALSE;
	p_obj->ch_status_rcvd = FALSE;
	p_obj->is_input_ch_status_valid = FALSE;
	p_obj->is_clk_det_intr_mask_enable = FALSE;
	p_obj->is_lock_det_intr_mask_enable = FALSE;
	p_obj->is_fs_chng_intr_mask_enable = FALSE;
	p_obj->is_cs_chng_intr_mask_enable = FALSE;
	p_obj->is_pkt_ecc_err_fix_intr_mask_en = FALSE;
	p_obj->is_pkt_ecc_err_intr_mask_en = FALSE;
	p_obj->is_ecc_err_intr_mask_en = FALSE;
	p_obj->erx_latency = p_config->erx_latency;

	SII_MEMSET(p_obj->caps_ds, 0, SII_EARC_CAPS_DS_MAX_LENGTH);
#ifdef SII_LOG_QUEUE
	p_obj->log_fifo = sii_lib_pipe_create("log_fifo",
			p_config->log_fifo_size, sizeof(char));
	sii_sys_log_set_pipe(p_obj->log_fifo);
#endif

#if (SII943X_USER_DEF__MULTI_THREAD)
	/* Create/Initialise resources */
	/* Create mutex for multi-thread safe handling */
	p_obj->mutex = SII_OSAL_MUTEX_CREATE(dev_id);
	SII_ASSERT(p_obj->mutex);
#endif

	/* Create timer sequence to add timers */
	p_obj->seq_inst = sii_sys_seq_create((sii_inst_t) p_obj,
			sii_timer_value_callback);

	SII_ASSERT(p_obj->seq_inst);

	p_obj->earc_hpd_timer = sii_sys_seq_timer_create(p_obj->seq_inst,
			"eARC_HPD_Timer",
			earc_hpd_timer_callback, p_obj);

	p_obj->hpd_timer = sii_sys_seq_timer_create(p_obj->seq_inst,
			"HPD_Timer",
			hpd_timer_callback, p_obj);

	p_obj->audio_wait_timer =
			sii_sys_seq_timer_create(p_obj->seq_inst,
				"Rx_Audio_Wait_Timer",
				audio_wait_timer_callback, p_obj);

	p_obj->comma_init_timer =
			sii_sys_seq_timer_create(p_obj->seq_inst,
				"Rx_Comma_Init_Timer",
				comma_init_timer_callback, p_obj);

	s_sii9437_init(p_obj, p_config);

	*p_inst = (sii_inst_t) p_obj;

	sii9437_hal_chip_version_query(p_obj->dev_id,
					&(p_obj->chip_version));

	s_print_mem_usage(p_obj, "After Sii9437 Create");

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_delete(sii_inst_t inst)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
				inst);

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT_CHECK();

	s_print_mem_usage(p_obj, "Before Sii9437 Delete");

#if (SII943X_USER_DEF__MULTI_THREAD)

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	/* Delete Mutex */
	p_obj->mutex = SII_OSAL_MUTEX_DELETE(p_obj->mutex);
	p_obj->mutex = 0;
#endif

	/* Reset the chip */
	sii9437_hal_chip_reset(p_obj->dev_id);

	/* Delete timers in the reverse order they were created */
	sii_sys_seq_timer_delete(p_obj->comma_init_timer);
	sii_sys_seq_timer_delete(p_obj->audio_wait_timer);
	sii_sys_seq_timer_delete(p_obj->hpd_timer);
	sii_sys_seq_timer_delete(p_obj->earc_hpd_timer);

	/* Delete sequencer instance */
	sii_sys_seq_delete(p_obj->seq_inst);

#ifdef SII_LOG_QUEUE
	/* Delete the logging FIFO */
	if (p_obj->log_fifo)
		sii_lib_pipe_delete(p_obj->log_fifo);
#endif

	/* Delete the object instance */
	if (p_obj) {
		sii_sys_obj_instance_delete(p_obj);
		p_obj = NULL;
	}

	/* Delete instance list if the list is empty*/
	if (inst_list) {
		if (NULL == sii_sys_obj_first_get(inst_list)) {
			sii_sys_obj_list_delete(inst_list);
			inst_list = SII_INST_NULL;
		}
	}

	s_print_mem_usage(p_obj, "After Sii9437 Delete");

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_handle(sii_inst_t inst)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);

	/* If there is any assertion failure, notify the event */
	if (b_assert_status) {
		SII_LOG1A("", NULL, ("Assertion error!\n"));
		s_user_notification_handler(p_obj, SII9437_EVENT_FLAGS__ASSERT);
	}

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	/* Handle notifications and interrupts */
	s_int_handle(p_obj);

	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_chip_id_query(sii_inst_t inst,
	uint32_t *p_chip_id)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_chip_id);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	*p_chip_id = p_obj->chip_id;

	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_chip_version_query(sii_inst_t inst, uint8_t *p_chip_version)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_chip_version);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	*p_chip_version = p_obj->chip_version;

	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_software_version_query(char *version_str, uint8_t size)
{
	char sw_version[MAX_SW_VERSION_LENGTH];
	uint32_t return_value = 0;

	SII_ASSERT_CHECK();
	SII_ASSERT(version_str);
	SII_ASSERT_CHECK();

	SII_STRCPY(sw_version, SII_VERSION, MAX_SW_VERSION_LENGTH);
	SII_STRCAT(sw_version, ".", MAX_SW_VERSION_LENGTH);
	SII_STRCAT(sw_version, SII_REVISION, MAX_SW_VERSION_LENGTH);
	if (SII_STRCMP(SII_REVISION, "unknown")) {
		if (0 != SII_STRLEN(SII_BRANCH)) {
			SII_STRCAT(sw_version, ".", MAX_SW_VERSION_LENGTH);
			SII_STRCAT(sw_version, SII_BRANCH,
			 MAX_SW_VERSION_LENGTH);
		}
	}
	if (0 != SII_STRLEN(SII_USERNAME)) {
		SII_STRCAT(sw_version, "_", MAX_SW_VERSION_LENGTH);
		SII_STRCAT(sw_version, SII_USERNAME, MAX_SW_VERSION_LENGTH);
	}

	SII_STRCPY(version_str, sw_version, size);
	if (size <= SII_STRLEN(sw_version))
		return_value = SII_STRLEN(sw_version) - size + 1;

	return return_value;
}

uint32_t sii9437_log_queue_query(sii_inst_t inst,
	char *log_str, uint32_t size)
{
#ifdef SII_LOG_QUEUE
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);
	uint32_t i;
	enum sii_lib_pipe_error error_code;

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(log_str);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	for (i = 0; i < size; i++) {
		error_code = sii_lib_pipe_read(p_obj->log_fifo, &log_str[i]);
		if (error_code == SII_LIB_PIPE_ERROR_EMPTY)
			break;
	}

	SII_OSAL_MUTEX_CLR(p_obj->mutex);
#endif

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_log_queue_size(sii_inst_t inst, uint32_t *p_size)
{
#ifdef SII_LOG_QUEUE
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_size);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	*p_size = sii_lib_pipe_get_fill_size(p_obj->log_fifo);

	SII_OSAL_MUTEX_CLR(p_obj->mutex);
#else
	SII_ASSERT(p_size);
	*p_size = 0;
#endif

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_assert_query(sii_inst_t inst, uint32_t *p_value)
{
	SII_ASSERT(p_value);

	if (!p_value) {
		SII_ASSERT_CHECK();
	} else {
		*p_value = assert_module_id;
		(*p_value) <<= 24;
		(*p_value) |= assert_line_no;
	}

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_event_flags_mask_set(sii_inst_t inst,
		uint32_t *p_flags)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_flags);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	p_obj->event_flags_mask = *p_flags;

	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_event_flags_mask_get(sii_inst_t inst,
		uint32_t *p_flags)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_flags);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	*p_flags = p_obj->event_flags_mask;

	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_event_flags_status_clear(sii_inst_t inst,
	uint32_t *p_flags)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_flags);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	p_obj->event_flags_status &= (~(*p_flags));

	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_event_flags_query(sii_inst_t inst,
	uint32_t *p_flags)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_flags);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	*p_flags = p_obj->event_flags_status;

	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_standby_set(sii_inst_t inst, bool_t *p_on)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_on);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	s_standby_set(p_obj, p_on);

	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_standby_get(sii_inst_t inst, bool_t *p_on)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_on);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	*p_on = p_obj->b_standby_mode;

	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_gpio_config_set(sii_inst_t inst,
	uint16_t *p_gpio_pin_mask)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_gpio_pin_mask);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	p_obj->gpio_pin_io_config = *p_gpio_pin_mask;

	if (p_obj->b_standby_mode) {
		SII_LOG1A("", NULL,
			("Standby Mode: Skipping GPIO config set\n"));
	} else
		s_gpio_config_update(p_obj);

	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_gpio_config_get(sii_inst_t inst,
	uint16_t *p_gpio_pin_mask)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_gpio_pin_mask);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	*p_gpio_pin_mask = p_obj->gpio_pin_io_config;

	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_gpio_output_set(sii_inst_t inst,
	uint16_t *p_gpio_pin_mask)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_gpio_pin_mask);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	if (p_obj->b_standby_mode) {
		SII_LOG1A("", NULL,
			("Standby Mode: Skipping GPIO output set\n"));
	} else {
		/* Take output pins only from the mask */
		*p_gpio_pin_mask = (*p_gpio_pin_mask) &
				p_obj->gpio_pin_io_config;
		s_gpio_output_update(p_obj, p_gpio_pin_mask, TRUE);
	}

	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_gpio_output_clr(sii_inst_t inst,
	uint16_t *p_gpio_pin_mask)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_gpio_pin_mask);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	if (p_obj->b_standby_mode) {
		SII_LOG1A("", NULL,
			("Standby Mode: Skipping GPIO output clear\n"));
	} else {
		/* Take output pins only from the mask */
		*p_gpio_pin_mask = (*p_gpio_pin_mask) &
				p_obj->gpio_pin_io_config;
		s_gpio_output_update(p_obj, p_gpio_pin_mask, FALSE);
	}

	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_gpio_event_mask_set(sii_inst_t inst,
	uint16_t *p_gpio_pin_mask)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_gpio_pin_mask);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	p_obj->gpio_pin_event_mask = *p_gpio_pin_mask;
	if (p_obj->b_standby_mode) {
		SII_LOG1A("", NULL,
			("Standby Mode: Skipping GPIO event mask set\n"));
	} else
		s_gpio_event_mask_set(p_obj);

	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_gpio_event_mask_get(sii_inst_t inst,
	uint16_t *p_gpio_pin_mask)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_gpio_pin_mask);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	*p_gpio_pin_mask = p_obj->gpio_pin_event_mask;

	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_gpio_query(sii_inst_t inst, uint16_t *p_gpio_pin_mask)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_gpio_pin_mask);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	sii9437_hal_gpio_query(p_obj->dev_id, p_gpio_pin_mask);

	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_earc_caps_ds_set(sii_inst_t inst, uint16_t offset,
	uint8_t *p_data, uint16_t length)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);
	uint32_t status = SII_RETURN_VALUE__SUCCESS;

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_data);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	if (p_obj->b_standby_mode) {
		SII_LOG1A("", NULL, ("Standby Mode: Skipping eARC Caps DS set\n"));
	} else if ((offset + length) <= SII_EARC_CAPS_DS_MAX_LENGTH) {
		SII_MEMCPY(p_obj->caps_ds + offset, p_data, length);

		/* Write Caps DS Data */
		sii9437_hal_caps_ds_set(p_obj->dev_id, p_obj->caps_ds, length);

		/* Set CAP_CHNG bit */
		if (p_obj->b_earc_connected_status)
			sii9437_hal_cap_chng_bit_set(p_obj->dev_id, TRUE);

		p_obj->b_earc_caps_ds_ready = TRUE;
	} else {
		SII_LOG1A("", NULL, ("Error: Invalid Args for Caps DS Set\n"));
		status = SII_RETURN_VALUE__INVALID_ARG_ERR;
	}

	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return status;
}

uint32_t sii9437_earc_caps_ds_get(sii_inst_t inst, uint16_t offset,
	uint8_t *p_data, uint16_t length)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);
	uint32_t status = SII_RETURN_VALUE__SUCCESS;

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_data);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	if ((offset + length) <= SII_EARC_CAPS_DS_MAX_LENGTH) {
		SII_MEMCPY(p_data, p_obj->caps_ds + offset, length);
	} else {
		SII_LOG1A("", NULL, ("Error: Invalid Args for Caps DS Get\n"));
		status = SII_RETURN_VALUE__INVALID_ARG_ERR;
	}

	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return status;
}

uint32_t sii9437_earc_hpd_set(sii_inst_t inst, bool_t *p_hpd)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_hpd);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	if (p_obj->b_standby_mode) {
		SII_LOG1A("", NULL,
			("Standby Mode: Skipping eARC HPD set\n"));
	} else
		s_earc_hpd_set(p_obj, *p_hpd);

	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_earc_hpd_get(sii_inst_t inst, bool_t *p_hpd)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_hpd);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	*p_hpd = p_obj->b_earc_hpd;

	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_earc_hpd_toggle_time_set(sii_inst_t inst,
		uint32_t *p_earc_hpd_toggle_time)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
				inst);
	uint32_t return_code = SII_RETURN_VALUE__SUCCESS;

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_earc_hpd_toggle_time);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	/* Check for minimum of 100ms toggle time configuration */
	if (*p_earc_hpd_toggle_time < 100)
		return_code = SII_RETURN_VALUE__INVALID_ARG_ERR;
	else
		p_obj->earc_hpd_toggle_time = *p_earc_hpd_toggle_time;

	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return return_code;
}

uint32_t sii9437_earc_hpd_toggle_time_get(sii_inst_t inst,
		uint32_t *p_earc_hpd_toggle_time)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
				inst);

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_earc_hpd_toggle_time);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	*p_earc_hpd_toggle_time = p_obj->earc_hpd_toggle_time;

	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_arc_mode_set(sii_inst_t inst,
	enum sii_arc_mode *p_arc_mode)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_arc_mode);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	if (p_obj->b_standby_mode) {
		SII_LOG1A("", NULL,
			("Standby Mode: Skipping ARC mode set\n"));
	} else if (p_obj->arc_mode_status != *p_arc_mode) {
		SII_LOG1A("", NULL, ("ARC mode changed\n"));
		p_obj->arc_mode_status = *p_arc_mode;
		s_arc_change_sm_update(p_obj);
	}

	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_arc_mode_get(sii_inst_t inst,
	enum sii_arc_mode *p_arc_mode)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_arc_mode);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	*p_arc_mode = p_obj->arc_mode_status;

	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_preferred_extraction_mode_set(sii_inst_t inst,
	enum sii9437_pref_extraction_mode *p_mode)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_mode);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	p_obj->pref_extraction_mode = *p_mode;

	if (p_obj->b_standby_mode) {
		SII_LOG1A("", NULL,
			("Standby Mode: Skipping audio update\n"));
	} else
		s_audio_update(p_obj);

	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_preferred_extraction_mode_get(sii_inst_t inst,
	enum sii9437_pref_extraction_mode *p_mode)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_mode);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	*p_mode = p_obj->pref_extraction_mode;

	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_extraction_mode_query(sii_inst_t inst,
	enum sii9437_extraction_mode *p_mode)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_mode);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	*p_mode = p_obj->extraction_mode_status;

	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_i2s_mclk_mode_set(sii_inst_t inst,
	enum sii9437_mclk_mode *p_mode)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_mode);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	p_obj->mclock_mode = *p_mode;
	if (p_obj->b_standby_mode) {
		SII_LOG1A("", NULL,
			("Standby Mode: Skipping MCLK mode set\n"));
	} else
		sii9437_hal_i2s_mclk_mode_set(p_obj->dev_id, *p_mode);

	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_i2s_mclk_mode_get(sii_inst_t inst,
	enum sii9437_mclk_mode *p_mode)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_mode);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	*p_mode = p_obj->mclock_mode;

	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_channel_status_query(sii_inst_t inst,
	struct sii_channel_status *p_channel_status)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_channel_status);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	SII_MEMCPY(p_channel_status->data,
		p_obj->channel_status_out.data,
		SII_CHANNEL_STATUS_SIZE);

	//~ s_print_channel_status(p_obj, p_obj->channel_status.data, FALSE);

	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_mute_enable_set(sii_inst_t inst, bool_t *p_flag)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_flag);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	if (p_obj->b_audio_mute_user_set != *p_flag) {
		p_obj->b_audio_mute_user_set = *p_flag;

		if (p_obj->b_standby_mode) {
			SII_LOG1A("", NULL,
				("Standby Mode: Skipping mute enable set\n"));
		} else {
			/* Set Mute in the register */
			sii9437_hal_aud_mute_set(p_obj->dev_id, *p_flag);
		}

		p_obj->mute_state = *p_flag;
	}
	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_mute_enable_get(sii_inst_t inst, bool_t *p_flag)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_flag);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	*p_flag = p_obj->b_audio_mute_user_set;

	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hpd_query(sii_inst_t inst, bool_t *p_hpd)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_hpd);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	*p_hpd = p_obj->b_hpd_out;

	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_audio_info_query(sii_inst_t inst,
		struct sii_audio_info *p_audio_info)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_audio_info);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	*p_audio_info = p_obj->audio_info;

	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_erx_latency_set(sii_inst_t inst,
				struct sii_erx_latency *p_erx_latency)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_erx_latency);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	p_obj->erx_latency.data = p_erx_latency->data;

	if (p_obj->b_standby_mode) {
		SII_LOG1A("", NULL,
			("Standby Mode: Skipping latency set\n"));
	} else {
		/* Set latency */
		sii9437_hal_erx_latency_set(p_obj->dev_id,
					p_obj->erx_latency.data);
		sii9437_hal_stat_chng_bit_set(p_obj->dev_id, TRUE);
	}

	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_erx_latency_get(sii_inst_t inst,
				struct sii_erx_latency *p_erx_latency)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_erx_latency);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	p_erx_latency->data = p_obj->erx_latency.data;

	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_erx_latency_req_query(sii_inst_t inst,
				struct sii_erx_latency_req *p_erx_latency_req)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_erx_latency_req);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	sii9437_hal_erx_latency_req_query(p_obj->dev_id,
					&(p_erx_latency_req->data));

	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_earc_link_query(sii_inst_t inst, bool_t *p_on)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_on);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	*p_on = p_obj->b_earc_connected_status;

	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_sample_rate_query(sii_inst_t inst, uint32_t *p_sample_rate)
{
	struct obj_t *p_obj = (struct obj_t *)SII_SYS_OBJ_CHECK(inst_list,
			inst);

	SII_ASSERT_CHECK();
	SII_ASSERT(p_obj);
	SII_ASSERT(p_sample_rate);
	SII_ASSERT_CHECK();

	SII_OSAL_MUTEX_SET(p_obj->mutex);

	*p_sample_rate = p_obj->sample_rate;

	SII_OSAL_MUTEX_CLR(p_obj->mutex);

	return SII_RETURN_VALUE__SUCCESS;
}

/***** local functions ******************************************************/

static void s_ckdt_mode_update(struct obj_t *p_obj)
{
	uint32_t earc_clk_freq;
	bool_t ckdt_mode_set;

	sii9437_hal_rx_earc_clk_freq_get(p_obj->dev_id, &earc_clk_freq);

	if ((earc_clk_freq > 65000000) || (earc_clk_freq == 0))
		ckdt_mode_set = TRUE;
	else
		ckdt_mode_set = FALSE;

	sii9437_hal_clk_lock_mode_set(p_obj->dev_id, ckdt_mode_set);

	SII_LOG1A("", p_obj, ("Clock Detect Mode: %d\n", ckdt_mode_set));
}
static void s_int_handle(struct obj_t *p_obj)
{
	uint8_t intRegs[REG_RX__INT_NUM];
	uint8_t int_top_regs[REG_TOP__INT_NUM];
	uint8_t reg_top;
	uint32_t evt_flgs_notify = 0;
	uint16_t gpio_values;
	bool_t is_audio_update = FALSE;
	bool_t b_ckdt_mode_update = FALSE;

#ifdef SII_LOG_QUEUE
	if (sii_lib_pipe_get_fill_size(p_obj->log_fifo))
		s_user_notification_handler(p_obj,
			SII9437_EVENT_FLAGS__LOG_FIFO_CHNG);
#endif

	sii9437_hal_top_int_get(p_obj->dev_id, &reg_top);

	if (reg_top & BIT_MSK__TOP_INTR_STATE__INTR_STATE_TOP) {

		SII_LOG1A("", NULL, ("__Processing interrupts__\n"));
		/* Disable all Interrupts */
		sii9437_hal_enable_all_int(p_obj->dev_id, FALSE);

		/* Read all top interrupt status registers This function
		automatically clears the interrupts after read */
		sii9437_hal_top_int_status_get(p_obj->dev_id,
						int_top_regs,
						REG_TOP__INT_NUM);

		SII_LOG1A("", NULL,
				("Top INTR status: 0x%02x, 0x%02x, 0x%02x\n",
				int_top_regs[REG_INTR_TOP_1],
				int_top_regs[REG_INTR_TOP_2],
				int_top_regs[REG_INTR_TOP_3]));

		/* Check timer interrupt */
		if (int_top_regs[REG_INTR_TOP_1] &
			BIT_MSK__TOP_INTR1__REG_TOP_INTR_STAT1) {
			SII_LOG1A("", NULL,
				("Timer interrupt received\n"));

			sii_sys_seq_handle(p_obj->seq_inst);
		}

		sii9437_hal_gpio_query(p_obj->dev_id, &gpio_values);
		SII_LOG1A("", NULL, ("GPIO values: 0x%02x\n",
			gpio_values));

		/* Check HPD-IN change from GPIO4 */
		if ((int_top_regs[REG_INTR_TOP_2] &
			BIT_MSK__TOP_INTR2__REG_TOP_INTR_STAT10) ||
			(int_top_regs[REG_INTR_TOP_3] &
			BIT_MSK__TOP_INTR3__REG_TOP_INTR_STAT16)) {
			bool_t b_hpd_in_val;
			uint16_t gpio_in_val;

			sii9437_hal_gpio_query(p_obj->dev_id,
					&gpio_in_val);
			b_hpd_in_val = gpio_in_val & BIT__GPIO_NUM_4;

			SII_LOG1A("", NULL,
				("GPIO4 (RX HPD IN) changed: %s\n",
					b_hpd_in_val ? "hi" : "low"));

			s_hpd_update(p_obj,
				p_obj->b_earc_connected_status,
				b_hpd_in_val, &evt_flgs_notify);

			p_obj->b_hpd_in = b_hpd_in_val;
		}

		if (p_obj->is_clk_det_intr_mask_enable &&
			(int_top_regs[REG_INTR_TOP_1] &
			BIT_MSK__TOP_INTR1__REG_TOP_INTR_STAT2)) {
			SII_LOG1A("", p_obj,
				("Rx Clock status change\n"));

			if (p_obj->arc_mode_status ==
					SII_ARC_MODE__EARC) {
				sii9437_hal_earc_clock_status_query(
						p_obj->dev_id,
						&p_obj->earc_clock_detected);

				/* Reset Rx status*/
				s_status_reset(p_obj);

				b_ckdt_mode_update = TRUE;

				if (p_obj->earc_clock_detected) {
					SII_LOG1A("", p_obj,
						("----eARC clock Detected\n"));
				} else {
					SII_LOG1A("", p_obj,
					("----eARC clock NOT Detected\n"));
				}
			}
		}

		if (p_obj->is_lock_det_intr_mask_enable &&
				(int_top_regs[REG_INTR_TOP_1] &
			BIT_MSK__TOP_INTR1__REG_TOP_INTR_STAT3)) {
			SII_LOG1A("", p_obj,
				("Rx lock status change\n"));

			s_earc_lock_chng_intr_mask_en(p_obj, FALSE);

			if (p_obj->arc_mode_status ==
					SII_ARC_MODE__EARC) {

				p_obj->earc_lock_detected = TRUE;

				/* Reset Rx status*/
				s_status_reset(p_obj);

				b_ckdt_mode_update = TRUE;

				if (p_obj->earc_lock_detected) {
					SII_LOG1A("", p_obj,
						("eARC Lock Detected\n"));
				} else {
					SII_LOG1A("", p_obj,
						("eARC Lock NOT Detected\n"));
				}
			}
		}

		if (b_ckdt_mode_update)
			s_ckdt_mode_update(p_obj);

		if (reg_top & BIT_MSK__TOP_INTR_STATE__INTR_STATE_RX) {
			/* SII_LOG1A("", p_obj, ("got rx Interrupt\n")); */
			uint8_t earc_int;

			sii9437_hal_earc_block_int_get(p_obj->dev_id,
							&earc_int);
			if (earc_int & BIT_MSK__INTR_STATE__INTR_STATE) {
				sii9437_hal_earc_int_status_get(p_obj->dev_id,
					intRegs, REG_RX__INT_NUM);

				s_sm_state_update(p_obj, intRegs);

				if (intRegs[REG_INTR_7] &
					BIT_MSK__INTR7__REG_INTR_STAT56) {
					/* Virtual HPD changed */
					SII_LOG1A("", p_obj,
						("Virtual HPD changed\n"));
					s_vhpd_read(p_obj, &evt_flgs_notify);
				}

				if (intRegs[REG_INTR_4] &
					BIT_MSK__INTR4__REG_INTR_STAT33) {
					bool_t is_heart_beat_lost;
					sii9437_hal_is_heartbeat_lost(
						p_obj->dev_id,
						&is_heart_beat_lost);
					if (is_heart_beat_lost) {
						SII_LOG1A("", p_obj,
						("RX heartbeat lost\n"));
					} else {
						SII_LOG1A("", p_obj,
						("RX heartbeat success\n"));
					}
				}

				if (intRegs[REG_INTR_3] &
					BIT_MSK__INTR3__REG_INTR_STAT31) {
					if (p_obj->b_caps_ds_stress_test) {
						SII_LOG1A("", p_obj,
						("RX got CAPS DS interrupt\n"));
					}
				}

				if (intRegs[REG_INTR_7] &
					BIT_MSK__INTR7__REG_INTR_STAT59) {
					bool_t b_cap_chng_conf;

					sii9437_hal_tx_cap_chng_conf_query(
							p_obj->dev_id,
							&b_cap_chng_conf);

					SII_LOG1A("", p_obj,
					("CAP_CHNG_CONF from TX Intr - %s\n",
						b_cap_chng_conf ? "TRUE" :
							"FALSE"));

					/* Clear CAP_CHNG bit in 0xD0 if
					 * CAP_CHNG_CONF is 1 */
					if (b_cap_chng_conf)
						sii9437_hal_cap_chng_bit_set(
							p_obj->dev_id, FALSE);
				}

				if (intRegs[REG_INTR_7] &
					BIT_MSK__INTR7__REG_INTR_STAT60) {
					bool_t b_stat_chng_conf;

					sii9437_hal_tx_stat_chng_conf_query(
							p_obj->dev_id,
							&b_stat_chng_conf);

					SII_LOG1A("", p_obj,
					("STAT_CHNG_CONF from TX Intr - %s\n",
						b_stat_chng_conf ? "TRUE" :
							"FALSE"));

					/* Clear Stat chng bit in 0xD0 if
					 * STAT_CHNG_CONF is 1 */
					if (b_stat_chng_conf)
						sii9437_hal_stat_chng_bit_set(
							p_obj->dev_id, FALSE);
				}

				if (intRegs[REG_INTR_8] &
					BIT_MSK__INTR8__REG_INTR_STAT66) {
					SII_LOG1A("", p_obj,
						("TX Latency Request intr\n"));
					s_erx_latency_req(p_obj,
						 &evt_flgs_notify);
				}

				if (intRegs[REG_INTR_8] &
					BIT_MSK__INTR8__REG_INTR_STAT65) {
					SII_LOG1A("", p_obj,
					("TX read Latency successfully\n"));
				}

				if (intRegs[REG_INTR_6] &
					BIT_MSK__INTR6__REG_INTR_STAT51) {

					s_earc_only_intr_mask_en(
							p_obj,
							FALSE);

					SII_LOG1A("", p_obj,
					("RX Discovery Timeout Asserted\n"));
					evt_flgs_notify |=
					SII9437_EVENT_FLAGS__EARC_DISC_TIMEOUT;
				}

				if (intRegs[REG_INTR_6] &
					BIT_MSK__INTR6__REG_INTR_STAT52) {
					SII_LOG1A("", p_obj,
					("RX Discovery Timeout De-asserted\n"));
				}

				if (intRegs[REG_INTR_8] &
					BIT_MSK__INTR8__REG_INTR_STAT64) {
					bool_t val;
					sii9437_hal_conn_status_get(
							p_obj->dev_id, &val);

					/* Reset Rx status*/
					s_status_reset(p_obj);

					if (val) {
						s_earc_only_intr_mask_en(
								p_obj,
								TRUE);
						SII_LOG1A("", p_obj,
						("RX eARC connected\n"));

					} else {
						s_earc_only_intr_mask_en(
								p_obj,
								FALSE);
						sii9437_hal_tx_hb_status_clear(
								p_obj->dev_id);
						SII_LOG1A("", p_obj,
						("RX eARC Disconnected\n"));

						is_audio_update = TRUE;
					}
					s_hpd_update(p_obj, val,
						p_obj->b_hpd_in,
						&evt_flgs_notify);
					p_obj->b_earc_connected_status = val;

					s_rx_earc_connect_config(p_obj);

					evt_flgs_notify |=
					SII9437_EVENT_FLAGS__EARC_LINK_CHNG;
				}

				if (p_obj->is_fs_chng_intr_mask_enable &&
					(intRegs[REG_INTR_2] &
					BIT_MSK__INTR2__REG_INTR_STAT16)) {

					p_obj->ch_status_rcvd = FALSE;
					p_obj->is_input_ch_status_valid = FALSE;
					is_audio_update = TRUE;

					SII_LOG1A("", p_obj,
						("Sample Rate changed\n"));
				}

				if (p_obj->is_cs_chng_intr_mask_enable &&
					(intRegs[REG_INTR_2] &
					BIT_MSK__INTR2__REG_INTR_STAT19)) {

					SII_LOG1A("", p_obj,
						("Ch. Status changed\n"));

					s_cs_change_check(p_obj,
							&is_audio_update);

					/*is_audio_update = TRUE;*/
				}

				if (p_obj->is_pkt_ecc_err_fix_intr_mask_en &&
						(intRegs[REG_INTR_4] &
				BIT_MASK__INTR4_MASK__PKT_ECC_ERR_FIXED)) {
					uint16_t err_fixed;
					sii9437_hal_ecc_err_fixed_cnt_get(
						p_obj->dev_id, &err_fixed);
					SII_LOG1A("", p_obj,
					("One pkt ecc err is fixed - %d\n",
							err_fixed));
					sii9437_hal_ecc_err_fixed_cnt_clear(
							p_obj->dev_id);
				}

				if (p_obj->is_pkt_ecc_err_intr_mask_en &&
						(intRegs[REG_INTR_6] &
					BIT_MASK__INTR4_MASK__PKT_ECC_ERR)) {
					uint16_t err_cnt;
					sii9437_hal_ecc_err_cnt_get(
							p_obj->dev_id,
							&err_cnt);
					SII_LOG1A("", p_obj,
					("Pkt has uncorrectable ECC ERR - %d\n",
						err_cnt));
					sii9437_hal_ecc_err_cnt_clear(
							p_obj->dev_id);
				}

				if (p_obj->is_ecc_err_intr_mask_en &&
						(intRegs[REG_INTR_6] &
					BIT_MASK__INTR4_MASK__ECC_ERR)) {
					uint16_t err_cnt;
					sii9437_hal_ecc_err_cnt_get(
							p_obj->dev_id,
							&err_cnt);
					SII_LOG1A("", p_obj,
					("unexpected pkts or ECC errors - %d\n",
						err_cnt));
					sii9437_hal_ecc_err_cnt_clear(
							p_obj->dev_id);
				}

				if (p_obj->b_earc_connected_status &&
					(intRegs[REG_INTR_1] &
					BIT_MSK__INTR1__REG_INTR_STAT12)) {

					SII_LOG1A("", p_obj,
					("Parity error in incoming stream\n"));

					/* Reset Rx status*/
					s_status_reset(p_obj);
					sii9437_hal_rx_lock_toggle(
							p_obj->dev_id);
				}

			}
		}

		if (!(p_obj->earc_clock_detected))
			p_obj->is_input_ch_status_valid = FALSE;
		if (is_audio_update)
			s_audio_update(p_obj);

		/* Check GPIO status changes */
		/* If any one of the GPIO0 to GPIO3 status registers are not 0,
		 * there would be GPIO change event.
		 * GPIO4 and GPIO5 are reserved for HDP IN and HPD OUT
		 * respectively, so we don't check their status here */
		if ((int_top_regs[REG_INTR_TOP_2] &
			BIT_MASK__TOP_INTR2__REG_INTR_GEN_GPIO)|
			(int_top_regs[REG_INTR_TOP_3] &
			BIT_MASK__TOP_INTR3__REG_INTR_GEN_GPIO))
			evt_flgs_notify |= SII9437_EVENT_FLAGS__GPIO_CHNG;

		/* enable all interrupts */
		sii9437_hal_enable_all_int(p_obj->dev_id, TRUE);
	}

	/* If the mute bit has not been accounted for copy and event it */
	if ((p_obj->channel_status_out.data[8] & 0x04) != (p_obj->channel_status.data[8] & 0x04)) {
		p_obj->channel_status_out.data[8] &= ~0x04;
		p_obj->channel_status_out.data[8] |= p_obj->channel_status.data[8] & 0x04;
		evt_flgs_notify |= SII9437_EVENT_FLAGS__CHAN_STAT_CHNG;
	}

	if (evt_flgs_notify) {
		SII_LOG1A("", p_obj,
			("Event Notification flags: 0x%02x\n",
			evt_flgs_notify));
		s_user_notification_handler(p_obj, evt_flgs_notify);
	}
#ifdef SII_LOG_QUEUE
	if (sii_lib_pipe_get_fill_size(p_obj->log_fifo))
		s_user_notification_handler(p_obj,
			SII9437_EVENT_FLAGS__LOG_FIFO_CHNG);
#endif

}

static uint32_t s_earc_lock_chng_intr_mask_en(struct obj_t *p_obj, bool_t en)
{
	sii9437_hal_earc_lock_chg_intr_en(p_obj->dev_id, en);
	/* set flags to use in interrupt handler*/
	p_obj->is_lock_det_intr_mask_enable = en;

	return SII_RETURN_VALUE__SUCCESS;
}

static uint32_t s_earc_only_intr_mask_en(struct obj_t *p_obj, bool_t en)
{
	sii9437_hal_earc_only_intr_en(p_obj->dev_id, en);
	/* set flags to use in interrupt handler*/
	p_obj->is_clk_det_intr_mask_enable = en;
	p_obj->is_lock_det_intr_mask_enable = en;
	p_obj->is_fs_chng_intr_mask_enable = en;
	p_obj->is_cs_chng_intr_mask_enable = en;
	p_obj->is_pkt_ecc_err_fix_intr_mask_en = en;
	p_obj->is_pkt_ecc_err_intr_mask_en = en;
	p_obj->is_ecc_err_intr_mask_en = en;

	return SII_RETURN_VALUE__SUCCESS;
}

static uint32_t s_cs_change_check(struct obj_t *p_obj, bool_t *is_audio_update)
{
	uint8_t ch_status[SII9437_CHANNEL_STATUS_SIZE];
	uint8_t cs_change = 0;
	uint16_t i = 0;
	bool_t mute_change = FALSE;
	uint8_t all_ff_count = 0;

	sii9437_hal_channel_status_query(
			p_obj->dev_id,
			ch_status,
			SII9437_CHANNEL_STATUS_SIZE);

	p_obj->is_input_ch_status_valid = FALSE;

	/* Discard Mute bit change in channel status as HW will take care of
	 * Mute/Unmute */
	for (i = 0; i < SII9437_CHANNEL_STATUS_SIZE; i++) {
		if (i != 8)
			if (ch_status[i] != p_obj->channel_status.data[i])
				cs_change = TRUE;
		/*Validate channel status*/
		if (ch_status[i] != 0)
			p_obj->is_input_ch_status_valid = TRUE;
		if (ch_status[i] == 0xFF)
			all_ff_count++;
	}

	if (all_ff_count > 5)
		p_obj->is_input_ch_status_valid = FALSE;

	if ((ch_status[8] & 0xFB) != (p_obj->channel_status.data[8] & 0xFB))
		cs_change = TRUE;

	if ((ch_status[8] & 0x04) != (p_obj->channel_status.data[8] & 0x04))
		mute_change = TRUE;

	s_print_channel_status(p_obj, p_obj->channel_status.data, TRUE);
	s_print_channel_status(p_obj, ch_status, TRUE);

	SII_MEMCPY(p_obj->channel_status.data,
			ch_status,
			SII9437_CHANNEL_STATUS_SIZE);

	SII_LOG1A("", p_obj,
			("Mute: %s \n",
			(ch_status[8] & 0x04) ? "TRUE" : "FALSE"));

	if (p_obj->is_input_ch_status_valid) {
		/* Clear ckdt mode for freq < 65MHz.*/
		s_clk_lock_mode_set(p_obj, ch_status);
		/* ckdt is not valid for freq < 65 MHz. So make it true.*/
		p_obj->earc_clock_detected = TRUE;
	}

	if (!p_obj->is_input_ch_status_valid) {
		cs_change = FALSE;
		*is_audio_update = TRUE;
	} else if (cs_change) {
		*is_audio_update = TRUE;
	} else if (mute_change) {
		*is_audio_update = FALSE;
	} else {
		*is_audio_update = FALSE;
	}

	p_obj->ch_status_rcvd = cs_change;

	return cs_change;
}

static void s_status_reset(struct obj_t *p_obj)
{
	if (p_obj->arc_mode_status != SII_ARC_MODE__ARC) {
		/* Mute output Audio */
		s_audio_mute(p_obj, TRUE);

		/* Disable Output */
		s_output_io_enable(p_obj, FALSE);
	}

	sii9437_hal_rx_softreset(p_obj->dev_id);

	/* Disable ECC */
	sii9437_hal_ecc_enable(p_obj->dev_id, FALSE);

	SII_MEMSET(p_obj->channel_status.data, 0,
			SII9437_CHANNEL_STATUS_SIZE);
	p_obj->ch_status_rcvd = FALSE;
	p_obj->is_input_ch_status_valid = FALSE;
}

static void s_rx_earc_connect_config(struct obj_t *p_obj)
{
	if (p_obj->b_earc_connected_status) {
		/* Set latency */
		sii9437_hal_erx_latency_set(p_obj->dev_id,
				p_obj->erx_latency.data);
		sii9437_hal_stat_chng_bit_set(p_obj->dev_id, TRUE);
		SII_LOG1A("", NULL, ("Audio latency set: %d msec\n",
				p_obj->erx_latency.data));
	} else {
		sii9437_hal_erx_latency_set(p_obj->dev_id, 0x00);
		sii9437_hal_stat_chng_bit_set(p_obj->dev_id, FALSE);
		sii9437_hal_cap_chng_bit_set(p_obj->dev_id, TRUE);
	}
}

static void s_erx_latency_req(struct obj_t *p_obj,
				uint32_t *evt_flgs_notify)
{
	sii9437_hal_erx_latency_req_query(p_obj->dev_id,
				&(p_obj->erx_latency_req.data));
	*evt_flgs_notify |=
		SII9437_EVENT_FLAGS__ERX_LATENCY_REQ_CHNG;
	SII_LOG1A("", p_obj,
	("Latency Req from Tx (0xD3): %d msec\n",
					p_obj->erx_latency_req.data));

}
static void s_sm_state_update(struct obj_t *p_obj, uint8_t *int_regs)
{
	uint8_t sm_state;
	if ((int_regs[REG_INTR_0] & BIT_MASK__INTR0_MASK__STATE_IDLE1) ||
		(int_regs[REG_INTR_0] & BIT_MASK__INTR0_MASK__STATE_IDLE2) ||
		(int_regs[REG_INTR_1] & BIT_MASK__INTR1_MASK__STATE_DISC1) ||
		(int_regs[REG_INTR_1] & BIT_MASK__INTR1_MASK__STATE_DISC2) ||
		(int_regs[REG_INTR_1] & BIT_MASK__INTR1_MASK__STATE_ARC) ||
		(int_regs[REG_INTR_4] & BIT_MASK__INTR4_MASK__STATE_EARC)) {

		sii9437_hal_sm_state_get(p_obj->dev_id, &sm_state);
		p_obj->sm_state = sm_state;
		SII_LOG1A("", p_obj, ("State machine state changed to %s\n",
					sm_states[sm_state]));
		s_state_change_sm_update(p_obj);
	}
}

static void s_arc_change_sm_update(struct obj_t *p_obj)
{
	if (p_obj->arc_mode_status == SII_ARC_MODE__NONE) {
		/* If the ARC mode is NONE and state is not IDLE2,
		 * force state to IDLE2 and freeze it there */
		sii9437_hal_sm_state_set(p_obj->dev_id,
				SII9437_SM_STATE__IDLE2);
		sii9437_hal_sm_state_override_enable(p_obj->dev_id, TRUE);

		/* If the state is already in IDLE2, update with
		 * ARC_MODE__NONE configuration */
		if (p_obj->sm_state == SII9437_SM_STATE__IDLE2)
			s_arc_mode_update(p_obj, p_obj->arc_mode_status);

	} else if (p_obj->arc_mode_status == SII_ARC_MODE__ARC) {
		/* If the ARC mode is legacy ARC force the state to ARC
		 * freeze it there until ARC mode is changed */
		sii9437_hal_sm_state_set(p_obj->dev_id,
				SII9437_SM_STATE__ARC);
		sii9437_hal_sm_state_override_enable(p_obj->dev_id, TRUE);

	} else if (p_obj->arc_mode_status == SII_ARC_MODE__EARC) {
		/* If the current state is IDLE2, configure eARC and unfreeze */
		if (p_obj->sm_state == SII9437_SM_STATE__IDLE2) {

			if (p_obj->arc_mode_status == SII_ARC_MODE__EARC &&
				p_obj->arc_mode_configured !=
						SII_ARC_MODE__EARC)
				s_arc_mode_update(p_obj,
						p_obj->arc_mode_status);

			sii9437_hal_sm_state_override_enable(p_obj->dev_id,
					FALSE);
		} else if (p_obj->sm_state == SII9437_SM_STATE__ARC) {
			sii9437_hal_sm_state_override_enable(p_obj->dev_id,
								FALSE);
			/* If the current state is ARC, terminate it */
			sii9437_hal_arc_init_term(p_obj->dev_id, FALSE);
		} else if (p_obj->sm_state == SII9437_SM_STATE__IDLE1) {
			/* If the current state is in IDLE1 and if eARC is
			 * not configured yet, do it now */
			if (p_obj->arc_mode_configured != SII_ARC_MODE__EARC) {
				s_arc_mode_update(p_obj,
					p_obj->arc_mode_status);
			}
		}
	}
}

static void s_state_change_sm_update(struct obj_t *p_obj)
{
	if (p_obj->sm_state == SII9437_SM_STATE__IDLE2) {
		if (p_obj->arc_mode_status == SII_ARC_MODE__ARC)
			sii9437_hal_arc_init_term(p_obj->dev_id, TRUE);
		else if (p_obj->arc_mode_status == SII_ARC_MODE__EARC &&
			p_obj->arc_mode_configured != SII_ARC_MODE__EARC) {
			/* Update eARC configuration */
			s_arc_mode_update(p_obj, p_obj->arc_mode_status);
		} else if (p_obj->arc_mode_status == SII_ARC_MODE__NONE) {
			/* Update ARC_NONE configuration */
			s_arc_mode_update(p_obj, p_obj->arc_mode_status);
		}
	} else if (p_obj->sm_state == SII9437_SM_STATE__ARC) {
		/* Update ARC configuration */
		s_arc_mode_update(p_obj, p_obj->arc_mode_status);
	} else if (p_obj->sm_state == SII9437_SM_STATE__IDLE1) {
		/* In case state transition happened from ARC to IDLE1 on HPD
		 * low and ARC mode is set to eARC, configure eARC properly */
		if (p_obj->arc_mode_status == SII_ARC_MODE__EARC &&
					p_obj->arc_mode_configured !=
						SII_ARC_MODE__EARC) {
			/* Update eARC configuration */
			s_arc_mode_update(p_obj, p_obj->arc_mode_status);
		}
	}
}

static void s_gpio_config_update(struct obj_t *p_obj)
{
	sii9437_hal_gpio_config(p_obj->dev_id, &p_obj->gpio_pin_io_config);
}

static void s_gpio_output_update(struct obj_t *p_obj,
		uint16_t *p_gpio_pin_mask, bool_t b_set)
{
	sii9437_hal_gpio_output_update(p_obj->dev_id, p_gpio_pin_mask, b_set);
}

static void s_gpio_event_mask_set(struct obj_t *p_obj)
{
	/* Get the event mask for GPIO inputs */
	uint8_t gpio_event_mask = 0x3F & p_obj->gpio_pin_event_mask &
				(~p_obj->gpio_pin_io_config);

	sii9437_hal_gpio_event_mask_set(p_obj->dev_id, &gpio_event_mask);
}

static void s_sii9437_init(struct obj_t *p_obj,
		const struct sii9437_config *p_config)
{
	sii9437_hal_rx_init_stage_1(p_obj->dev_id, p_config);

	p_obj->b_ouput_io_en_state = TRUE;
	s_output_io_enable(p_obj, FALSE);

	s_hpd_init(p_obj);

	if (p_obj->b_hpd_in) {
		uint8_t int_top_regs[REG_TOP__INT_NUM] = {0};

		/* Read all top interrupt status registers This function
		automatically clears the interrupts after read
		This read is to clear any spurious interrupts */
		sii9437_hal_top_int_status_get(p_obj->dev_id, int_top_regs,
							REG_TOP__INT_NUM);
		sii_sys_seq_timer_start(p_obj->comma_init_timer,
				SII9437_COMMA_INIT_TIMER_VAL, 0);
	} else {
		s_sii9437_init_stage_2(p_obj);
	}
}

static void s_sii9437_init_stage_2(struct obj_t *p_obj)
{
	uint8_t int_top_regs[REG_TOP__INT_NUM] = {0};

	sii9437_hal_caps_ds_set(p_obj->dev_id, p_obj->caps_ds,
			SII_EARC_CAPS_DS_MAX_LENGTH);
	sii9437_hal_rx_init_stage_2(p_obj->dev_id);

	p_obj->mute_state = FALSE;
	s_audio_mute(p_obj, TRUE);

	sii9437_hal_rx_cs_mute_mask_pattern_set(p_obj->dev_id);

	/* Read all top interrupt status registers This function
	automatically clears the interrupts after read
	This read is to clear any spurious interrupts */
	sii9437_hal_top_int_status_get(p_obj->dev_id, int_top_regs,
						REG_TOP__INT_NUM);
	sii9437_hal_int_mask_enable(p_obj->dev_id, TRUE);

	sii9437_hal_cap_chng_bit_set(p_obj->dev_id, TRUE);

	SII_LOG1A("", p_obj, ("Enabling COMMA...\n"));
	/* Enable COMMA after all initialisation is done */
	sii9437_hal_rx_comma_enable(p_obj->dev_id, TRUE);
}

static void s_hpd_init(struct obj_t *p_obj)
{
	bool_t b_hpd_in_value;

	sii9437_hal_hpd_init(p_obj->dev_id, &b_hpd_in_value);

	p_obj->b_hpd_in = b_hpd_in_value;
	p_obj->b_hpd_out = b_hpd_in_value;
}

char *s_getting_freq_name(uint8_t fs)
{
	uint8_t i;
	uint8_t length = 18;
	for (i = 0; i < length; i++) {
		if (fs_table[i].fs == fs)
			return fs_table[i].fsName;
	}
	return "Invalid Freq";
}

static uint16_t s_get_freq(uint8_t fs)
{
	uint8_t i;
	uint8_t length = 18;
	for (i = 0; i < length; i++) {
		if (fs_table[i].fs == fs)
			return fs_table[i].fs_val;
	}
	return SII_RETURN_VALUE__FAIL;
}

static uint8_t s_get_code(uint16_t fs_val)
{
	uint8_t i;
	uint8_t length = 18;
	for (i = 0; i < length; i++) {
		if (fs_table[i].fs_val == fs_val)
			return fs_table[i].fs;
	}
	return SII_RETURN_VALUE__FAIL;
}

static void s_print_channel_status(struct obj_t *p_obj, uint8_t *p_data,
		bool_t b_extra_bytes)
{
	if (b_extra_bytes) {
		SII_LOG1A("", p_obj,
			("CS: %02x.%02x.%02x.%02x.%02x.%02x.%02x...%02x.%02x.%02x.%02x.%02x.%02x.%02x",
			p_data[0],  p_data[1],  p_data[2],  p_data[3],
			p_data[4],  p_data[5],  p_data[6],
			p_data[7],  p_data[8],  p_data[9],  p_data[10],
			p_data[11], p_data[12],  p_data[13]));
	} else {
		SII_LOG1A("", p_obj,
			("CS: %02x.%02x.%02x.%02x.%02x",
			p_data[0],  p_data[1],  p_data[2],  p_data[3],
			p_data[4]));
	}
}

static void s_aif_extraction_get(struct obj_t *p_obj,
		uint8_t *p_ch_stat,
		struct sii_audio_info *p_audio_info)
{
	p_audio_info->speaker_alloc = p_ch_stat[7];

	p_audio_info->lfepbl = (p_ch_stat[8] &
			BIT_MASK__RX_CS_EXTRA2__EARC_LFE);
	p_audio_info->lsv = (p_ch_stat[8] &
			BIT_MASK__RX_CS_EXTRA2__EARC_LSV) >>
				BIT_MASK__RX_CS_EXTRA2__EARC_LSV_SHIFT;
	p_audio_info->dm_inh = (p_ch_stat[8] &
			BIT_MASK__RX_CS_EXTRA2__EARC_DM_INH) >>
				BIT_MASK__RX_CS_EXTRA2__EARC_DM_INH_SHIFT;

	SII_LOG1A("", NULL,
		("Spkr Alloc: 0x%x\n",
			p_audio_info->speaker_alloc));
	SII_LOG1A("", NULL,
		("lfe: 0x%x, lsv: 0x%x, dm_inh: 0x%x\n",
			p_audio_info->lfepbl,
			p_audio_info->lsv,
			p_audio_info->dm_inh));
	SII_LOG1A("", NULL, ("Audio Info: b[17]: 0x%x, b[18]: 0x%x\n",
			p_ch_stat[6], p_ch_stat[7]));
}

static bool_t s_is_hbra(struct obj_t *p_obj)
{
	return (((p_obj->channel_status.data[3] & 0xCF) ==
				SII_FREQ_4x192_KHZ) ||
			((p_obj->channel_status.data[3] & 0xCF) ==
					SII_FREQ_384_KHZ) ||
			((p_obj->channel_status.data[3] & 0xCF) ==
					SII_FREQ_1536_KHZ) ||
			((p_obj->channel_status.data[3] & 0xCF) ==
					SII_FREQ_705_6_KHZ) ||
			((p_obj->channel_status.data[3] & 0xCF) ==
					SII_FREQ_352_8_KHZ) ||
			((p_obj->channel_status.data[3] & 0xCF) ==
					SII_FREQ_1411_2_KHZ) ||
			((p_obj->channel_status.data[3] & 0xCF) ==
					SII_FREQ_256_KHZ) ||
			((p_obj->channel_status.data[3] & 0xCF) ==
					SII_FREQ_512_KHZ) ||
			((p_obj->channel_status.data[3] & 0xCF) ==
					SII_FREQ_512_KHZ) ||
			((p_obj->channel_status.data[3] & 0xCF) ==
					SII_FREQ_1024_KHZ)) ?
			TRUE : FALSE;
}

static bool_t s_is_non_lpcm(struct obj_t *p_obj)
{
	bool_t is_nonlpcm = FALSE;

	switch (p_obj->channel_status.data[0] &
			SII_CHANSTATUS_PATTERN__MASK) {
	case SII_CHANSTATUS_PATTERN__NON_PCM:
		is_nonlpcm = TRUE;
		break;
	}
	return is_nonlpcm;
}

static bool_t is_multi_channel(struct obj_t *p_obj)
{
	bool_t multi_chan;

	switch (p_obj->channel_status.data[0] &
			SII_CHANSTATUS_PATTERN__MASK) {
	case SII_CHANSTATUS_PATTERN__MULTI_LPCM:
		multi_chan = TRUE;
		break;
	default:
		multi_chan = FALSE;
		break;
	}

	SII_LOG1A("", p_obj, ("is Multi channel: %s\n",
			(multi_chan) ? "TRUE" : "FALSE"));
	if (s_is_hbra(p_obj)) {
		SII_LOG1A("", p_obj, ("9437 hbra\n"));
	} else {
		if (s_is_non_lpcm(p_obj)) {
			SII_LOG1A("", p_obj, ("9437 ac3\n"));
			multi_chan = FALSE;
		} else {
			SII_LOG1A("", p_obj, ("9437 lpcm\n"));
		}
	}

	return multi_chan;
}

static void s_update_samp_freq(struct obj_t *p_obj)
{
	uint8_t layout	= (p_obj->channel_status.data[5] &
				SII_CHANSTATUS_LAYOUT__MASK);
	uint8_t frame_rate;
	int16_t samp_freq;

	/* Frame rate is stored in bits 24-27, 30 & 31 */
	frame_rate = p_obj->channel_status.data[3] & 0xCF;
	samp_freq = (int16_t)s_get_freq(frame_rate);

	if (SII_RETURN_VALUE__FAIL == samp_freq) {
		p_obj->is_input_ch_status_valid = false;
		return;
	}

	SII_LOG2A("", p_obj, ("Input frame_rate:%x samp_freq:%u",
			frame_rate, samp_freq));

	if (is_multi_channel(p_obj)) {
		switch (layout) {
		case SII_CHANSTATUS_LAYOUT__2CH:
			samp_freq = (samp_freq >> SII_LAYOUT_2CH_SHIFT_OFFSET);
			break;
		case SII_CHANSTATUS_LAYOUT__8CH:
			SII_LOG2A("", p_obj, ("LAYOUT_8CH"));
			samp_freq = (samp_freq >> SII_LAYOUT_8CH_SHIFT_OFFSET);
			break;
		case SII_CHANSTATUS_LAYOUT__16CH:
			samp_freq = (samp_freq >> SII_LAYOUT_16CH_SHIFT_OFFSET);
			break;
		case SII_CHANSTATUS_LAYOUT__32CH:
			samp_freq = (samp_freq >> SII_LAYOUT_32CH_SHIFT_OFFSET);
			break;
		default:
			samp_freq = (samp_freq >> SII_LAYOUT_2CH_SHIFT_OFFSET);
			break;
		}
	} else if (s_is_non_lpcm(p_obj)) { /* Update layout for Non-LPCM */
		switch (layout) {
		case SII_CHANSTATUS_COMPR_LAYOUT__A:
			samp_freq = (samp_freq >> SII_LAYOUT_A_SHIFT_OFFSET);
			break;
		case SII_CHANSTATUS_COMPR_LAYOUT__B:
			samp_freq = (samp_freq >> SII_LAYOUT_B_SHIFT_OFFSET);
			break;
		default:
			samp_freq = (samp_freq >> SII_LAYOUT_A_SHIFT_OFFSET);
			break;
		}
	}
	frame_rate = s_get_code(samp_freq);
	p_obj->sample_rate = samp_freq * 100;
	SII_LOG2A("", p_obj, ("Output frame_rate:%x samp_freq:%u",
			frame_rate, samp_freq));

	/* Storing bits 24,25,26,27 into ch status byte 3 */
	p_obj->channel_status.data[3] = ((p_obj->channel_status.data[3] & ~0xCF)
			| (frame_rate & 0xCF));


	p_obj->channel_status_out.data[3] = p_obj->channel_status.data[3];
}

static void s_update_channel_status_pattern(struct obj_t *p_obj)
{
	uint8_t pattern;

	pattern = p_obj->channel_status.data[0] &
			SII_CHANSTATUS_PATTERN__MASK;

	p_obj->channel_status_out.data[0] &=
			~SII_CHANSTATUS_PATTERN__MASK;

	SII_LOG1A("", p_obj, ("Input Audio Info Pattern 0x%x\n", pattern));

	switch (pattern) {
	case SII_CHANSTATUS_PATTERN__NON_PCM:
		p_obj->channel_status_out.data[0] |=
				SII_CHANSTATUS_PATTERN__NON_PCM;
		break;
	case SII_CHANSTATUS_PATTERN__2CH_LPCM:
		p_obj->channel_status_out.data[0] |=
				SII_CHANSTATUS_PATTERN__2CH_LPCM;
		break;
	case SII_CHANSTATUS_PATTERN__MULTI_LPCM:
		p_obj->channel_status_out.data[0] |=
				SII_CHANSTATUS_PATTERN__MULTI_LPCM;
		break;
	default:
		p_obj->channel_status_out.data[0] |=
				SII_CHANSTATUS_PATTERN__NON_PCM;
		break;
	}

	SII_LOG1A("", p_obj, ("Output Audio Info Pattern = 0x%x\n",
			(p_obj->channel_status_out.data[0] &
			SII_CHANSTATUS_PATTERN__MASK)));
}

static void s_audio_wait_timer_start(struct obj_t *p_obj)
{
	sii_sys_seq_timer_start(p_obj->audio_wait_timer,
		SII_RX_AUDIO_WAIT_TIME, 0);
}

static void s_audio_update(struct obj_t *p_obj)
{
	bool_t multi_ch = FALSE;
	enum sii9437_extraction_mode mode = SII9437_EXTRACTION_MODE__I2S8;

	SII_LOG1A("", p_obj,
			("___s_audio_update function___\n"));

	if (sii_sys_seq_timer_running_is(p_obj->audio_wait_timer))
		sii_sys_seq_timer_stop(p_obj->audio_wait_timer);

	if (p_obj->b_standby_mode) {
		SII_LOG1A("", p_obj, ("Driver is in Standby mode\n"));
		return;
	}

	if (p_obj->arc_mode_status != SII_ARC_MODE__EARC)
		return;

	/* Mute output Audio */
	s_audio_mute(p_obj, TRUE);

	/* Disable output */
	s_output_io_enable(p_obj, FALSE);

	sii9437_hal_reset_aud_fifo(p_obj->dev_id);

	if (!(p_obj->is_input_ch_status_valid))
		return;

	if (p_obj->b_earc_connected_status &&
		p_obj->earc_lock_detected &&
		p_obj->earc_clock_detected)
		p_obj->audio_mute_disable = 1;
	else
		p_obj->audio_mute_disable = 0;

	/* Use the preferred extraction mode in the audio */
	if (p_obj->pref_extraction_mode ==
			SII9437_PREF_EXTRACTION_MODE__I2S)
		mode = SII9437_EXTRACTION_MODE__I2S8;
	else if (p_obj->pref_extraction_mode ==
			SII9437_PREF_EXTRACTION_MODE__SPDIF)
		mode = SII9437_EXTRACTION_MODE__SPDIF8;

	SII_MEMCPY(p_obj->channel_status_out.data,
			p_obj->channel_status.data,
			SII_CHANNEL_STATUS_SIZE);

	s_update_samp_freq(p_obj);

	if (!(p_obj->is_input_ch_status_valid))
		return;
	s_update_channel_status_pattern(p_obj);
	s_aif_extraction_get(p_obj, &p_obj->channel_status.data[0],
			&p_obj->audio_info);
	multi_ch = is_multi_channel(p_obj);
	s_print_layout(p_obj);
	SII_LOG1A("", p_obj,
		("Sampling Freq : %s\n",
		s_getting_freq_name(p_obj->channel_status.data[3] & 0xCF)));

	if (s_is_non_lpcm(p_obj))
		SII_LOG1A("", p_obj,
			("Compressed Audio layout : %s\n",
			(s_get_layout(p_obj) == SII_CHANSTATUS_COMPR_LAYOUT__A)
					? "A" : "B"));

	if (s_is_hbra(p_obj)) {
		mode = SII9437_EXTRACTION_MODE__I2S8;
		SII_LOG1A("", p_obj, ("HBRA Audio: Force extr mode to I2S8\n"));
	} else if (s_is_non_lpcm(p_obj) || !multi_ch || (multi_ch && s_get_layout(p_obj) ==
			SII_CHANSTATUS_LAYOUT__2CH)) {
		if (p_obj->pref_extraction_mode ==
				SII9437_PREF_EXTRACTION_MODE__I2S)
			mode = SII9437_EXTRACTION_MODE__I2S2;
		else if (p_obj->pref_extraction_mode ==
			SII9437_PREF_EXTRACTION_MODE__SPDIF)
			mode = SII9437_EXTRACTION_MODE__SPDIF2;
	}

	SII_LOG1A("", p_obj, ("audio mode : %d\n", mode));

	p_obj->extraction_mode_status = mode;

	if (p_obj->chip_version == 1)
		s_aud_mode_set(p_obj, mode);
	else
		s_aud_mode_rev0_set(p_obj, mode);

	if (s_is_non_lpcm(p_obj)) {
		bool_t b_ecc_on = FALSE;
		uint8_t stat_check_countdown = 200;
		uint8_t mask_value = 0;

		sii9437_hal_ecc_status(p_obj->dev_id, &b_ecc_on);

		sii9437_hal_ecc_mask_get(p_obj->dev_id, &mask_value);
		SII_LOG1A("", NULL, ("ECC mask value - 0x%02x\n", mask_value));

		if (!b_ecc_on) {

			/*For Non LPCM, enable ECC*/
			SII_LOG1A("", NULL, ("Enabling ECC\n"));

			sii9437_hal_ecc_enable(p_obj->dev_id, FALSE);
			sii9437_hal_ecc_mask_set(p_obj->dev_id);
			sii9437_hal_ecc_enable(p_obj->dev_id, TRUE);

			do {
				sii9437_hal_ecc_status(p_obj->dev_id,
						&b_ecc_on);
				/* Update countdown value*/
				stat_check_countdown--;
			} while (!b_ecc_on && stat_check_countdown);

			if (b_ecc_on) {
				SII_LOG1A("", NULL, ("ECC is turned ON\n"));
			} else {
				SII_LOG1A("", NULL,
				("ECC is NOT turned ON before timeout!\n"));
				return;
			}
		}
	} else {
		sii9437_hal_ecc_enable(p_obj->dev_id, FALSE);
	}

	if (p_obj->audio_mute_disable)
		s_audio_wait_timer_start(p_obj);
}

static uint32_t s_clk_lock_mode_set(struct obj_t *p_obj, uint8_t *ch_data)
{
	bool_t ckdt_mode_set =  TRUE;
	uint32_t earc_clk_freq;

	sii9437_hal_rx_earc_clk_freq_get(p_obj->dev_id, &earc_clk_freq);

	ckdt_mode_set = (earc_clk_freq > 65000000) ? TRUE : FALSE;

	sii9437_hal_clk_lock_mode_set(p_obj->dev_id, ckdt_mode_set);
	SII_LOG1A("", p_obj, ("Clock Detect Mode: %d\n", ckdt_mode_set));

	return SII_RETURN_VALUE__SUCCESS;
}
static uint32_t s_aud_mode_set(struct obj_t *p_obj,
		enum sii9437_extraction_mode audio_mode)
{
	bool_t spdif = FALSE;
	uint8_t clk_div = 0;
	uint8_t bsync_count = 0x41;
	uint8_t samp_freq  = p_obj->channel_status.data[3] & 0x0F;

	switch (audio_mode) {
	case SII9437_EXTRACTION_MODE__SPDIF2:
		spdif = TRUE;
		if ((s_get_layout(p_obj) != SII_CHANSTATUS_LAYOUT__2CH) ||
			(s_is_non_lpcm(p_obj) && (s_get_layout(p_obj) ==
				SII_CHANSTATUS_COMPR_LAYOUT__B))) {
			clk_div |=
				(BIT_MSK__RX_AUD_OUT_CH_CTRL__REG_OUT_DIV_BY4 |
				BIT_MSK__RX_AUD_OUT_CH_CTRL__REG_MULTI_CH_IN |
				BIT_MSK__RX_AUD_OUT_CH_CTRL__REG_DECI_EN);
		} else {
			clk_div |=
				(BIT_MSK__RX_AUD_OUT_CH_CTRL__REG_OUT_DIV_BY1);
		}
		SII_LOG1A("", p_obj, ("SPDIF2 Extraction: 0x%x\n", clk_div));
		break;
	case SII9437_EXTRACTION_MODE__SPDIF8:
		spdif = TRUE;
		if ((s_get_layout(p_obj) == SII_CHANSTATUS_LAYOUT__8CH) ||
				(s_is_non_lpcm(p_obj) && (s_get_layout(p_obj) ==
					SII_CHANSTATUS_COMPR_LAYOUT__B))) {
			if (p_obj->audio_info.speaker_alloc == 0)
				clk_div |=
				(BIT_MSK__RX_AUD_OUT_CH_CTRL__REG_OUT_DIV_BY4 |
				BIT_MSK__RX_AUD_OUT_CH_CTRL__REG_MULTI_CH_IN |
				BIT_MSK__RX_AUD_OUT_CH_CTRL__REG_DECI_EN);
			else
				clk_div |=
				(BIT_MSK__RX_AUD_OUT_CH_CTRL__REG_OUT_DIV_BY4 |
				BIT_MSK__RX_AUD_OUT_CH_CTRL__REG_MULTI_CH_OUT |
				BIT_MSK__RX_AUD_OUT_CH_CTRL__REG_MULTI_CH_IN);
		} else {
			clk_div |=
			(BIT_MSK__RX_AUD_OUT_CH_CTRL__REG_OUT_DIV_BY1);
			clk_div |=
				BIT_MSK__RX_AUD_OUT_CH_CTRL__REG_MULTI_CH_OUT;
		}
		SII_LOG1A("", p_obj, ("SPDIF8 Extraction: 0x%x\n", clk_div));
		break;
	case SII9437_EXTRACTION_MODE__I2S2:
		if ((is_multi_channel(p_obj)
			&& (s_get_layout(p_obj) ==
					SII_CHANSTATUS_LAYOUT__8CH)) ||
				(s_is_non_lpcm(p_obj) && (s_get_layout(p_obj) ==
				SII_CHANSTATUS_COMPR_LAYOUT__B))) {
			clk_div |=
				(BIT_MSK__RX_AUD_OUT_CH_CTRL__REG_OUT_DIV_BY4 |
				BIT_MSK__RX_AUD_OUT_CH_CTRL__REG_MULTI_CH_IN |
				BIT_MSK__RX_AUD_OUT_CH_CTRL__REG_DECI_EN);
		} else {
			clk_div |=
			(BIT_MSK__RX_AUD_OUT_CH_CTRL__REG_OUT_DIV_BY1);
		}
		SII_LOG1A("", p_obj, ("I2S2 Extraction: 0x%x\n", clk_div));
		break;
	case SII9437_EXTRACTION_MODE__I2S8:
		if ((is_multi_channel(p_obj) &&
			(s_get_layout(p_obj) == SII_CHANSTATUS_LAYOUT__8CH)) ||
			s_is_hbra(p_obj) ||
			(s_is_non_lpcm(p_obj) && (s_get_layout(p_obj) ==
				SII_CHANSTATUS_COMPR_LAYOUT__B))) {
			clk_div |=
			(BIT_MSK__RX_AUD_OUT_CH_CTRL__REG_OUT_DIV_BY4 |
			BIT_MSK__RX_AUD_OUT_CH_CTRL__REG_MULTI_CH_IN);
		} else {
			clk_div |=
				(BIT_MSK__RX_AUD_OUT_CH_CTRL__REG_OUT_DIV_BY1);
		}
		SII_LOG1A("", p_obj, ("I2S8 Extraction: 0x%x\n", clk_div));
		break;
	case SII9437_EXTRACTION_MODE__NONE:
		break;
	}

	if (s_get_layout(p_obj) == SII_CHANSTATUS_LAYOUT__8CH) {
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
			bsync_count = 0x43;
			break;
		case SII_FREQ_192_KHZ:
		case SII_FREQ_176_4_KHZ:
		case SII_FREQ_128_KHZ:
			bsync_count = 0x41;
			break;
		case SII_FREQ_96_KHZ:
		case SII_FREQ_88_2_KHZ:
		case SII_FREQ_64_KHZ:
			bsync_count = 0x21;
			break;
		case SII_FREQ_32_KHZ:
		case SII_FREQ_48_KHZ:
		case SII_FREQ_44_1_KHZ:
			bsync_count = 0x11;
			break;
		default:
			break;
		}
	} else {
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
			bsync_count = 0x43;
			break;
		case SII_FREQ_192_KHZ:
		case SII_FREQ_176_4_KHZ:
		case SII_FREQ_128_KHZ:
			bsync_count = 0x13;
			break;
		case SII_FREQ_96_KHZ:
		case SII_FREQ_88_2_KHZ:
		case SII_FREQ_64_KHZ:
			bsync_count = 0x0B;
			break;
		case SII_FREQ_48_KHZ:
		case SII_FREQ_44_1_KHZ:
		case SII_FREQ_32_KHZ:
			bsync_count = 0x07;
			break;
		default:
			break;
		}
	}

	sii9437_hal_aud_mode_set(p_obj->dev_id, spdif);
	sii9437_hal_clk_div_set(p_obj->dev_id, clk_div);
	sii9437_hal_bsync_count_set(p_obj->dev_id, bsync_count);

	return SII_RETURN_VALUE__SUCCESS;
}

static uint32_t s_aud_mode_rev0_set(struct obj_t *p_obj,
		enum sii9437_extraction_mode audio_mode)
{
	bool_t spdif = FALSE;
	uint8_t clk_div = 0;
	bool_t multi_chan = TRUE;
	bool_t is_multi_ch = is_multi_channel(p_obj);

	switch (audio_mode) {
	case SII9437_EXTRACTION_MODE__SPDIF2:
	case SII9437_EXTRACTION_MODE__SPDIF8:
		spdif = TRUE;

		if (is_multi_ch) {
			clk_div |= 0x23;
			multi_chan = TRUE;
		} else {
			clk_div |= 0x04;
			multi_chan = FALSE;
		}
		break;
	case SII9437_EXTRACTION_MODE__I2S8:
	case SII9437_EXTRACTION_MODE__I2S2:
		spdif = FALSE;
		if (is_multi_ch) {
			clk_div = 0x25;
			multi_chan = TRUE;
		} else {
			clk_div = 0x04;
			multi_chan = FALSE;
		}

		break;
	case SII9437_EXTRACTION_MODE__NONE:
		break;
	}

	sii9437_hal_aud_mode_set(p_obj->dev_id, spdif);
	sii9437_hal_clk_div_set(p_obj->dev_id, clk_div);
	sii9437_hal_bsync_count_set(p_obj->dev_id,
			multi_chan ? 0x41 : 0x07);

	return SII_RETURN_VALUE__SUCCESS;
}

static void s_audio_mute(struct obj_t *p_obj, bool_t b_on)
{
	/* Mute/unmute audio internally, only if the mute is not enabled from
	 * mute_enable_set() API */
	if (p_obj->mute_state != b_on) {
		p_obj->mute_state = b_on;

		if (!p_obj->b_audio_mute_user_set)
			sii9437_hal_aud_mute_set(p_obj->dev_id, b_on);
	}
}

static void s_output_io_enable(struct obj_t *p_obj, bool_t b_en)
{
	uint8_t io_value;
	if (p_obj->b_ouput_io_en_state != b_en) {
		p_obj->b_ouput_io_en_state = b_en;

		if (b_en)
			io_value = (p_obj->arc_mode_status ==
				SII_ARC_MODE__ARC) ? 0xBF :
				((p_obj->extraction_mode_status ==
					SII9437_EXTRACTION_MODE__SPDIF2) ||
				(p_obj->extraction_mode_status ==
					SII9437_EXTRACTION_MODE__SPDIF8)) ?
						0x3E : 0x40;
		else
			io_value = 0xFF;

		sii9437_hal_ouput_io_enable(p_obj->dev_id, io_value);
	}
}

static void s_standby_set(struct obj_t *p_obj, bool_t *p_on)
{
	/* If the new standby mode setting is same as the current mode,
	 * skip processing and return */
	if (p_obj->b_standby_mode == *p_on) {
		SII_LOG1A("", p_obj,
			("No change in standby mode:  %s\n",
			p_obj->b_standby_mode ? "ON" : "OFF"));
		return;
	}

	p_obj->b_standby_mode = *p_on;

	if (*p_on) {
		/* Disable interrupt mask */
		sii9437_hal_int_mask_enable(p_obj->dev_id, FALSE);

		/* Mute output Audio */
		s_audio_mute(p_obj, TRUE);

		/* Disable OEN */
		s_output_io_enable(p_obj, FALSE);
	} else {
		/* Enable interrupt mask */
		sii9437_hal_int_mask_enable(p_obj->dev_id, TRUE);

		s_config_update(p_obj);

		s_audio_update(p_obj);
	}

	SII_LOG1A("", NULL, ("Standby mode is %s\n",
		(p_obj->b_standby_mode == TRUE) ? "enabled" : "disabled"));
}

static void s_user_notification_handler(struct obj_t *p_obj,
			uint32_t evt_flgs_notify)
{
	/* All unmasked events which are not notified to the callback
	 * function, will be added to event_flags_status. Do OR to prevent
	 * overriding existing flag which is uncleared yet by calling
	 * event_flags_status_clear API */
	p_obj->event_flags_status |= (evt_flgs_notify &
			~p_obj->event_flags_mask);

	/* Notify only events which are enabled in the mask */
	evt_flgs_notify &= p_obj->event_flags_mask;

	if (evt_flgs_notify) {
		if (p_obj->event_cb_func)
			p_obj->event_cb_func(&evt_flgs_notify);
	}
}

static void s_print_mem_usage(struct obj_t *p_obj, const char *log_str)
{
	SII_LOG1A("", p_obj, ("%s - Memory allocated in bytes: %d\n",
			log_str, sii_sys_malloc_bytes_allocated_get()));
}

static void s_status_flags_reset(struct obj_t *p_obj)
{
	p_obj->ch_status_rcvd = FALSE;
	p_obj->is_input_ch_status_valid = FALSE;
	p_obj->b_earc_connected_status = FALSE;
	p_obj->earc_clock_detected = FALSE;
}


static void s_arc_mode_update(struct obj_t *p_obj,
		enum sii_arc_mode arc_mode_status)
{
	uint32_t evt_flgs_notify = 0;

	switch (arc_mode_status) {

	case SII_ARC_MODE__ARC:

		SII_LOG1A("", p_obj,
			("ARC Mode: Legacy ARC\n"));

		/* Mute output Audio */
		s_audio_mute(p_obj, TRUE);

		/* Disable output */
		s_output_io_enable(p_obj, FALSE);

		s_hpd_update(p_obj, FALSE, p_obj->b_hpd_in,
					&evt_flgs_notify);
		s_status_flags_reset(p_obj);

		/* Disable All interrupts */
		sii9437_hal_int_mask_enable(p_obj->dev_id, FALSE);

		/* Enable only HPD interrupt */
		sii9437_hal_hpd_int_mask_enable(p_obj->dev_id, TRUE);

		/* Enable SM state change interrupts */
		sii9437_hal_sm_states_intr_enable(p_obj->dev_id);

		/* Change the ARC mode */
		sii9437_hal_arc_mode_set(p_obj->dev_id, arc_mode_status);

		/* Enable SPDIF output */
		s_output_io_enable(p_obj, TRUE);

		/* UnMute output Audio */
		s_audio_mute(p_obj, FALSE);

		p_obj->arc_mode_configured =  SII_ARC_MODE__ARC;

		break;

	case SII_ARC_MODE__EARC:

		SII_LOG1A("", p_obj, ("ARC Mode: eARC\n"));

		/* Mute output Audio */
		s_audio_mute(p_obj, TRUE);

		/* Disable output */
		s_output_io_enable(p_obj, FALSE);

		/*Set cap change bit before eARC is enabled*/
		sii9437_hal_cap_chng_bit_set(p_obj->dev_id, TRUE);

		/* Change the ARC mode */
		sii9437_hal_arc_mode_set(p_obj->dev_id, arc_mode_status);

		/* Enable interrupt mask */
		sii9437_hal_int_mask_enable(p_obj->dev_id, TRUE);

		s_audio_update(p_obj);

		p_obj->arc_mode_configured =  SII_ARC_MODE__EARC;

		break;

	case SII_ARC_MODE__NONE:

		SII_LOG1A("", p_obj, ("ARC Mode: NONE\n"));

		/* Mute output Audio */
		s_audio_mute(p_obj, TRUE);

		/* Disable output */
		s_output_io_enable(p_obj, FALSE);

		s_hpd_update(p_obj, FALSE, p_obj->b_hpd_in, &evt_flgs_notify);

		s_status_flags_reset(p_obj);

		/* Disable interrupt mask */
		sii9437_hal_int_mask_enable(p_obj->dev_id, FALSE);

		/* Enable HPD interrupt mask */
		sii9437_hal_hpd_int_mask_enable(p_obj->dev_id, TRUE);

		/* Enable SM state change interrupts */
		sii9437_hal_sm_states_intr_enable(p_obj->dev_id);

		/* Change the ARC mode */
		sii9437_hal_arc_mode_set(p_obj->dev_id, arc_mode_status);

		/* UnMute output Audio */
		/*s_audio_mute(p_obj, FALSE);*/

		p_obj->arc_mode_configured =  SII_ARC_MODE__NONE;

		break;

	default:

		break;
	}

	if (evt_flgs_notify) {
		SII_LOG1A("", p_obj,
			("HPD change event notification - 0x%02x\n",
					evt_flgs_notify));
		s_user_notification_handler(p_obj, evt_flgs_notify);
	}
}

static void s_vhpd_read(struct obj_t *p_obj, uint32_t *evt_flgs_notify)
{
	bool_t b_vhpd;
	sii9437_hal_hdmi_hpd_query(p_obj->dev_id, &b_vhpd);

	SII_LOG1A("", p_obj, ("VHPD status : %s\n",
		b_vhpd ? "hi" : "low"));

	/* Virtual HPD status is to be read only after eARC connection is
	 * established */
	if (p_obj->b_earc_connected_status)
		s_hpd_set(p_obj, b_vhpd, evt_flgs_notify);
	else
		SII_LOG1A("", p_obj,
			("eARC is OFF, ignoring VHPD Status change : %s\n",
			b_vhpd ? "hi" : "low"));
}

static void s_hpd_update(struct obj_t *p_obj,
		bool_t b_earc_conn_stat, bool_t b_hpd_in,
		uint32_t *evt_flgs_notify)
{
	bool_t b_hpd_out =  b_hpd_in;

	/* If there is no change in arc mode and in HPD there wont
	 * be any change in the out hpd */
	if ((b_earc_conn_stat == p_obj->b_earc_connected_status)
			&& (b_hpd_in == p_obj->b_hpd_in)) {

		SII_LOG1A("", NULL,
			("No change in eARC status or RX_HPD_IN status\n"));
		SII_LOG1A("", NULL,
			("No update for RX_HPD_OUT - %s\n",
				p_obj->b_hpd_out ? "hi" : "low"));
		return;
	}

	SII_LOG1A("", p_obj, ("Current eARC status - %s",
			p_obj->b_earc_connected_status ? "ON" : "OFF"));
	SII_LOG1A("", p_obj, ("New eARC status - %s",
			b_earc_conn_stat ? "ON" : "OFF"));

	SII_LOG1A("", p_obj, ("Current HPD IN status - %s",
				p_obj->b_hpd_in ? "hi" : "low"));
	SII_LOG1A("", p_obj, ("New HPD IN status - %s",
			b_hpd_in ? "hi" : "lo"));

	SII_LOG1A("", p_obj, ("Current RX HPD OUT - %s",
			p_obj->b_hpd_out ? "hi" : "low"));

	/* If eARC is ON */
	if (b_earc_conn_stat) {
		/* If HPD-IN change from H to L - may be cable disconnect
		 * then propagate input HPD */
		if (!b_hpd_in && p_obj->b_hpd_in) {
			SII_LOG1A("", p_obj,
			("RX HPD OUT to be changed (RX HPD IN(hi-lo)) - %s",
				b_hpd_out ? "hi" : "low"));
			SII_LOG1A("", p_obj, ("Forcing state to IDLE1\n"));
			sii9437_hal_sm_state_set(p_obj->dev_id,
					SII9437_SM_STATE__IDLE1);
			sii9437_hal_sm_state_override_enable(p_obj->dev_id,
					TRUE);
			sii9437_hal_sm_state_override_enable(p_obj->dev_id,
					FALSE);
		} else if ((b_hpd_in && !p_obj->b_hpd_in) ||
				(!p_obj->b_hpd_in)) {
			/* In Illegal conditions:
			 * while eARC is ON
			 *	HPD-IN change from L to H
			 *	HPD-IN in L
			 * Send low to HDP-OUT */
			b_hpd_out = FALSE;
			SII_LOG1A("", p_obj,
			("RX HPD OUT to be changed (RX HPD IN lo/lo-hi) - %s",
				b_hpd_out ? "hi" : "low"));
		} else if (b_hpd_in) {
			/* If eARC is ON and HPD-IN is High, HDMI-HPD
			 * changes from heartbeat are sent to HPD-OUT */
			bool_t b_vhpd;
			sii9437_hal_hdmi_hpd_query(p_obj->dev_id,
					&b_vhpd);

			b_hpd_out = b_vhpd;
			SII_LOG1A("", p_obj,
			("RX HPD OUT to be changed (From VPHD) - %s",
				b_hpd_out ? "hi" : "low"));
		}
	}
	/* else */
	/* If eARC is OFF then propagate input HPD to application
	 * if there is any change in it or if RX_HPD_OUT is not in
	 * sync with RX_HPD_IN */

	s_hpd_set(p_obj, b_hpd_out, evt_flgs_notify);

	SII_LOG1A("", p_obj, ("RX HPD OUT - %s",
			p_obj->b_hpd_out ? "hi" : "low"));
}

static void s_hpd_set(struct obj_t *p_obj, bool_t b_hpd,
		uint32_t *evt_flgs_notify)
{
	p_obj->b_hpd_pending = b_hpd;

	if (!sii_sys_seq_timer_running_is(p_obj->hpd_timer)) {

		if (p_obj->b_hpd_out == b_hpd)
			return;

		p_obj->b_hpd_out = b_hpd;
		sii9437_hal_hpd_set(p_obj->dev_id, p_obj->b_hpd_out);
		SII_LOG1A("", NULL,
			("HPD Set to: %s\n", p_obj->b_hpd_out ? "hi" : "low"));

		if (evt_flgs_notify)
			*evt_flgs_notify |= SII9437_EVENT_FLAGS__HPD_CHNG;

		/* If HPD is set to low, start timer to maintain min low */
		if (p_obj->b_hpd_out == FALSE)
			s_hpd_timer_start(p_obj);
	}
}

static void s_earc_hpd_set(struct obj_t *p_obj, bool_t b_earc_hpd)
{
	if (p_obj->arc_mode_status != SII_ARC_MODE__EARC)
		return;

	if (p_obj->b_earc_hpd == b_earc_hpd)
		return;

	p_obj->b_earc_hpd_pending = b_earc_hpd;

	if (!sii_sys_seq_timer_running_is(p_obj->earc_hpd_timer)) {
		p_obj->b_earc_hpd = b_earc_hpd;
		sii9437_hal_earc_hpd_set(p_obj->dev_id, p_obj->b_earc_hpd);
		SII_LOG1A("", NULL,
			("eARC HPD Set to: %s\n", b_earc_hpd ? "hi" : "low"));

		/* If eARC HPD is set to low, start timer to maintain min low */
		if (p_obj->b_earc_hpd == FALSE)
			s_earc_hpd_timer_start(p_obj);
	}
}

static void s_earc_hpd_timer_start(struct obj_t *p_obj)
{
	/* eARC HPD timer should run for toggle time + T_heartbeat_max */
	sii_sys_seq_timer_start(p_obj->earc_hpd_timer,
		p_obj->earc_hpd_toggle_time + SII_HEARTBEAT_TIME_MAX, 0);
}

static void s_hpd_timer_start(struct obj_t *p_obj)
{
	sii_sys_seq_timer_start(p_obj->hpd_timer, SII_HPD_LOW_MIN, 0);
}

static void s_config_update(struct obj_t *p_obj)
{
	s_gpio_config_update(p_obj);
	s_gpio_event_mask_set(p_obj);
	sii9437_hal_i2s_mclk_mode_set(p_obj->dev_id, p_obj->mclock_mode);
}

static uint32_t s_get_layout(struct obj_t *p_obj)
{
	return p_obj->channel_status.data[5] & SII_CHANSTATUS_LAYOUT__MASK;
}

static void s_print_layout(struct obj_t *p_obj)
{
	char *layout_str = "undefined";
	switch (s_get_layout(p_obj)) {
	case SII_CHANSTATUS_LAYOUT__2CH:
		layout_str = "2 ch";
		break;
	case SII_CHANSTATUS_LAYOUT__8CH:
		layout_str = "8 ch";
		break;
	case SII_CHANSTATUS_LAYOUT__16CH:
		layout_str = "16 ch";
		break;
	case SII_CHANSTATUS_LAYOUT__32CH:
		layout_str = "32 ch";
		break;
	default:
		break;
	}

	SII_LOG1A("", p_obj, ("Audio Layout : %s\n", layout_str));
}

/***** end of file **********************************************************/

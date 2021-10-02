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
 * @file  sii9437_drv.h
 *
 * @brief Defines Sii9437 Receiver Register Info.
 *
 ******************************************************************************/
#ifndef _SII9437_DRV_H_
#define _SII9437_DRV_H_

#include "sii_datatypes.h"
#include "sii9437_api.h"

#define REG_TOP__INT_NUM			3
#define REG_INTR_TOP_1				0
#define REG_INTR_TOP_2				1
#define REG_INTR_TOP_3				2

#define REG_RX__INT_NUM				9
#define OWN_BKSV_NUM				5

#define REG_INTR_0				0
#define REG_INTR_1				1
#define REG_INTR_2				2
#define REG_INTR_3				3
#define REG_INTR_4				4
#define REG_INTR_5				5
#define REG_INTR_6				6
#define REG_INTR_7				7
#define REG_INTR_8				8

#define BIT__GPIO_NUM_0				0x01
#define BIT__GPIO_NUM_1				0x02
#define BIT__GPIO_NUM_2				0x04
#define BIT__GPIO_NUM_3				0x08
#define BIT__GPIO_NUM_4				0x10
#define BIT__GPIO_NUM_5				0x20

enum sii9437_freq_zone {
	SII_FREQ_ZONE_1X,
	SII_FREQ_ZONE_2X,
	SII_FREQ_ZONE_4X,
	SII_FREQ_ZONE_8X,
};

uint32_t sii9437_hal_rx_init_stage_1(uint32_t dev_id,
				const struct sii9437_config *p_config);
uint32_t sii9437_hal_rx_init_stage_2(uint32_t dev_id);

uint32_t sii9437_hal_gpio_query(uint32_t dev_id, uint16_t *p_gpio_pin_mask);

uint32_t sii9437_hal_gpio_output_set(uint32_t dev_id,
		uint16_t *p_gpio_pin_mask);

uint32_t sii9437_hal_gpio_config(uint32_t dev_id,
		uint16_t *gpio_pin_io_config);

uint32_t sii9437_hal_gpio_output_update(uint32_t dev_id,
		uint16_t *p_gpio_pin_mask, bool_t b_set);

uint32_t sii9437_hal_chip_reset(uint32_t dev_id);
uint32_t sii9437_hal_earc_hpd_set(uint32_t dev_id, bool_t b_en);
uint32_t sii9437_hal_hpd_set(uint32_t dev_id, bool_t b_en);
uint32_t sii9437_hal_caps_ds_set(uint32_t dev_id, uint8_t *p_caps_ds,
				uint16_t size);
uint32_t sii9437_hal_channel_status_query(uint32_t dev_id,
				uint8_t *p_data,
				uint16_t size);
uint32_t sii9437_hal_aud_mute_set(uint32_t dev_id, bool_t en);
uint32_t sii9437_hal_conn_status_get(uint32_t dev_id,
					bool_t *p_conn);
uint32_t sii9437_hal_top_int_get(uint32_t dev_id,
					uint8_t *p_top_int);
uint32_t sii9437_hal_enable_all_int(uint32_t dev_id,
					bool_t en);
uint32_t sii9437_hal_earc_block_int_get(uint32_t dev_id,
					uint8_t *p_int);
uint32_t sii9437_hal_gpio_event_mask_set(uint32_t dev_id,
		uint8_t *gpio_pin_event_mask);
uint32_t sii9437_hal_earc_int_status_get(uint32_t dev_id,
				 uint8_t *p_int_regs,
				 uint16_t num_regs);
uint32_t sii9437_hal_top_int_status_get(uint32_t dev_id,
				 uint8_t *p_int_top_regs,
				 uint16_t num_regs);
uint32_t sii9437_hal_is_heartbeat_lost(uint32_t dev_id,
				 bool_t *p_status);
uint32_t sii9437_hal_tx_data_get(uint32_t dev_id,
					uint8_t *p_data);

uint32_t sii9437_hal_hdmi_hpd_query(uint32_t dev_id, bool_t *hpd_status);

uint32_t sii9437_hal_hpd_init(uint32_t dev_id, bool_t *b_hpd_in_value);

uint32_t sii9437_hal_aud_mode_set(uint32_t dev_id, bool_t is_spdif_mode);
uint32_t sii9437_hal_rx_softreset(uint32_t dev_id);
uint32_t sii9437_hal_reset_aud_fifo(uint32_t dev_id);
uint32_t sii9437_hal_arc_mode_set(uint32_t dev_id,
				enum sii_arc_mode arc_mode);
uint32_t sii9437_hal_i2s_mclk_mode_set(uint32_t dev_id,
		enum sii9437_mclk_mode mclk_mode);
uint32_t sii9437_hal_int_mask_enable(uint32_t dev_id, bool_t en);
uint32_t sii9437_hal_earc_lock_status_query(uint32_t dev_id,
				bool_t *lock_status);
uint32_t sii9437_hal_earc_clock_status_query(uint32_t dev_id,
				bool_t *p_clock_status);
uint32_t sii9437_hal_output_channel_status_set(uint32_t dev_id,
				uint8_t *pchannel_status_out,
				uint8_t *pchannel_status_extra);

uint32_t sii9437_hal_hpd_int_mask_enable(uint32_t dev_id, bool_t b_en);

uint32_t sii9437_hal_ouput_io_enable(uint32_t dev_id,
		uint8_t value);
uint32_t sii9437_hal_timer_value_set(uint32_t dev_id, uint16_t time_value_ms);
uint32_t sii9437_hal_timer_value_get(uint32_t dev_id, uint16_t *time_value_ms);
uint32_t sii9437_hal_chip_version_query(uint32_t dev_id,
					uint8_t *p_chip_version);
uint32_t sii9437_hal_bsync_count_set(uint32_t dev_id,
					uint8_t bsync_count);
uint32_t sii9437_hal_clk_div_set(uint32_t dev_id,
		uint8_t clk_div);
uint32_t sii9437_hal_rx_cs_mute_mask_pattern_set(uint32_t dev_id);
uint32_t sii9437_hal_idle2_enable(uint32_t dev_id, bool_t b_en);
uint32_t sii9437_hal_sm_state_get(uint32_t dev_id, uint8_t *p_state_val);
uint32_t sii9437_hal_sm_state_set(uint32_t dev_id, uint8_t state_val);
uint32_t sii9437_hal_sm_state_override_enable(uint32_t dev_id, bool_t b_en);
uint32_t sii9437_hal_arc_init_term(uint32_t dev_id, bool_t b_init_is);
uint32_t sii9437_hal_sm_states_intr_enable(uint32_t dev_id);
uint32_t sii9437_hal_erx_latency_set(uint32_t dev_id,
					uint8_t erx_latency);
uint32_t sii9437_hal_erx_latency_req_query(uint32_t dev_id,
					uint8_t *erx_latency_req);
uint32_t sii9437_hal_stat_chng_bit_set(uint32_t dev_id,
					bool_t b_stat_chng_set);

uint32_t sii9437_hal_ecc_mask_init(uint32_t dev_id);
uint32_t sii9437_hal_ecc_mask_set(uint32_t dev_id);
uint32_t sii9437_hal_ecc_mask_get(uint32_t dev_id, uint8_t *ecc_mask);
uint32_t sii9437_hal_ecc_enable(uint32_t dev_id, bool_t b_en);
uint32_t sii9437_hal_ecc_status(uint32_t dev_id, bool_t *p_b_on);
uint32_t sii9437_hal_ecc_err_cnt_get(uint32_t dev_id, uint16_t *p_err_cnt);
uint32_t sii9437_hal_ecc_err_cnt_clear(uint32_t dev_id);
uint32_t sii9437_hal_ecc_err_fixed_cnt_get(uint32_t dev_id,
		uint16_t *p_err_fixed_cnt);
uint32_t sii9437_hal_ecc_err_fixed_cnt_clear(uint32_t dev_id);

uint32_t sii9437_hal_cap_chng_bit_set(uint32_t dev_id, bool_t b_set);
uint32_t sii9437_hal_tx_cap_chng_conf_query(uint32_t dev_id,
					bool_t *p_b_cap_chng_conf);
uint32_t sii9437_hal_tx_stat_chng_conf_query(uint32_t dev_id,
					bool_t *p_b_stat_chng_conf);
uint32_t sii9437_hal_clk_lock_mode_set(uint32_t dev_id,
					bool_t is_ckdt_mode);
uint32_t sii9437_hal_earc_only_intr_en(uint32_t dev_id, bool_t  enable);
uint32_t sii_hal_freq_zone_set(uint32_t dev_id,
			enum  sii9437_freq_zone ins_mode);
uint32_t sii9437_hal_earc_lock_chg_intr_en(uint32_t dev_id, bool_t  enable);
uint32_t sii9437_hal_tx_pll_enable(uint32_t dev_id, bool_t pll_enable);
uint32_t sii9437_hal_rx_earc_clk_freq_get(uint32_t dev_id,
		uint32_t *earc_freq);
uint32_t sii9437_hal_rx_comma_enable(uint32_t dev_id, bool_t b_enable);
uint32_t sii9437_hal_tx_hb_status_clear(uint32_t dev_id);
uint32_t sii9437_hal_rx_lock_toggle(uint32_t dev_id);

#endif /* _SII9437_DRV_H_ */

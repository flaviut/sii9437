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
 * @file sii9437.c
 *
 * @brief Driver for sii9437 (eARC/ARC receiver)
 *
 *****************************************************************************/
 #define SII_DEBUG	3

/***** #include statements ***************************************************/
/* #include "ls_dbglog_api.h" */
#include "sii9437_drv.h"
#include "sii9437_api.h"
#include "sii_sys_log_api.h"
#include "sii_drv_cra_api.h"
#include "sii9437_regs.h"
#include "sii_system_api.h"
#include "sii_sys_time_api.h"

/***** Register Module name **************************************************/
SII_MODULE_NAME_SET(9437RxHal);


/***** local macro definitions ***********************************************/
/* Timer scale and counts used for 1ms resolution */
/* Timer scale needed as per formula:
 * Required time = 50ns * (timer_count + 1) * (timer_scale + 1)
 * For 1ms timer: (timer_count + 1) * (timer_scale + 1) = 20000
 *	for which timer scale can be 9999 = 0x270F
 *	and timer count (2 - 1) = 1
 * Timer count ref is taken as 2 so that for n ms timer the timer count
 * will be (n*2 - 1)
 */
#define	SII943X_TIMER_SCALE		0x270F
#define	SII943X_TIMER_COUNT_VALUE_REF	0x0002

#define SII943X_ECC_MASK		0x3A
#define SII943X_ECC_PATTERN		0x02

/* 20MHz/rx_freq_win
 * rx_freq_win = 128 default */
#define EARC_CLOCK_FREQ_SCALE_FACTOR	156250

/***** local type definitions ************************************************/


/***** local prototypes ******************************************************/


/***** local data objects ****************************************************/


/***** call-back functions ***************************************************/


/***** public functions ******************************************************/

uint32_t sii9437_hal_rx_init_stage_1(uint32_t dev_id,
		const struct sii9437_config *p_config)
{
	/* override i2c address */
	/* Override Unused address*/
	sii_drv_cra_write8(dev_id, REG_ADDR__I2C_ADDR0, 0x00);
	/* Override Unused address*/
	sii_drv_cra_write8(dev_id, REG_ADDR__I2C_ADDR1, 0x00);
	/* Override Unused address*/
	sii_drv_cra_write8(dev_id, REG_ADDR__I2C_ADDR2, 0x00);
	/* Override Rx EARC address*/
	sii_drv_cra_write8(dev_id, REG_ADDR__I2C_ADDR3, p_config->i2c_addr_1);
	/* Override Phy address*/
	sii_drv_cra_write8(dev_id, REG_ADDR__I2C_ADDR4, p_config->i2c_addr_2);
	/* Expose all i2c */
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__I2C_CONFIG,
				BIT_MSK__I2C_CONFIG__REG_EXPOSE_SLAVE_ADDR,
				TRUE);

	/* Disable test mode */
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__TEST_CTRL,
				BIT_MSK__TEST_CTRL__REG_TEST_EN,
				FALSE);

	/* Enable timer in one shot mode */
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__TIMER_CTRL,
			BIT_MASK__TIMER_CTRL__REG_TIMER_CONFIG_ENABLE,
			TRUE);
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__TIMER_CTRL,
			BIT_MASK__TIMER_CTRL__REG_TIMER_CONFIG_MODE,
			FALSE);

	/* Timer scale */
	sii_drv_cra_write16(dev_id, REG_ADDR__TIMER_SCALE1,
			SII943X_TIMER_SCALE);

	/* Enable timer interrupt */
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__TOP_INTR1_MASK,
			BIT_MSK__TOP_INTR1__REG_TOP_INTR_STAT1, TRUE);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_rx_init_stage_2(uint32_t dev_id)
{
#if SII_CHECK_VALID_BKSV
	uint8_t rx_bksv[5];
	uint8_t i, j, count_ones = 0;
#endif
	uint8_t chip_id = 0;

	/* Change CMC Tx to CMC Rx */
	sii_drv_cra_write8(dev_id, REG_ADDR__CHIP_ID_I2C, 0x89);

	/* send 0 to spdif pad */
	sii_drv_cra_write8(dev_id, REG_ADDR__SPDIF_SEL,
			BIT_MSK__SPDIF_SEL__REG_SPDIF_I_SEL);
	/* Disable test mode */
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__TEST_CTRL,
				BIT_MSK__TEST_CTRL__REG_TEST_EN,
				FALSE);
	/* disable test mode */
	sii_drv_cra_write8(dev_id, REG_ADDR__RX_I2S_CTRL1, 0xC0);
	/* i2s output enable */
	sii_drv_cra_write8(dev_id, REG_ADDR__RX_I2S_CTRL2, 0xF0);
	/* i2s output enable */
	sii_drv_cra_write8(dev_id, REG_ADDR__RX_AUDRX_CTRL, 0x21);

	/* Debug registers.
	 * Should be programmed to 0x00. */
	sii_drv_cra_write8(dev_id, REG_ADDR__RX_CS_DEBUG1, 0x00);
	sii_drv_cra_write8(dev_id, REG_ADDR__RX_CS_DEBUG2, 0x00);
	sii_drv_cra_write8(dev_id, REG_ADDR__RX_CS_DEBUG3, 0x00);

	/* Enable IDLE2 state */
	sii9437_hal_idle2_enable(dev_id, TRUE);

	/* Enable mute control */
	sii_drv_cra_write8(dev_id, REG_ADDR__MUTE_EXP_EN_1,
			(BIT_MSK__MUTE_EXP_EN_1__REG_MUTE_EN |
			BIT_MSK__MUTE_EXP_EN_1__MUTE_EN_ON_CS_BIT));
	/* zone overwide */
	sii_drv_cra_write8(dev_id, REG_ADDR__VIOLA_VCO_CAL, 0x83);

	chip_id = sii_drv_cra_read8(dev_id,
			REG_ADDR__DEV_IDL);

	sii_drv_cra_write8(dev_id, REG_ADDR__EARC_CFG_1,
			(BIT_MSK__EARC_CFG1__CM_LEVEL_CTRL_350MV |
			BIT_MSK__EARC_CFG1__ARC_MODE_CTRL_CM));

	if (chip_id == 0x39) {
		/**p_chip_version = 1;*/
		sii_drv_cra_write8(dev_id, REG_ADDR__VIOLA_SLEW_N_SWING, 0x52);
		sii_drv_cra_write8(dev_id, REG_ADDR__VIOLA_SPRX, 0x23);
	} else if (chip_id == 0x37) {
		/**p_chip_version = 0;*/
		sii_drv_cra_write8(dev_id, REG_ADDR__VIOLA_SLEW_N_SWING, 0x56);
		sii_drv_cra_write8(dev_id, REG_ADDR__VIOLA_SPRX, 0xA2);
	}

	sii_drv_cra_write8(dev_id, REG_ADDR__VIOLA_CM_BW_CTL, 0x08);

	sii_drv_cra_write8(dev_id, REG_ADDR__VIOLA_RX_CFG, 0x18);
	/* enable earc mode */
	sii_drv_cra_write8(dev_id, REG_ADDR__VIOLA_MODE,
			(BIT_MSK__VIOLA_MODE__REG_AUTO_CFG |
			BIT_MSK__VIOLA_MODE__REG_LOCK |
			BIT_MSK__VIOLA_MODE__REG_LOCK_OVR |
			BIT_MSK__VIOLA_MODE__REG_ARC_MODE |
			BIT_MSK__VIOLA_MODE__REG_ARC_DIR |
			BIT_MSK__VIOLA_MODE__PLL_EN));

	/* mclk io ctrl register */
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__MCLK_IO_CTRL,
		BIT_MSK__MCLK_IO_CTRL__REG_MCLK_OEN,
		FALSE);
	/* Output channel status override enable */
	sii_drv_cra_bits_set8(dev_id,
		REG_ADDR__RX_AUD_OUT_SPDIF_CH_CTRL_1,
		BIT_MSK__RX_AUD_OUT_SPDIF_CTRL_1__REG_CBIT_OVERRIDE,
		TRUE);

	/* Initialise ECC Mask */
	sii9437_hal_ecc_mask_init(dev_id);

	/* Disable ECC by default.
	 * Will be enabled for non-lpcm during channel status update */
	sii9437_hal_ecc_enable(dev_id, FALSE);

	/* Initialise PU and PE so that HPD will go low properly
	 * on HPD removal */
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__GPIO_PE_CTRL,
			BIT_MASK__GPIO_PE_CTRL__REG_GPIO4_PE, TRUE);
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__GPIO_PU_CTRL,
			BIT_MASK__GPIO_PU_CTRL__REG_GPIO4_PU, FALSE);

	sii_drv_cra_write8(dev_id, REG_ADDR__I2S_CS_PTR,
			BIT_MASK__I2S_CS_PTR__REG_CS_EXTRA_PTR_OFFSET);

	/* eARC RXs shall refer to sub-frame 1 Channel Status.
	(left channel sub-frame for the L-PCM audio) as defined in
	.IEC60958-1 and IEC60958-3 */
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__RX_BPM_CS_CTRL,
			BIT_MSK__RX_BPM_CS_CTRL__REG_USE_L_CS, TRUE);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_timer_value_set(uint32_t dev_id, uint16_t time_value_ms)
{
	uint16_t timer_count = 0;

	if (time_value_ms)
		timer_count = (time_value_ms *
			SII943X_TIMER_COUNT_VALUE_REF) - 1;

	sii_drv_cra_write16(dev_id, REG_ADDR__TIMER_VAL0, timer_count);

	if (!time_value_ms) {
		uint16_t wait_count = 10;
		while (0 == (sii_drv_cra_read8(dev_id, REG_ADDR__TOP_INTR1) &
				BIT_MSK__TOP_INTR1__REG_TOP_INTR_STAT1)) {
			SII_LOG3A("", NULL, ("Waiting for Timer Interrupt"));
			wait_count--;
			if (wait_count <= 0)
				break;

		}
		sii_drv_cra_write8(dev_id, REG_ADDR__TOP_INTR1,
				BIT_MSK__TOP_INTR1__REG_TOP_INTR_STAT1);
	}

	/* First get will be always 0. So a dummy read*/
	sii9437_hal_timer_value_get(dev_id, &timer_count);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_timer_value_get(uint32_t dev_id, uint16_t *time_value_ms)
{
	uint16_t timer_count = 0;
	timer_count = sii_drv_cra_read16(dev_id, REG_ADDR__TIMER_VAL0);

	/* Use ceil[timer_count/timer_count_ref] to round off sub millisecond
	 * timer value */
	if (timer_count) {
		timer_count++;
		if (timer_count % SII943X_TIMER_COUNT_VALUE_REF) {
			timer_count /= SII943X_TIMER_COUNT_VALUE_REF;
			timer_count++;
		} else {
			timer_count /= SII943X_TIMER_COUNT_VALUE_REF;
		}
	}

	*time_value_ms = timer_count;

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_gpio_query(uint32_t dev_id, uint16_t *p_gpio_pin_mask)
{
	*p_gpio_pin_mask = sii_drv_cra_read8(dev_id, REG_ADDR__GPIO_C_CTRL);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_gpio_config(uint32_t dev_id, uint16_t *gpio_pin_io_config)
{
	/* Enable output registers: 0 - enables output */
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__GPIO_OEN_CTRL,
		(BIT_MSK__GPIO_OEN_CTRL__REG_GPIO_OEN & *gpio_pin_io_config),
		FALSE);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_hpd_init(uint32_t dev_id, bool_t *b_hpd_in_value)
{
	uint16_t gpio_values;

	/* Enable GPIO4 as HPD-IN */
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__GPIO_OEN_CTRL,
			BIT__GPIO_NUM_4, TRUE);

	/* By default write RX_HPD_IN to GPIO5 */
	/* and configure GPIO5 as RX_HPD_OUT */
	sii9437_hal_gpio_query(dev_id, &gpio_values);
	*b_hpd_in_value = (gpio_values & BIT__GPIO_NUM_4) ? TRUE : FALSE;
	sii9437_hal_hpd_set(dev_id, *b_hpd_in_value);

	/* Enable GPIO5 as HPD-OUT */
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__GPIO_OEN_CTRL,
			BIT__GPIO_NUM_5, FALSE);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_gpio_output_update(uint32_t dev_id,
		uint16_t *p_gpio_pin_mask, bool_t b_set)
{
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__GPIO_I_CTRL,
			(BIT_MSK__GPIO_I_CTRL__REG_GPIO_I & *p_gpio_pin_mask),
			b_set);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_chip_reset(uint32_t dev_id)
{
	uint32_t milli_to;
	sii_sys_time_out_milli_set(&milli_to, 1);

	/* Trigger reset */
	sii_drv_cra_write8(dev_id, REG_ADDR__SWHRST, 0x80);
	/* wait for reset done or time out */
	while (sii_drv_cra_read8(dev_id, REG_ADDR__SWHRST))
		/* && !SiiSysTimeOutMilliIs(&MilliTO)) */;

	SII_LOG1A("", NULL, ("Rx Reset Done\n"));
	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_earc_hpd_set(uint32_t dev_id, bool_t b_en)
{
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__CMC_SLAVE_RX6,
				BIT_MSK__CMC_SLAVE_RX6__EARC_HPD, b_en);
	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_hpd_set(uint32_t dev_id, bool_t b_en)
{
	uint16_t hpd_out_bit_mask = BIT__GPIO_NUM_5;
	return sii9437_hal_gpio_output_update(dev_id, &hpd_out_bit_mask, b_en);
}

uint32_t sii9437_hal_hdmi_hpd_query(uint32_t dev_id, bool_t *hpd_status)
{
	uint8_t status;
	status = sii_drv_cra_read8(dev_id, REG_ADDR__CMC_SLAVE_RX5);

	*hpd_status = status & BIT_MASK__CMC_SLAVE_RX5__HDMI_HPD;

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_tx_hb_status_clear(uint32_t dev_id)
{
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__SRST4,
				BIT_MSK__SRST4__REG_RST_4CLK_CMC_RX_N, FALSE);
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__SRST4,
				BIT_MSK__SRST4__REG_RST_4CLK_CMC_RX_N, TRUE);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_tx_stat_chng_conf_query(uint32_t dev_id,
		bool_t *p_b_stat_chng_conf)
{
	uint8_t status;
	status = sii_drv_cra_read8(dev_id, REG_ADDR__CMC_SLAVE_RX5);

	*p_b_stat_chng_conf = status & BIT_MASK__CMC_SLAVE_RX5__STAT_CHNG_CONF;

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_tx_cap_chng_conf_query(uint32_t dev_id,
		bool_t *p_b_cap_chng_conf)
{
	uint8_t status;
	status = sii_drv_cra_read8(dev_id, REG_ADDR__CMC_SLAVE_RX5);

	*p_b_cap_chng_conf = status & BIT_MASK__CMC_SLAVE_RX5__CAP_CNG_CONF;

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_caps_ds_set(uint32_t dev_id,
		uint8_t *p_caps_ds,
		uint16_t size)
{
	sii_drv_cra_write8(dev_id,
			REG_ADDR__EDID_FIFO_ADDR, 0x0);
	sii_drv_cra_fifo_write8(dev_id,
			REG_ADDR__EDID_FIFO_DATA, p_caps_ds, size);

	return SII_RETURN_VALUE__SUCCESS;
}

#define SII9437_FIRST_CSB_BLOCK_SIZE (REG_ADDR__RX_CS_EXTRA1 - REG_ADDR__RX_CHST0)
#define SII9437_LAST_CSB_BLOCK_SIZE (7)

uint32_t sii9437_hal_channel_status_query(uint32_t dev_id,
				uint8_t *p_data,
				uint16_t size)
{
	int read_size;
	int offset;
	int total_read = 0;

	/* Read the first block of CSB bytes from contiguous registers */
	if (size >= SII9437_FIRST_CSB_BLOCK_SIZE) {
		read_size = SII9437_FIRST_CSB_BLOCK_SIZE;
	}
	sii_drv_cra_block_read8(dev_id, REG_ADDR__RX_CHST0,
				p_data, read_size);
	total_read += read_size;

	/* Read the last block of CSB bytes (Audio InfoFrame) using pointer registers */
	for (offset = 0; offset < SII9437_LAST_CSB_BLOCK_SIZE; offset += 2) {
		read_size = ((SII9437_LAST_CSB_BLOCK_SIZE - offset) > 1) ? 2 : 1;
		if ((size - total_read) < read_size) {
			read_size = size - total_read;
		}

		sii_drv_cra_write8(dev_id, REG_ADDR__I2S_CS_PTR,
				BIT_MASK__I2S_CS_PTR__REG_CS_EXTRA_PTR_OFFSET + offset);

		sii_drv_cra_block_read8(dev_id, REG_ADDR__RX_CS_EXTRA1,
				 &(p_data[total_read]), read_size);

		total_read += read_size;
	}

	 return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_aud_mute_set(uint32_t dev_id, bool_t en)
{
	SII_LOG1A("", NULL, ("Audio Mute - %s\n",
				(en ? "ENABLED" : "DISABLED")));
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__MUTE_CTRL,
			BIT_MSK__MUTE_CTRL__REG_MUTE_EN, en);
	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_aud_mode_set(uint32_t dev_id,
					bool_t is_spdif_mode)
{
	sii_drv_cra_bits_set8(dev_id,
			REG_ADDR__RX_AUD_OUT_SPDIF_CH_CTRL_1,
			BIT_MSK__RX_AUD_OUT_SPDIF_CTRL_1__REG_RX_SPDIF_EN,
			is_spdif_mode);

	/* i2s output enable */
	sii_drv_cra_write8(dev_id, REG_ADDR__RX_I2S_CTRL2,
			is_spdif_mode ? 0x00 : 0xF0);
	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_clk_lock_mode_set(uint32_t dev_id,
					bool_t is_ckdt_mode)
{
	/* Setting Clock detect mode */
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__EARC_CFG0,
			BIT_MASK__EARC_CFG0__CKDT_MODE,
			is_ckdt_mode);

	/* Clear any unwanted interrupts */
	sii_drv_cra_write8(dev_id, REG_ADDR__TOP_INTR1,
			BIT_MSK__TOP_INTR1__REG_TOP_INTR_STAT2 |
			BIT_MSK__TOP_INTR1__REG_TOP_INTR_STAT3);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_clk_div_set(uint32_t dev_id,
		uint8_t clk_div)
{
	sii_drv_cra_write8(dev_id,
			REG_ADDR__RX_AUD_OUT_CH_CTRL,
			clk_div);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_bsync_count_set(uint32_t dev_id,
					uint8_t bsync_count)
{
	sii_drv_cra_write8(dev_id,
			REG_ADDR__BSYNC_CTRL, bsync_count);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_output_channel_status_set(uint32_t dev_id,
				uint8_t *pchannel_status_out,
				uint8_t *pchannel_status_extra)
{
	sii_drv_cra_block_write8(dev_id, REG_ADDR__AUD_CHST0,
				pchannel_status_out, 5);

	sii_drv_cra_block_write8(dev_id, REG_ADDR__AUD_CHST5,
				pchannel_status_extra, 4);

	sii_drv_cra_bits_set8(dev_id,
			REG_ADDR__RX_AUD_OUT_SPDIF_CH_CTRL_1,
			BIT_MSK__RX_AUD_OUT_SPDIF_CTRL_1__REG_CS_UPDATE_EN,
			TRUE);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_conn_status_get(uint32_t dev_id, bool_t *p_conn)
{
	uint8_t val;
	val = sii_drv_cra_read8(dev_id, REG_ADDR__DISC_DISCONN_RX8);
	*p_conn = (val &
		BIT_MSK__DISC_DISCONN_RX8__REG_RX_EARC_CONNECTION_STATUS)
		? TRUE : FALSE;

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_arc_mode_set(uint32_t dev_id,
				enum sii_arc_mode arc_mode)
{
	switch (arc_mode) {
	case SII_ARC_MODE__ARC:
		/* Legacy ARC Mode */
		/* Raw spdif output from ARC.
		 * This is the functional path for legacy ARC. */
		sii_drv_cra_write8(dev_id, REG_ADDR__SPDIF_SEL,
				BIT_MSK__SPDIF_SEL__FUNC_PATH_LEGCY_ARC);
		sii_drv_cra_write8(dev_id, REG_ADDR__IO_OEN, 0xBF);
		sii_drv_cra_write8(dev_id, REG_ADDR__VIOLA_MODE,
			(BIT_MSK__VIOLA_MODE__REG_AUTO_CFG |
			BIT_MSK__VIOLA_MODE__REG_ARC_DIR |
			BIT_MSK__VIOLA_MODE__PLL_EN));
		sii_drv_cra_write8(dev_id, REG_ADDR__EARC_CFG0,
			BIT_MASK__EARC_CFG0__RX_DPATH_PWRDN |
			BIT_MASK__EARC_CFG0__CLDT_BLK_PWRDN |
			BIT_MASK__EARC_CFG0__CKDT_MODE |
			BIT_MASK__EARC_CFG0__TX_DATAPATH_POL_CHG |
			BIT_MASK__EARC_CFG0__PLL_CLK_RX_DATAPATH |
			BIT_MASK__EARC_CFG0__PLL_CLK_TX_DATAPATH);

		sii_drv_cra_write8(dev_id, REG_ADDR__EARC_CFG_1,
				(BIT_MSK__EARC_CFG1__RTERM_30OHM |
				BIT_MASK__EARC_CFG_1__SINGLE_MODE));
		sii_drv_cra_write8(dev_id, REG_ADDR__VIOLA_SLEW_N_SWING,
							0x2B);
		sii_drv_cra_write8(dev_id, REG_ADDR__VIOLA_SPRX, 0x04);
		sii_drv_cra_write8(dev_id, REG_ADDR__VIOLA_TERM60, 0x0F);
		sii_drv_cra_write8(dev_id, REG_ADDR__VIOLA_TERM300, 0x0C);
		sii_drv_cra_write8(dev_id, REG_ADDR__VIOLA_CM_BW_CTL, 0x00);

		/* Disable test mode */
		sii_drv_cra_bits_set8(dev_id, REG_ADDR__TEST_CTRL,
				BIT_MSK__TEST_CTRL__REG_TEST_EN,
				FALSE);

		sii9437_hal_ecc_enable(dev_id, FALSE);

		break;

	case SII_ARC_MODE__EARC:
		/* eARC Mode */
		/* disable test mode */
		sii_drv_cra_bits_set8(dev_id, REG_ADDR__TEST_CTRL,
				BIT_MSK__TEST_CTRL__REG_TEST_EN,
				FALSE);
		/* Output from Rx side spdif module.
		 * This is the functional path for eARC. */
		sii_drv_cra_write8(dev_id, REG_ADDR__SPDIF_SEL,
				BIT_MSK__SPDIF_SEL__FUNC_PATH_EARC);
		sii_drv_cra_write8(dev_id, REG_ADDR__VIOLA_MODE,
			(BIT_MSK__VIOLA_MODE__REG_AUTO_CFG |
			BIT_MSK__VIOLA_MODE__REG_LOCK |
			BIT_MSK__VIOLA_MODE__REG_LOCK_OVR |
			BIT_MSK__VIOLA_MODE__REG_ARC_MODE |
			BIT_MSK__VIOLA_MODE__REG_ARC_DIR |
			BIT_MSK__VIOLA_MODE__PLL_EN));

		sii_drv_cra_write8(dev_id, REG_ADDR__EARC_CFG0,
			BIT_MASK__EARC_CFG0__CKDT_MODE |
			BIT_MASK__EARC_CFG0__ARC_DATA_PATH_FIL_EN |
			BIT_MASK__EARC_CFG0__RX_DPATH_PWRDN |
			BIT_MASK__EARC_CFG0__CLDT_BLK_PWRDN |
			BIT_MASK__EARC_CFG0__TX_DATAPATH_POL_CHG |
			BIT_MASK__EARC_CFG0__PLL_CLK_RX_DATAPATH |
			BIT_MASK__EARC_CFG0__PLL_CLK_TX_DATAPATH);
		sii_drv_cra_write8(dev_id, REG_ADDR__EARC_CFG_1,
				(BIT_MSK__EARC_CFG1__CM_LEVEL_CTRL_350MV |
				BIT_MSK__EARC_CFG1__ARC_MODE_CTRL_CM));
		sii_drv_cra_write8(dev_id, REG_ADDR__VIOLA_SLEW_N_SWING, 0x52);
		sii_drv_cra_write8(dev_id, REG_ADDR__VIOLA_SPRX, 0x23);
		sii_drv_cra_write8(dev_id, REG_ADDR__VIOLA_TERM60, 0x0C);
		sii_drv_cra_write8(dev_id, REG_ADDR__VIOLA_TERM300, 0x0C);
		sii_drv_cra_write8(dev_id, REG_ADDR__VIOLA_CM_BW_CTL, 0x08);

		sii_drv_cra_write8(dev_id, REG_ADDR__IO_OEN, 0x00);

		break;

	case SII_ARC_MODE__NONE:

		/*Disable output */
		sii_drv_cra_write8(dev_id, REG_ADDR__IO_OEN, 0xFF);

		/* Raw spdif output from ARC.
		 * This is the functional path for legacy ARC. */
		sii_drv_cra_write8(dev_id, REG_ADDR__SPDIF_SEL,
				BIT_MSK__SPDIF_SEL__FUNC_PATH_LEGCY_ARC);
		sii_drv_cra_write8(dev_id, REG_ADDR__VIOLA_MODE,
			(BIT_MSK__VIOLA_MODE__REG_AUTO_CFG |
			BIT_MSK__VIOLA_MODE__REG_ARC_DIR |
			BIT_MSK__VIOLA_MODE__PLL_EN));
		sii_drv_cra_write8(dev_id, REG_ADDR__EARC_CFG0,
			BIT_MASK__EARC_CFG0__RX_DPATH_PWRDN |
			BIT_MASK__EARC_CFG0__CLDT_BLK_PWRDN |
			BIT_MASK__EARC_CFG0__CKDT_MODE |
			BIT_MASK__EARC_CFG0__TX_DP_POL_CM_ARC |
			BIT_MASK__EARC_CFG0__PLL_CLK_RX_DATAPATH |
			BIT_MASK__EARC_CFG0__PLL_CLK_TX_DATAPATH);
		sii_drv_cra_write8(dev_id, REG_ADDR__EARC_CFG_1,
				(BIT_MSK__EARC_CFG1__CM_LEVEL_CTRL_350MV |
				BIT_MSK__EARC_CFG1__ARC_MODE_CTRL_CM));
		sii_drv_cra_write8(dev_id, REG_ADDR__VIOLA_TERM60, 0x0C);
		sii_drv_cra_write8(dev_id, REG_ADDR__VIOLA_TERM300, 0x0C);
		sii_drv_cra_write8(dev_id, REG_ADDR__VIOLA_CM_BW_CTL, 0x00);
		/* Enable test mode */
		sii_drv_cra_bits_set8(dev_id, REG_ADDR__TEST_CTRL,
				BIT_MSK__TEST_CTRL__REG_TEST_EN,
				TRUE);

		sii9437_hal_ecc_enable(dev_id, FALSE);

		break;

	default:

		break;
	}

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_ecc_mask_init(uint32_t dev_id)
{
	/* pointer to starting bit of cs byte w/ compressed bit (cs #0)
	 * 0x0 * 8 = 0x00 (0th bit) */
	sii_drv_cra_write8(dev_id, REG_ADDR__RX_CS_ECC_PTR, 0x00);

	/* mask - compare bits 1, 3, 4 & 5 */
	sii_drv_cra_write8(dev_id, REG_ADDR__RX_CS_ECC_MASK, SII943X_ECC_MASK);

	/* bits 1, 3, 4 & 5 value to match for ecc on: 1, 0, 0, 0 */
	sii_drv_cra_write8(dev_id, REG_ADDR__RX_CS_ECC_PATTERN,
			SII943X_ECC_PATTERN);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_ecc_mask_set(uint32_t dev_id)
{
	/* bits 1, 3, 4 & 5 value to match for ecc on: 1, 0, 0, 0 */
	sii_drv_cra_write8(dev_id, REG_ADDR__RX_CS_ECC_PATTERN,
			SII943X_ECC_PATTERN);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_ecc_mask_get(uint32_t dev_id, uint8_t *ecc_mask)
{
	if (ecc_mask) {
		*ecc_mask = sii_drv_cra_read8(dev_id,
				REG_ADDR__RX_CS_ECC_PATTERN);
	}

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_ecc_enable(uint32_t dev_id, bool_t b_en)
{
	/* enable ecc if ecc=on and enable BCH interleaving */
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__BCH_CTRL,
			(BIT_MSK__BCH_CTRL__REG_ECC_ENABLE |
			BIT_MSK__BCH_CTRL__REG_BCHINTV_EN), b_en);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_ecc_status(uint32_t dev_id, bool_t *p_b_on)
{
	if (p_b_on) {
		*p_b_on = sii_drv_cra_read8(dev_id, REG_ADDR__BCH_STAT) &
				BIT_MSK__BCH_STAT__ECC_DEC_ON ? TRUE : FALSE;
	}

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_ecc_err_cnt_get(uint32_t dev_id, uint16_t *p_err_cnt)
{
	if (p_err_cnt)
		*p_err_cnt = sii_drv_cra_read16(dev_id, REG_ADDR__PKT_TOP_RX13);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_ecc_err_cnt_clear(uint32_t dev_id)
{
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__PKT_TOP_RX12,
		BIT_MSK__PKT_TOP_RX12__REG_CMC_RX_PKT_ECC_ERR_CLR, TRUE);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_ecc_err_fixed_cnt_get(uint32_t dev_id,
		uint16_t *p_err_fixed_cnt)
{
	if (p_err_fixed_cnt)
		*p_err_fixed_cnt = sii_drv_cra_read16(dev_id,
				REG_ADDR__PKT_TOP_RX16);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_ecc_err_fixed_cnt_clear(uint32_t dev_id)
{
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__PKT_TOP_RX15,
		BIT_MSK__PKT_TOP_RX15__REG_CMC_RX_PKT_ECC_FIX_CLR, TRUE);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_sm_state_get(uint32_t dev_id, uint8_t *p_state_val)
{
	if (p_state_val) {
		*p_state_val = sii_drv_cra_read8(dev_id,
				REG_ADDR__DISC_DISCONN_RX8) &
		BIT_MSK__DISC_DISCONN_RX8__REG_RX_DISC_DISCONN_STATE_B2_B0;
	}

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_sm_state_set(uint32_t dev_id, uint8_t state_val)
{
	sii_drv_cra_bits_mod8(dev_id, REG_ADDR__DISC_DISCONN_RX9,
		BIT_MSK__DISC_DISCONN_RX9__REG_RX_DISC_DISCON_ST_OVR_B2_B0,
		state_val);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_sm_state_override_enable(uint32_t dev_id, bool_t b_en)
{
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__DISC_DISCONN_RX9,
		BIT_MSK__DISC_DISCONN_RX9__REG_RX_DISC_CONN_ST_OVERRIDE_EN,
		b_en);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_idle2_enable(uint32_t dev_id, bool_t b_en)
{
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__DISC_DISCONN_RX9,
		BIT_MSK__DISC_DISCONN_RX9__REG_RX_DISC_CON_ST_NO_IDLE2,
		!b_en);

	return SII_RETURN_VALUE__SUCCESS;
}
uint32_t sii9437_hal_arc_init_term(uint32_t dev_id, bool_t b_init_is)
{
	if (b_init_is) {
		sii_drv_cra_bits_set8(dev_id, REG_ADDR__CMC_SLAVE_RX7,
			BIT_MSK__CMC_SLAVE_RX7__REG_CEC_ARC_INIT, TRUE);
		sii_drv_cra_bits_set8(dev_id, REG_ADDR__CMC_SLAVE_RX7,
			BIT_MSK__CMC_SLAVE_RX7__REG_CEC_ARC_TERM, FALSE);
	} else {
		sii_drv_cra_bits_set8(dev_id, REG_ADDR__CMC_SLAVE_RX7,
			BIT_MSK__CMC_SLAVE_RX7__REG_CEC_ARC_INIT, FALSE);
		sii_drv_cra_bits_set8(dev_id, REG_ADDR__CMC_SLAVE_RX7,
			BIT_MSK__CMC_SLAVE_RX7__REG_CEC_ARC_TERM, TRUE);
	}

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_i2s_mclk_mode_set(uint32_t dev_id,
		enum sii9437_mclk_mode mclk_mode)
{
	switch (mclk_mode) {

	case SII9437_MCLK_MODE__NONE:
		/* Do not overwrite MCLK OUT */
		sii_drv_cra_bits_set8(dev_id, REG_ADDR__MCLK_CTRL,
				BIT_MSK__MCLK_CTRL__REG_OVR_EN, FALSE);
		break;

	case SII9437_MCLK_MODE__FS128:
		sii_drv_cra_write8(dev_id, REG_ADDR__MCLK_CTRL, 0x4E);
		break;

	case SII9437_MCLK_MODE__FS256:
		sii_drv_cra_write8(dev_id, REG_ADDR__MCLK_CTRL, 0x4A);
		break;

	case SII9437_MCLK_MODE__FS512:
		sii_drv_cra_write8(dev_id, REG_ADDR__MCLK_CTRL, 0x40);
		break;

	default:
		break;

	}

	sii_drv_cra_bits_set8(dev_id, REG_ADDR__MCLK_IO_CTRL,
			BIT_MSK__MCLK_IO_CTRL__REG_MCLK_OEN, FALSE);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_top_int_get(uint32_t dev_id, uint8_t *p_top_int)
{
	*p_top_int = sii_drv_cra_read8(dev_id, REG_ADDR__TOP_INTR_STATE);
	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_top_int_status_get(uint32_t dev_id,
				 uint8_t *p_int_top_regs,
				 uint16_t num_regs)
{
	uint16_t i;
	sii_drv_cra_block_read8(dev_id, REG_ADDR__TOP_INTR1,
					p_int_top_regs, num_regs);
	/* Clear all interrupts */
	for (i = 0; i < num_regs; i++) {
		if (p_int_top_regs[i])
			sii_drv_cra_write8(dev_id, (REG_ADDR__TOP_INTR1 + i),
						p_int_top_regs[i]);
	}

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_enable_all_int(uint32_t dev_id, bool_t en)
{
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__TOP_INT_CTRL,
			BIT_MSK__TOP_INT_CTRL__REG_GLOBAL_INTR_MASK, en);
	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_earc_block_int_get(uint32_t dev_id, uint8_t *p_int)
{
	*p_int = sii_drv_cra_read8(dev_id, REG_ADDR__INTR_STATE);
	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_gpio_event_mask_set(uint32_t dev_id,
		uint8_t *gpio_pin_event_mask)
{
	uint8_t intr2_reg_value = 0, intr3_reg_value = 0;

	if (*gpio_pin_event_mask & BIT__GPIO_NUM_0)
		intr2_reg_value |= (BIT_MSK__TOP_INTR2__REG_TOP_INTR_STAT6 |
				BIT_MSK__TOP_INTR2__REG_TOP_INTR_STAT12);

	if (*gpio_pin_event_mask & BIT__GPIO_NUM_1)
		intr2_reg_value |= (BIT_MSK__TOP_INTR2__REG_TOP_INTR_STAT7 |
				BIT_MSK__TOP_INTR2__REG_TOP_INTR_STAT13);

	if (*gpio_pin_event_mask & BIT__GPIO_NUM_2) {
		intr2_reg_value |= BIT_MSK__TOP_INTR2__REG_TOP_INTR_STAT8;
		intr3_reg_value |= BIT_MSK__TOP_INTR3__REG_TOP_INTR_STAT14;
	}

	if (*gpio_pin_event_mask & BIT__GPIO_NUM_3) {
		intr2_reg_value |= BIT_MSK__TOP_INTR2__REG_TOP_INTR_STAT9;
		intr3_reg_value |= BIT_MSK__TOP_INTR3__REG_TOP_INTR_STAT15;
	}

	if (*gpio_pin_event_mask & BIT__GPIO_NUM_4) {
		intr2_reg_value |= BIT_MSK__TOP_INTR2__REG_TOP_INTR_STAT10;
		intr3_reg_value |= BIT_MSK__TOP_INTR3__REG_TOP_INTR_STAT16;

	}

	if (*gpio_pin_event_mask & BIT__GPIO_NUM_5) {
		intr2_reg_value |= BIT_MSK__TOP_INTR2__REG_TOP_INTR_STAT11;
		intr3_reg_value |= BIT_MSK__TOP_INTR3__REG_TOP_INTR_STAT17;
	}

	sii_drv_cra_write8(dev_id, REG_ADDR__TOP_INTR2_MASK, intr2_reg_value);
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__TOP_INTR3_MASK,
				BIT_MSK__TOP_INTR3_MASK__REG_INTR_MASK_B17_B14 &
				intr3_reg_value, TRUE);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_earc_int_status_get(uint32_t dev_id,
				 uint8_t *p_int_regs,
				 uint16_t num_regs)
{
	uint8_t i;
	sii_drv_cra_block_read8(dev_id,
				REG_ADDR__INTR0,
				p_int_regs,
				num_regs);
	/* Clear all interrupts */
	for (i = 0; i < num_regs; i++) {
		if (p_int_regs[i]) {
			sii_drv_cra_write8(dev_id,
				(REG_ADDR__INTR0 + i),
				p_int_regs[i]);
		}
	}
	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_is_heartbeat_lost(uint32_t dev_id, bool_t *p_status)
{
	*p_status = (sii_drv_cra_read8(dev_id,
			REG_ADDR__CMC_SLAVE_RX4) &
			BIT_MSK__CMC_SLAVE_RX4__LOST_HEARTBEAT) ? TRUE : FALSE;
	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_tx_data_get(uint32_t dev_id, uint8_t *p_data)
{
	*p_data = sii_drv_cra_read8(dev_id, REG_ADDR__CMC_SLAVE_RX8);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_rx_softreset(uint32_t dev_id)
{
	/* Set REG_RST_4CLK_EARC_LINK_N to retain RX_FREQ_VAL0 register value */
	sii_drv_cra_bits_mod8(dev_id, REG_ADDR__SRST,
			(BIT_MSK__SRST__REG_RST_4CLK_EXTR_N |
			BIT_MSK__SRST__REG_RST_4CLK_EARC_LINK_N |
					BIT_MSK__SRST__REG_RST_4CLK_I2S_OUT_N),
			BIT_MSK__SRST__REG_RST_4CLK_EARC_LINK_N);
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__MUTE_CTRL,
				BIT_MSK__MUTE_CTRL__REG_MUTE_EN, FALSE);
	/* Set REG_RST_4CLK_EARC_LINK_N to retain RX_FREQ_VAL0 register value */
	sii_drv_cra_bits_mod8(dev_id, REG_ADDR__SRST,
			(BIT_MSK__SRST__REG_RST_4CLK_EXTR_N |
			BIT_MSK__SRST__REG_RST_4CLK_EARC_LINK_N |
					BIT_MSK__SRST__REG_RST_4CLK_I2S_OUT_N),
			(BIT_MSK__SRST__REG_RST_4CLK_EXTR_N |
			BIT_MSK__SRST__REG_RST_4CLK_EARC_LINK_N |
					BIT_MSK__SRST__REG_RST_4CLK_I2S_OUT_N));

	sii_drv_cra_bits_set8(dev_id, REG_ADDR__MUTE_CTRL,
					BIT_MSK__MUTE_CTRL__REG_MUTE_EN, TRUE);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_rx_earc_clk_freq_get(uint32_t dev_id,
		uint32_t *earc_freq)
{
	uint8_t rx_freq_val[2];

	sii_drv_cra_bits_set8(dev_id, REG_ADDR__SRST,
			BIT_MSK__SRST__REG_RST_4CLK_EARC_LINK_N, TRUE);

	sii_sys_time_milli_delay(1);

	sii_drv_cra_block_read8(dev_id, REG_ADDR__RX_FREQ_VAL0, rx_freq_val, 2);

	*earc_freq = (uint32_t) ((uint32_t) rx_freq_val[1] << 8 |
			(uint32_t)rx_freq_val[0]) *
			(uint32_t)EARC_CLOCK_FREQ_SCALE_FACTOR;

	SII_LOG1A("", NULL, ("eARC clock Frequency: %d Hz\n", *earc_freq));

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_reset_aud_fifo(uint32_t dev_id)
{
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__SRST2,
			BIT_MSK__SRST2__REG_RST_RX_FIFO_N, FALSE);
	sii_sys_time_milli_delay(1);
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__SRST2,
			BIT_MSK__SRST2__REG_RST_RX_FIFO_N, TRUE);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_int_mask_enable(uint32_t dev_id, bool_t en)
{
	uint8_t maskRegs[REG_RX__INT_NUM] = {0};
	uint8_t maskTopRegs[REG_TOP__INT_NUM] = {0};

	if (en) {
		maskRegs[REG_INTR_0] = BIT_MASK__INTR0_MASK__STATE_IDLE1 |
				BIT_MASK__INTR0_MASK__STATE_IDLE2;
		maskRegs[REG_INTR_1] = BIT_MASK__INTR1_MASK__STATE_DISC1 |
				BIT_MASK__INTR1_MASK__STATE_DISC2 |
				BIT_MASK__INTR1_MASK__STATE_ARC;
		maskRegs[REG_INTR_2] = BIT_MSK__INTR2__REG_INTR_STAT18;
		maskRegs[REG_INTR_3] = BIT_MSK__INTR3__REG_INTR_STAT31;
		maskRegs[REG_INTR_4] = BIT_MSK__INTR4__REG_INTR_STAT33 |
				BIT_MASK__INTR4_MASK__STATE_EARC;
		maskRegs[REG_INTR_6] = BIT_MSK__INTR6__REG_INTR_STAT51 |
				BIT_MSK__INTR6__REG_INTR_STAT52;

		/* HDMI HPD, STAT_CHNG_CONF, CAP_CHNG_CONF interrupts */
		maskRegs[REG_INTR_7] = BIT_MSK__INTR7__REG_INTR_STAT56 |
					BIT_MSK__INTR7__REG_INTR_STAT59 |
					BIT_MSK__INTR7__REG_INTR_STAT60;
		maskRegs[REG_INTR_8] = BIT_MSK__INTR8__REG_INTR_STAT66 |
					BIT_MSK__INTR8__REG_INTR_STAT65 |
					BIT_MSK__INTR8__REG_INTR_STAT64;

		/* Enable timer interrupt */
		maskTopRegs[REG_INTR_TOP_1] |=
				BIT_MSK__TOP_INTR1__REG_TOP_INTR_STAT1;

		/* GPIO4 change from 0 to 1 */
		maskTopRegs[REG_INTR_TOP_2] =
				BIT_MSK__TOP_INTR2__REG_TOP_INTR_STAT10;
		/* GPIO4 change from 1 to 0 */
		maskTopRegs[REG_INTR_TOP_3] =
				BIT_MSK__TOP_INTR3__REG_TOP_INTR_STAT16;
	}

	sii_drv_cra_block_write8(dev_id, REG_ADDR__INTR0_MASK,
					maskRegs, REG_RX__INT_NUM);
	sii_drv_cra_block_write8(dev_id, REG_ADDR__TOP_INTR1_MASK,
					maskTopRegs, REG_TOP__INT_NUM);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_hpd_int_mask_enable(uint32_t dev_id, bool_t b_en)
{
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__TOP_INTR2_MASK,
			BIT_MSK__TOP_INTR2__REG_TOP_INTR_STAT10, b_en);
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__TOP_INTR3_MASK,
			BIT_MSK__TOP_INTR3__REG_TOP_INTR_STAT16, b_en);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_sm_states_intr_enable(uint32_t dev_id)
{
	uint8_t maskRegs[REG_RX__INT_NUM] = {0};
	maskRegs[REG_INTR_0] = BIT_MASK__INTR0_MASK__STATE_IDLE1 |
			BIT_MASK__INTR0_MASK__STATE_IDLE2;
	maskRegs[REG_INTR_1] = BIT_MASK__INTR1_MASK__STATE_DISC1 |
			BIT_MASK__INTR1_MASK__STATE_DISC2 |
			BIT_MASK__INTR1_MASK__STATE_ARC;

	maskRegs[REG_INTR_4] = BIT_MASK__INTR4_MASK__STATE_EARC;

	sii_drv_cra_block_write8(dev_id, REG_ADDR__INTR0_MASK,
			maskRegs, REG_RX__INT_NUM);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_earc_lock_chg_intr_en(uint32_t dev_id, bool_t  enable)
{
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__TOP_INTR1_MASK,
			BIT_MSK__TOP_INTR1__REG_TOP_INTR_STAT3, enable);
	/* Clear any spurious lock det chng interrupt */
	if (!enable) {
		sii_drv_cra_write8(dev_id, REG_ADDR__TOP_INTR1,
				BIT_MSK__TOP_INTR1__REG_TOP_INTR_STAT3);
	}

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_earc_only_intr_en(uint32_t dev_id, bool_t  enable)
{
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__INTR1_MASK,
			BIT_MSK__INTR1__REG_INTR_STAT12, enable);
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__INTR2_MASK,
			(BIT_MSK__INTR2__REG_INTR_STAT16 |
			BIT_MSK__INTR2__REG_INTR_STAT19), enable);
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__INTR4_MASK,
			BIT_MASK__INTR4_MASK__PKT_ECC_ERR_FIXED,
			enable);
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__INTR6_MASK,
			(BIT_MASK__INTR4_MASK__PKT_ECC_ERR |
			BIT_MASK__INTR4_MASK__ECC_ERR), enable);

	sii_drv_cra_bits_set8(dev_id, REG_ADDR__TOP_INTR1_MASK,
			(BIT_MSK__TOP_INTR1__REG_TOP_INTR_STAT3 |
			BIT_MSK__TOP_INTR1__REG_TOP_INTR_STAT2), enable);

	if (!enable) {
		sii_drv_cra_write8(dev_id, REG_ADDR__INTR1,
				BIT_MSK__INTR1__REG_INTR_STAT12);
		sii_drv_cra_write8(dev_id, REG_ADDR__INTR2,
				BIT_MSK__INTR2__REG_INTR_STAT16 |
				BIT_MSK__INTR2__REG_INTR_STAT19);
		sii_drv_cra_write8(dev_id, REG_ADDR__INTR4,
				BIT_MASK__INTR4_MASK__PKT_ECC_ERR_FIXED);
		sii_drv_cra_write8(dev_id, REG_ADDR__INTR6,
				BIT_MASK__INTR4_MASK__PKT_ECC_ERR |
				BIT_MASK__INTR4_MASK__ECC_ERR);

		sii_drv_cra_write8(dev_id, REG_ADDR__TOP_INTR1,
				BIT_MSK__TOP_INTR1__REG_TOP_INTR_STAT3 |
				BIT_MSK__TOP_INTR1__REG_TOP_INTR_STAT2);
	}

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_earc_lock_status_query(uint32_t dev_id,
					bool_t *lock_status)
{
	*lock_status = sii_drv_cra_read8(dev_id, REG_ADDR__VIOLA_STATUS) &
			BIT_MSK__VIOLA_STATUS__EARC_LOCK_DETECT ? TRUE : FALSE;

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_earc_clock_status_query(uint32_t dev_id,
					bool_t *p_clock_status)
{
	*p_clock_status = sii_drv_cra_read8(dev_id, REG_ADDR__VIOLA_STATUS) &
			BIT_MSK__VIOLA_STATUS__EARC_CLK_DETECT ? TRUE : FALSE;

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_ouput_io_enable(uint32_t dev_id,
		uint8_t value)
{
	sii_drv_cra_write8(dev_id, REG_ADDR__IO_OEN, value);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_chip_version_query(uint32_t dev_id,
					uint8_t *p_chip_version)
{
	uint8_t p_chip_id = 0;

	p_chip_id = sii_drv_cra_read8(dev_id,
			REG_ADDR__DEV_IDL);

	if (p_chip_id == 0x37)
		*p_chip_version = 0;
	else if (p_chip_id == 0x39)
		*p_chip_version = 1;

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_rx_cs_mute_mask_pattern_set(uint32_t dev_id)
{
	/* offset pointer within Channel Status array for mute pattern*/
	sii_drv_cra_write8(dev_id, REG_ADDR__RX_CS_MUTE_PTR,
			SII_CS_MUTE_POSITION);
	/* mask for the mute pattern in Channel Status array. */
	sii_drv_cra_write8(dev_id, REG_ADDR__RX_CS_MUTE_MASK,
			SII_CS_BIT_146);
	/* pattern to compare against to see if mute is on. */
	sii_drv_cra_write8(dev_id, REG_ADDR__RX_CS_MUTE_PATTERN,
			SII_CS_BIT_146);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_erx_latency_set(uint32_t dev_id,
					uint8_t erx_latency)
{
	sii_drv_cra_write8(dev_id, REG_ADDR__CMC_SLAVE_RX9, erx_latency);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_stat_chng_bit_set(uint32_t dev_id,
					bool_t b_stat_chng_set)
{
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__CMC_SLAVE_RX6,
				BIT_MSK__CMC_SLAVE_RX6__STAT_CHNG,
				b_stat_chng_set);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_cap_chng_bit_set(uint32_t dev_id, bool_t b_set)
{
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__CMC_SLAVE_RX6,
			BIT_MSK__CMC_SLAVE_RX6__CAP_CHNG,
			b_set);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_erx_latency_req_query(uint32_t dev_id,
					uint8_t *erx_latency_req)
{
	*erx_latency_req = sii_drv_cra_read8(dev_id, REG_ADDR__CMC_SLAVE_RX8);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_erx_latency_req_clear(uint32_t dev_id)
{
	sii_drv_cra_write8(dev_id, REG_ADDR__CMC_SLAVE_RX8,
							0x00);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_tx_pll_enable(uint32_t dev_id, bool_t pll_enable)
{
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__VIOLA_MODE,
					BIT_MSK__VIOLA_MODE__PLL_EN,
					pll_enable);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_rx_lock_toggle(uint32_t dev_id)
{
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__VIOLA_MODE,
			BIT_MSK__VIOLA_MODE__REG_LOCK, false);

	sii_drv_cra_bits_set8(dev_id, REG_ADDR__VIOLA_MODE,
			BIT_MSK__VIOLA_MODE__REG_LOCK, true);

	return SII_RETURN_VALUE__SUCCESS;
}

uint32_t sii9437_hal_rx_comma_enable(uint32_t dev_id, bool_t b_enable)
{
	sii_drv_cra_bits_set8(dev_id, REG_ADDR__SRST4,
			BIT_MSK__SRST4__REG_RST_4CLK_CMC_RX_N, b_enable);

	return SII_RETURN_VALUE__SUCCESS;
}

/***** local functions *******************************************************/

/***** end of file ***********************************************************/

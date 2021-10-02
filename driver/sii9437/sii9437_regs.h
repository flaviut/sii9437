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
 * @file  sii9437_regs.h
 *
 * @brief Defines SiI9437 Receiver Register Info.
 *
 ******************************************************************************/
#ifndef _SII9437_REGS_H_
#define _SII9437_REGS_H_

#define BASE_ADDRESS 0x0000

enum Page_t {
	RX_TOP_REG	= (BASE_ADDRESS | 0x0000),
	RX_EARC_REG	= (BASE_ADDRESS | 0x0100),
	RX_PHY_REG	= (BASE_ADDRESS | 0x0200),
};

/***************** Vienna TOP 60/200-60/2FF*****************/
/* SYS Vendor ID Low byte Register*/
#define REG_ADDR__VND_IDL				(RX_TOP_REG | 0x0000)
	/* (ReadOnly, Bits 7:0)*/
	/* Vendor ID Low byte Sil_Internal((01h))*/
	#define BIT_MSK__VND_IDL__REG_VHDL_IDL			0xFF

/* SYS Vendor ID High byte Register*/
#define REG_ADDR__VND_IDH				(RX_TOP_REG | 0x0001)
	/* (ReadOnly, Bits 7:0)*/
	/* Vendor ID High byte Sil_Internal((00h))*/
	#define BIT_MSK__VND_IDH__REG_VHDL_IDH			0xFF

/* SYS Device ID Low byte Register*/
#define REG_ADDR__DEV_IDL				(RX_TOP_REG | 0x0002)
	/* (ReadOnly, Bits 7:0)*/
	/* Device ID Low byte
     case (mhl_en arc_en ethernet_en repeater_en)
       4'b0000: dev_idl_bo =  8'h85;
       4'b1100: dev_idl_bo =  8'h87;
       4'b1111: dev_idl_bo =  8'h89;
       default:dev_idl_bo =  8'hFF;*/
	#define BIT_MSK__DEV_IDL__DEV_IDL			0xFF

/* SYS Device ID High byte Register*/
#define REG_ADDR__DEV_IDH				(RX_TOP_REG | 0x0003)
	/* (ReadOnly, Bits 7:0)*/
	/* Device ID High byteSil_Internal(: 0x94)*/
	#define BIT_MSK__DEV_IDH__REG_DEV_IDH			0xFF

/* SYS Device Revision Register*/
#define REG_ADDR__DEV_REV				(RX_TOP_REG | 0x0004)
	/* (ReadOnly, Bits 7:0)*/
	/* bit [7:4] - Device Revision (Major Number)
	 bit [3:0] - Device Revision (Minor Number)*/
	#define BIT_MSK__DEV_REV__DEV_REV_ID			0xFF

/* Software Reset Register*/
#define REG_ADDR__SRST					(RX_TOP_REG | 0x0005)
	/* (ReadWrite, Bits 7)*/
	/* Reset Rx I2S out*/
	#define BIT_MSK__SRST__REG_RST_4CLK_I2S_OUT_N		0x80
	/* (ReadWrite, Bits 5)*/
	/* reset logic that runs on extracted clock (the whole Rx)*/
	#define BIT_MSK__SRST__REG_RST_4CLK_EXTR_N		0x20
	/* (ReadWrite, Bits 4)*/
	/* Stop Extraction clock*/
	#define BIT_MSK__SRST__REG_STOP_CLK_EXTR		0x10
	/* (ReadWrite, Bits 3)*/
	/* reset logic that runs on PLL clock (the whole Tx)*/
	#define BIT_MSK__SRST__REG_RST_4CLK_PLL_N		0x08
	/* (ReadWrite, Bits 2)*/
	/* Stop PLL clock*/
	#define BIT_MSK__SRST__REG_STOP_CLK_PLL			0x04
	/* reset for clk_eARC_link*/
	#define BIT_MSK__SRST__REG_RST_4CLK_EARC_LINK_N		0x02
	/* (ReadWrite, Bits 0)*/
	/* reset for fifo between tx_spdif_out and rx_spdif_in.
	 * This only applies to digital loopback mode.*/
	#define BIT_MSK__SRST__REG_RST_DIG_LOOPBACK_FIFO_N	0x01

/* Software Reset #2 Register*/
#define REG_ADDR__SRST2					(RX_TOP_REG | 0x0006)
	/* (ReadWrite, Bits 3) */
	/* reset Tx audio FIFO */
	#define BIT_MSK__SRST2__REG_RST_RX_FIFO_N		0x10

/* Software Reset #4 Register*/
#define REG_ADDR__SRST4					(RX_TOP_REG | 0x0008)
	/* (ReadWrite, Bits 1)*/
	#define BIT_MSK__SRST4__REG_RST_4CLK_CMC_RX_N		0x02

/* IO Output Enable Register */
#define REG_ADDR__IO_OEN				(RX_TOP_REG | 0x0009)
	/* (ReadWrite, Bits 7) */
	/* MUTE output enable */
	#define BIT_MSK__IO_OEN__REG_MUTE_OEN			0x80
	/* (ReadWrite, Bits 6) */
	/* SPDIF output enable */
	#define BIT_MSK__IO_OEN__REG_SPDIF_OEN			0x40
	/* (ReadWrite, Bits 5) */
	/* SD3 output enable */
	#define BIT_MSK__IO_OEN__REG_SD3_OEN			0x20
	/* (ReadWrite, Bits 4) */
	/* SD2 output enable */
	#define BIT_MSK__IO_OEN__REG_SD2_OEN			0x10
	/* (ReadWrite, Bits 3) */
	/* SD1 output enable */
	#define BIT_MSK__IO_OEN__REG_SD1_OEN			0x08
	/* (ReadWrite, Bits 2) */
	/* SD0 output enable */
	#define BIT_MSK__IO_OEN__REG_SD0_OEN			0x04
	/* (ReadWrite, Bits 1) */
	/* SCK output enable */
	#define BIT_MSK__IO_OEN__REG_SCK_OEN			0x02
	/* (ReadWrite, Bits 0) */
	/* I2C output enable */
	#define BIT_MSK__IO_OEN__REG_I2C_OEN			0x01

/* GPIO out ctrl Register */
#define REG_ADDR__GPIO_I_CTRL				(RX_TOP_REG | 0x000C)
	/* (ReadWrite, Bits 5:0)
	 * GIO I value */
	#define BIT_MSK__GPIO_I_CTRL__REG_GPIO_I		0x3F

/* GPIO OEN ctrl Register */
#define REG_ADDR__GPIO_OEN_CTRL				(RX_TOP_REG | 0x000D)
	/* (ReadWrite, Bits 5:0) */
	/* GIO output enable value: "0" enable output */
	#define BIT_MSK__GPIO_OEN_CTRL__REG_GPIO_OEN		0x3F

/* GPIO pe ctrl Register */
#define REG_ADDR__GPIO_PE_CTRL				(RX_TOP_REG | 0x000E)
	/* (ReadWrite, Bits 5:0) */
	/* GPIO output pull up enable.  "1" enable */
	#define BIT_MSK__GPIO_PE_CTRL__REG_GPIO_PE		0x3F

	/* GPIO4 output pull up enable.  "1" enable */
	#define BIT_MASK__GPIO_PE_CTRL__REG_GPIO4_PE	0x10

/* GPIO pu ctrl Register */
#define REG_ADDR__GPIO_PU_CTRL				(RX_TOP_REG | 0x000F)
	/* (ReadWrite, Bits 5:0) */
	/* GPIO output pull up value. */
	#define BIT_MSK__GPIO_PU_CTRL__REG_GPIO_PU		0x3F

	/* GPIO4 output pull up value */
	#define BIT_MASK__GPIO_PU_CTRL__REG_GPIO4_PU	0x10

/* Software Reset #4 Register*/
#define REG_ADDR__SRST4					(RX_TOP_REG | 0x0008)
	/* (ReadWrite, Bits 1)*/
	#define BIT_MSK__SRST4__REG_RST_4CLK_CMC_RX_N		0x02

/* GPIO input status Register */
#define REG_ADDR__GPIO_C_CTRL				(RX_TOP_REG | 0x0010)
	/* (ReadOnly, Bits 5:0) */
	/* Value of the incoming GPIO */
	#define BIT_MSK__GPIO_C_CTRL__GPIO_C			0x3F

/* Rx SPDIF select Register */
#define REG_ADDR__SPDIF_SEL				(RX_TOP_REG | 0x0011)
	/* (ReadWrite, Bits 3:2) */
	/* Select the input for Rx SPDIF input module
	"00" select the data from eARC - the normal function path.
	"01" select the data from ARC. Only for debugging.
		"reg_bypass_clk_extr" needs to be set to 1'b1 to get the clock
		from MCLK which has to be synchronous to SPDIF input from
		ARC mode.
	"10" select the data from tx_spdif in digital loopback mode to bypass
		analog PHY. Only used for debugging Tx and Rx digital
		logic together
	"11" select SPDIF input from SPDIF pin. It is used in Rx isolation mode
		to debug Rx digital logic only. Note that to make this mode,
		"reg_bypass_clk_extr" needs to be set to 1'b1 to get the clock
		from MCLK which is synchronous to SPDIF input from SPDIF pin.
	*/
	#define BIT_MSK__SPDIF_SEL__REG_RX_SPDIF_IN_SEL		0x0C
	/* (ReadWrite, Bits 5:4) */
	/* Select what to be sent to eARC Tx
	"00" input to eARC is from Tx SPDIF. This is the functional path.
	"01" input to eARC is from SPDIF pad. For debugging.
	"10" enable Core ISO mode (Audio PHY mode)
	"11" send 0 */
	#define BIT_MSK__SPDIF_SEL__REG_EARC_IN_SEL		0x30
	/* (ReadWrite, Bits 7:6)*/
	/* Select what to be sent to SPDIF pad
	"00" spdif output from eARC after sampling. For debugging.
	"01" raw spdif output from ARC. This is the functional path for
	legacy ARC.
	"10" output from Tx side spdif module. For debugging.
	"11" output from Rx side spdif module. This is the functional path
	for eARC. */
	#define BIT_MSK__SPDIF_SEL__REG_SPDIF_I_SEL		0xC0
	#define BIT_MSK__SPDIF_SEL__FUNC_PATH_LEGCY_ARC		0x40
	#define BIT_MSK__SPDIF_SEL__FUNC_PATH_EARC		0xC0

/* Timer CTRL Register*/
#define REG_ADDR__TIMER_CTRL				(RX_TOP_REG | 0x0012)
	/* (ReadWrite, Bits 1:0)*/
	/* Control for the timer:
		bit 0: 1: enable the timer; 0: disable the timer
		bit 1: selects the mode:  0 = one shot; 1 = continuous*/
	#define BIT_MSK__TIMER_CTRL__REG_TIMER_CONFIG		0x03
	#define BIT_MASK__TIMER_CTRL__REG_TIMER_CONFIG_ENABLE	0x01
	#define BIT_MASK__TIMER_CTRL__REG_TIMER_CONFIG_MODE	0x02

/* Timer value #0 Register*/
#define REG_ADDR__TIMER_VAL0				(RX_TOP_REG | 0x0013)
	/* (ReadWrite, Bits 7:0)*/
	/* data[7:0] is the value of timer. The timer counts down to 0
	 * and then issues an interrupt. In one-shot mode it then stops;
	 * in continuous mode it then re-loads the value and restarts.
	 * Note that reg_timer_data will not be loaded into the logic
	 * until TIMER_VAL1 is written.*/
	#define BIT_MSK__TIMER_VAL0__REG_TIMER_DATA_B7_B0	0xFF

/* Timer value #1 Register*/
#define REG_ADDR__TIMER_VAL1				(RX_TOP_REG | 0x0014)
	/* (ReadWrite, Bits 7:0)*/
	/* data[15:8] is the value of timer. The timer counts down to 0 and
	 * then issues an interrupt. In one-shot mode it then stops;
	 * in continuous mode it then re-loads the value and restarts.
	 * Note that reg_timer_data will not be loaded into the logic
	 * until TIMER_VAL1 is written.*/
	#define BIT_MSK__TIMER_VAL1__REG_TIMER_DATA_B15_B8	0xFF

/* Timer value #2 Register*/
#define REG_ADDR__TIMER_SCALE1				(RX_TOP_REG | 0x0015)
	/* (ReadWrite, Bits 7:0)*/
	/* scale[7:0] is the value of the scale timer. The main timer only
	 * decrements when the scale timer reaches zero. The scale timer is
	 * always continuous. This can be used to multiply the clock period
	 * of the timer clock by the scale value (+1).
	 * Note that reg_timer_scale will not be loaded into the logic
	 * until TIMER_SCALE2 is written.*/
	#define BIT_MSK__TIMER_SCALE1__REG_TIMER_SCALE_B7_B0	0xFF

/* Timer value #3 Register*/
#define REG_ADDR__TIMER_SCALE2				(RX_TOP_REG | 0x0016)
	/* (ReadWrite, Bits 7:0)*/
	/* scale[7:0] is the value of the scale timer. The main timer only
	 * decrements when the scale timer reaches zero. The scale timer is
	 * always continuous. This can be used to multiply the clock period
	 * of the timer clock by the scale value (+1).
	 * Note that reg_timer_scale will not be loaded into the logic
	 * until TIMER_SCALE2 is written.*/
	#define BIT_MSK__TIMER_SCALE2__REG_TIMER_SCALE_B15_B8	0xFF


/* I2C address for eARC Tx SW TPI */
#define REG_ADDR__I2C_ADDR0				(RX_TOP_REG | 0x0018)

/* I2C address for eARC Tx HW TPI */
#define REG_ADDR__I2C_ADDR1				(RX_TOP_REG | 0x0019)

/* Timer value #0 Register*/
#define REG_ADDR__FR_TIMER_VAL0				(RX_TOP_REG | 0x001C)
	/* (ReadWrite, Bits 7:0)*/
	/* status of the free running timer*/
	#define BIT_MSK__FR_TIMER_VAL0__FR_TIMER_DATA_B7_B0 0xFF

/* Timer value #1 Register*/
#define REG_ADDR__FR_TIMER_VAL1				(RX_TOP_REG | 0x001D)
	/* (ReadWrite, Bits 7:0)*/
	/* status of the free running timer*/
	#define BIT_MSK__FR_TIMER_VAL1__FR_TIMER_DATA_B15_B8 0xFF

/* Timer value #2 Register*/
#define REG_ADDR__FR_TIMER_VAL2				(RX_TOP_REG | 0x001E)
	/* (ReadWrite, Bits 7:0)*/
	/* status of the free running timer*/
	#define BIT_MSK__FR_TIMER_VAL2__FR_TIMER_DATA_B23_B16 0xFF

/* Timer value #3 Register*/
#define REG_ADDR__FR_TIMER_VAL3				(RX_TOP_REG | 0x001F)
	/* (ReadWrite, Bits 7:0)*/
	/* status of the free running timer*/
	#define BIT_MSK__FR_TIMER_VAL3__FR_TIMER_DATA_B31_B24 0xFF

/* Chip ID DDC Register*/
#define REG_ADDR__CHIP_ID_I2C				(RX_TOP_REG | 0x0024)
	/* (ReadWrite, Bits 7:0)*/
	/* bit #7: enable overwrite
	bit #6: "1" means Tx
	bits [3:0]: device ID with bits 7:4 of device ID hardcoded to 3*/
	#define BIT_MSK__CHIP_ID_I2C__REG_CHIP_ID_LI2C		0xFF
/* mclk io ctrl register */
#define REG_ADDR__MCLK_IO_CTRL				(RX_TOP_REG | 0x0025)
	/* (ReadWrite, Bits 0) */
	/* output enable for pac MCLK */
	#define BIT_MSK__MCLK_IO_CTRL__REG_MCLK_OEN		0x01

/* I2C address for eARC Tx core registers */
#define REG_ADDR__I2C_ADDR2				(RX_TOP_REG | 0x0029)

/* I2C address for eARC Rx core registers */
#define REG_ADDR__I2C_ADDR3				(RX_TOP_REG | 0x002A)

/* I2C address for PHY registers */
#define REG_ADDR__I2C_ADDR4				(RX_TOP_REG | 0x002B)

/* I2C Config Register */
#define REG_ADDR__I2C_CONFIG				(RX_TOP_REG | 0x002C)
	/* (ReadWrite, Bits 0) */
	/* Set to 1 to expose the other slave addresses
	 * beside slave address 0x60 */
	#define BIT_MSK__I2C_CONFIG__REG_EXPOSE_SLAVE_ADDR	0x01

/* Interrupt State Register */
#define REG_ADDR__TOP_INTR_STATE			(RX_TOP_REG | 0x0030)
	/* (ReadOnly, Bits 0) */
	/* Interrupt state bit value. It shows whether the interrupt output
	 * signal on INT pin is active or not.  1 indicates that at least one
	 * masked interrupt has been asserted. This bit can only be cleared by
	 * SW; either by unmasking all asserted interrupts or by clearing all
	 * asserted interrupts. */
	#define BIT_MSK__TOP_INTR_STATE__INTR_STATE_TOP		0x01
	/* (ReadOnly, Bits 1) */
	/* OR of all mask enabled interrupts from eARC TX */
	#define BIT_MSK__TOP_INTR_STATE__INTR_STATE_TX		0x02
	/* (ReadOnly, Bits 2) */
	/* OR of all mask enabled interrupts from eARC RX */
	#define BIT_MSK__TOP_INTR_STATE__INTR_STATE_RX		0x04

/* Interrupt Source #1 Register */
#define REG_ADDR__TOP_INTR1				(RX_TOP_REG | 0x0031)
	/* (ReadWrite, Bits 5)*/
	/* "1" means clock detect status from rx digital logic changed.
	 * For status check reg_rx_clk_detected*/
	#define BIT_MSK__TOP_INTR1__REG_TOP_INTR_STAT5		0x20
	/* (ReadWrite, Bits 4)*/
	/* "1" means rx_freq_zone changed. For status check RX_FREQ_ZONE
	 * bit 1*/
	#define BIT_MSK__TOP_INTR1__REG_TOP_INTR_STAT4		0x10
	/* (ReadWrite, Bits 3)*/
	/* "1" means lock detect status changed. For status check
	 * viola_status #1*/
	#define BIT_MSK__TOP_INTR1__REG_TOP_INTR_STAT3		0x08
	/* (ReadWrite, Bits 2)*/
	/* "1" means clock detect status from Viola PHY changed.
	 * For status check viola_status bit #0*/
	#define BIT_MSK__TOP_INTR1__REG_TOP_INTR_STAT2		0x04
	/* (ReadWrite, Bits 1)*/
	/* timer interrupt*/
	#define BIT_MSK__TOP_INTR1__REG_TOP_INTR_STAT1		0x02
	/* (ReadWrite, Bits 0)*/
	/* "1" software enabled interrupt from TOP_INT_CTRL bit #3*/
	#define BIT_MSK__TOP_INTR1__REG_TOP_INTR_STAT0		0x01

/* Interrupt Source #2 Register */
#define REG_ADDR__TOP_INTR2				(RX_TOP_REG | 0x0032)
	/* (ReadWrite, Bits 7) */
	/* "1" means GPIO1 changed from 1 to 0
	 * For status check reg. 0xC (GPIO_I_CTRL) */
	#define BIT_MSK__TOP_INTR2__REG_TOP_INTR_STAT13		0x80
	/* (ReadWrite, Bits 6) */
	/* "1" means GPIO0 changed from 1 to 0
	 * For status check reg. 0xC (GPIO_I_CTRL) */
	#define BIT_MSK__TOP_INTR2__REG_TOP_INTR_STAT12		0x40
	/* (ReadWrite, Bits 5) */
	/* "1" means GPIO5 changed from 0 to 1
	 * For status check reg. 0xC (GPIO_I_CTRL) */
	#define BIT_MSK__TOP_INTR2__REG_TOP_INTR_STAT11		0x20
	/* (ReadWrite, Bits 4) */
	/* "1" means GPIO4 changed from 0 to 1
	 * For status check reg. 0xC (GPIO_I_CTRL) */
	#define BIT_MSK__TOP_INTR2__REG_TOP_INTR_STAT10		0x10
	/* (ReadWrite, Bits 3) */
	/* "1" means GPIO3 changed from 0 to 1
	 * For status check reg. 0xC (GPIO_I_CTRL) */
	#define BIT_MSK__TOP_INTR2__REG_TOP_INTR_STAT9		0x08
	/* (ReadWrite, Bits 2) */
	/* "1" means GPIO2 changed from 0 to 1
	 * For status check reg. 0xC (GPIO_I_CTRL) */
	#define BIT_MSK__TOP_INTR2__REG_TOP_INTR_STAT8		0x04
	/* (ReadWrite, Bits 1) */
	/* "1" means GPIO1 changed from 0 to 1
	 * For status check reg. 0xC (GPIO_I_CTRL) */
	#define BIT_MSK__TOP_INTR2__REG_TOP_INTR_STAT7		0x02
	/* (ReadWrite, Bits 0) */
	/* "1" means GPIO0 changed from 0 to 1
	 * For status check reg. 0xC (GPIO_I_CTRL) */
	#define BIT_MSK__TOP_INTR2__REG_TOP_INTR_STAT6		0x01

	/* INT[9:6], INT[12], INT[13];
	 * 1 - enable; 0 - disable (default)
	 * GPIO0 to GPIO3 : 0 to 1 changes
	 * GPIO0 and GPIO1: 1 to 0 changes */
	#define BIT_MASK__TOP_INTR2__REG_INTR_GEN_GPIO		0xCF

/* Interrupt Source #3 Register */
#define REG_ADDR__TOP_INTR3				(RX_TOP_REG | 0x0033)
	/* (ReadWrite, Bits 3) */
	/* "1" means GPIO5 changed from 1 to 0
	 * For status check reg. 0xC (GPIO_I_CTRL) */
	#define BIT_MSK__TOP_INTR3__REG_TOP_INTR_STAT17		0x08
	/* (ReadWrite, Bits 2) */
	/* "1" means GPIO4 changed from 1 to 0
	 * For status check reg. 0xC (GPIO_I_CTRL) */
	#define BIT_MSK__TOP_INTR3__REG_TOP_INTR_STAT16		0x04
	/* (ReadWrite, Bits 1) */
	/* "1" means GPIO3 changed from 1 to 0
	 * For status check reg. 0xC (GPIO_I_CTRL) */
	#define BIT_MSK__TOP_INTR3__REG_TOP_INTR_STAT15		0x02
	/* (ReadWrite, Bits 0) */
	/* "1" means GPIO2 changed from 1 to 0
	 * For status check reg. 0xC (GPIO_I_CTRL) */
	#define BIT_MSK__TOP_INTR3__REG_TOP_INTR_STAT14		0x01

	/* GPIO2 and GPIO3: 1 to 0 changes*/
	#define BIT_MASK__TOP_INTR3__REG_INTR_GEN_GPIO		0x03

/* Interrupt #1 Mask Register */
#define REG_ADDR__TOP_INTR1_MASK			(RX_TOP_REG | 0x0034)
	#define BIT_MASK__TOP_INTR1__REG_TOP_INTR_MASK3		0x08

/* Interrupt #2 Mask Register */
#define REG_ADDR__TOP_INTR2_MASK			(RX_TOP_REG | 0x0035)
	/* (ReadWrite, Bits 7:0) */
	/* Enable for INT[13:6]: 1 - enable; 0 - disable (default) */
	#define BIT_MSK__TOP_INTR2_MASK__REG_INTR_MASK_B13_B6		0xFF

	/* Enable for INT[9:6], INT[12], INT[13];
	 * 1 - enable; 0 - disable (default)
	 * GPIO0 to GPIO3 : 0 to 1 changes
	 * GPIO0 and GPIO1: 1 to 0 changes */
	#define BIT_MASK__TOP_INTR2_MASK__REG_INTR_MASK_GEN_GPIO	0xCF

/* Interrupt #3 Mask Register */
#define REG_ADDR__TOP_INTR3_MASK			(RX_TOP_REG | 0x0036)
	/* (ReadWrite, Bits 3:0) */
	/* Enable for INT[15:14]: 1 - enable; 0 - disable (default) */
	#define BIT_MSK__TOP_INTR3_MASK__REG_INTR_MASK_B17_B14		0x0F

	/* GPIO2 and GPIO3: 1 to 0 changes*/
	#define BIT_MASK__TOP_INTR3_MASK__REG_INTR_MASK_GEN_GPIO	0x03

/* Interrupt Control Register */
#define REG_ADDR__TOP_INT_CTRL				(RX_TOP_REG | 0x0037)
	/* (ReadWrite, Bits 7) */
	/* Mask for the final interrupt before goes to pin IRQ */
	#define BIT_MSK__TOP_INT_CTRL__REG_GLOBAL_INTR_MASK	0x80

/* RX Freq Measure Value #0 Register */
#define REG_ADDR__RX_FREQ_VAL0				(RX_TOP_REG | 0x0038)
	/* (Read, Bits 7:0)*/
	/* rx measured clock frequency
	(Fmeas = 20MHz/rx_freq_win * rx_freq_val)*/
	#define BIT_MSK__RX_FREQ_VAL0__REG_RX_FREQ_VAL		0xFF

/* RX Freq Measure Value #1 Register */
#define REG_ADDR__RX_FREQ_VAL1				(RX_TOP_REG | 0x0039)
	/* (Read, Bits 15:8)*/
	/* rx measured clock frequency
	(Fmeas = 20MHz/rx_freq_win * rx_freq_val)*/
	#define BIT_MSK__RX_FREQ_VAL1__REG_RX_FREQ_VAL		0xFF
/* RX Frequency Zone Register*/
#define REG_ADDR__RX_FREQ_ZONE				(RX_TOP_REG | 0x003A)
	/* (ReadOnly, Bits 1)*/
	/* RX clock detected*/
	#define BIT_MSK__RX_FREQ_ZONE__REG_RX_CLK_DETECTED	0x02
	/* (ReadOnly, Bits 0)*/
	/* RX freqency zone: 0=Below Threshold, 1=Above Threshold.
	 *(Note: Threshold is set in rx_freq_thres register*/
	#define BIT_MSK__RX_FREQ_ZONE__REG_RX_FREQ_ZONE		0x01

/* RX Freq Zone Threshold #0 Register*/
#define REG_ADDR__RX_FREQ_THRES0			(RX_TOP_REG | 0x003B)
	/* (ReadWrite, Bits 7:0)*/
	/* Frequency zone threshold (default is134)*/
	#define BIT_MSK__RX_FREQ_THRES0__REG_RX_FREQ_THRES_B7_B0	0xFF

/* RX Freq Zone Threshold #1 Register*/
#define REG_ADDR__RX_FREQ_THRES1			(RX_TOP_REG | 0x003C)
	/* (ReadWrite, Bits 7:0)*/
	/* Frequency zone threshold (default is 134)*/
	#define BIT_MSK__RX_FREQ_THRES1__REG_RX_FREQ_THRES_B15_B8	0xFF

/* RX Clock Detect Low Threshold #0 Register*/
#define REG_ADDR__RX_CLKDET_LOW_THRES0			(RX_TOP_REG | 0x003D)
	/* (ReadWrite, Bits 7:0)*/
	/* Clock detect low threshold (default 1.875 MHz)*/
#define BIT_MSK__RX_CLKDET_LOW_THRES0__REG_RX_CLKDET_LOW_THRES_B7_B0	0xFF

/* RX Clock Detect Low Threshold #1 Register*/
#define REG_ADDR__RX_CLKDET_LOW_THRES1			(RX_TOP_REG | 0x003E)
	/* (ReadWrite, Bits 7:0)*/
	/* Clock detect low threshold (default 1.875 MHz)*/
#define BIT_MSK__RX_CLKDET_LOW_THRES1__REG_RX_CLKDET_LOW_THRES_B15_B8	0xFF

/* RX Clock Detect High Threshold #0 Register*/
#define REG_ADDR__RX_CLKDET_HIGH_THRES0			(RX_TOP_REG | 0x003F)
	/* (ReadWrite, Bits 7:0)*/
	/* Clock detect high threshold (default 120MHz)*/
#define BIT_MSK__RX_CLKDET_HIGH_THRES0__REG_RX_CLKDET_HIGH_THRES_B7_B0	0xFF

/* RX Clock Detect High Threshold #1 Register*/
#define REG_ADDR__RX_CLKDET_HIGH_THRES1			(RX_TOP_REG | 0x0040)
	/* (ReadWrite, Bits 7:0)*/
	/* clock detect high threshold (default 120MHz)*/
#define BIT_MSK__RX_CLKDET_HIGH_THRES1__REG_RX_CLKDET_HIGH_THRES_B15_B8	0xFF

/* RX Freq Zone Window #0 Register */
#define REG_ADDR__RX_FREQ_WIN0				(RX_TOP_REG | 0x0041)
	/* (Read, Bits 7:0)*/
	/* Frequency zone window (default is 128) */
	#define BIT_MSK__RX_FREQ_WIN0__REG_RX_FREQ_WIN		0xFF

/* RX Freq Zone Window #1 Register */
#define REG_ADDR__RX_FREQ_WIN1				(RX_TOP_REG | 0x0042)
	/* (Read, Bits 15:8)*/
	/* Frequency zone window (default is 128) */
	#define BIT_MSK__RX_FREQ_WIN1__REG_RX_FREQ_WIN		0xFF

/* Kill Switch Reg. */
#define REG_ADDR__SWHRST				(RX_TOP_REG | 0x00FF)


/*****************  RX EARC (6E00-6EFF) *********************/

/* Audio Out I2S Control #2 Register */
#define REG_ADDR__RX_I2S_CTRL2				(RX_EARC_REG | 0x0000)
	/* (ReadWrite, Bits 7)*/
	/* SD3 input enable
	0 - SD3 is disabled (default);
	1 - SD3 is enabled.*/
	#define BIT_MSK__RX_I2S_CTRL2__REG_I2S_RX_SD3_EN 0x80
	/* (ReadWrite, Bits 6)*/
	/* SD2 input enable
	0 - SD2 is disabled (default);
	1 - SD2 is enabled.*/
	#define BIT_MSK__RX_I2S_CTRL2__REG_I2S_RX_SD2_EN 0x40
	/* (ReadWrite, Bits 5)*/
	/* SD1 input enable
	0 - SD1 is disabled (default);
	1 - SD1 is enabled.*/
	#define BIT_MSK__RX_I2S_CTRL2__REG_I2S_RX_SD1_EN 0x20
	/* (ReadWrite, Bits 4)*/
	/* SD0 input enable
	0 - SD0 is disabled (default);
	1 - SD0 is enabled.*/
	#define BIT_MSK__RX_I2S_CTRL2__REG_I2S_RX_SD0_EN 0x10
	/* (ReadWrite, Bits 1)*/
	/* 0 - 24 bits of real only data sent via I2S (default)
	1 - 28 bits of data sent via I2S.
		The VUCP bits are sent as well.*/
	#define BIT_MSK__RX_I2S_CTRL2__REG_I2S_RX_VUCP 0x02
	/* (ReadWrite, Bits 0)*/
	/* I2S data pass select:
	0 - pass whatever data is in SPDIF packet.
	1 - Only pass data from SPDIF packet
		if it is recognized as PCM data.
		If non-PCM data detected; send 0 as the data. (default)*/
	#define BIT_MSK__RX_I2S_CTRL2__REG_I2S_RX_PCM 0x01


/* Audio Out I2S SD Map Register*/
#define REG_ADDR__RX_I2S_MAP				(RX_EARC_REG | 0x0001)
	/* (ReadWrite, Bits 7:6)*/
	/* Map Data 1 of the 4 FIFO audio streams into the SD3:
	00 - map audio stream from FIFO #0 into the SD3;
	01 - map audio stream from FIFO #1 into the SD3;
	10 - map audio stream from FIFO #2 into the SD3;
	11 - map audio stream from FIFO #3 into the SD3 (default);*/
	#define BIT_MSK__RX_I2S_MAP__REG_I2S_RX_SD3_MAP		0xC0
	/* (ReadWrite, Bits 5:4)*/
	/* Map Data 1 of the 4 FIFO audio streams into the SD2:
	00 - map audio stream from FIFO #0 into the SD2;
	01 - map audio stream from FIFO #1 into the SD2;
	10 - map audio stream from FIFO #2 into the SD2 (default);
	11 - map audio stream from FIFO #3 into the SD2;*/
	#define BIT_MSK__RX_I2S_MAP__REG_I2S_RX_SD2_MAP		0x30
	/* (ReadWrite, Bits 3:2)*/
	/* Map Data 1 of the 4 FIFO audio streams into the SD1:
	00 - map audio stream from FIFO #0 into the SD1;
	01 - map audio stream from FIFO #1 into the SD1 (default);
	10 - map audio stream from FIFO #2 into the SD1;
	11 - map audio stream from FIFO #3 into the SD1;*/
	#define BIT_MSK__RX_I2S_MAP__REG_I2S_RX_SD1_MAP		0x0C
	/* (ReadWrite, Bits 1:0)*/
	/* Map Data 1 of the 4 FIFO audio streams into the SD0:
	00 - map audio stream from FIFO #0 into the SD0 (default);
	01 - map audio stream from FIFO #1 into the SD0;
	10 - map audio stream from FIFO #2 into the SD0;
	11 - map audio stream from FIFO #3 into the SD0.*/
	#define BIT_MSK__RX_I2S_MAP__REG_I2S_RX_SD0_MAP		0x03

/* Audio out I2S Control #1 Register */
#define REG_ADDR__RX_I2S_CTRL1				(RX_EARC_REG | 0x0002)

/* Audio out SPDIF Control 1 Register */
#define REG_ADDR__RX_AUD_OUT_SPDIF_CH_CTRL_1		(RX_EARC_REG | 0x0003)
	/* (ReadWrite, Bits 7)*/
	/* fix parity bit for SPDIF out*/
	#define BIT_MSK__RX_AUD_OUT_SPDIF_CTRL_1__REG_RX_FIX_P	0x80
	/* (ReadWrite, Bits 6)*/
	/* 1: override v bit for SPDIF and I2S output (if VUCP enabled)*/
	#define BIT_MSK__RX_AUD_OUT_SPDIF_CTRL_1__REG_VBIT_OVERRIDE 0x40
	/* (ReadWrite, Bits 5)*/
	/* the v bit used to override output (SPDIF or I2S)*/
	#define BIT_MSK__RX_AUD_OUT_SPDIF_CTRL_1__REG_VBIT	0x20
	/* (ReadWrite, Bits 4)*/
	/* 0 = do not pass spdif type of errors (Parity or VUC left/right
	 * sub-frame mismatch); i.e. conceal audio errors by repeating
	 * last good sample.
	1 = pass all audio data; regardless of errors (default)*/
	#define BIT_MSK__RX_AUD_OUT_SPDIF_CTRL_1__REG_RX_PASS_SPDIF_ERR	0x10
	/* (ReadWrite, Bits 3)*/
	/* write "1" to updae c bit configurable registers*/
	#define BIT_MSK__RX_AUD_OUT_SPDIF_CTRL_1__REG_CS_UPDATE_EN	0x08
	/* (ReadWrite, Bits 2)*/
	/* 1: override c bit for SPDIF and I2S output (if VUCP enabled)*/
	#define BIT_MSK__RX_AUD_OUT_SPDIF_CTRL_1__REG_CBIT_OVERRIDE	0x04
	/* (ReadWrite, Bits 1)*/
	/* SPDIF flat line enable.
	0 = SPDIF output is always produces valid bi-phase mark encoded
	data even during flat line (default).
	1 = SPDIF output is grounded if flat line is detected*/
	#define BIT_MSK__RX_AUD_OUT_SPDIF_CTRL_1__REG_RX_SPDIF_MODE	0x02
	/* (ReadWrite, Bits 0)*/
	/* 0 - SPDIF output disabled (defaut);
	1 - SPDIF output stream is enable*/
	#define BIT_MSK__RX_AUD_OUT_SPDIF_CTRL_1__REG_RX_SPDIF_EN	0x01

/* RX Audio Channel Control Register */
#define REG_ADDR__RX_AUD_OUT_CH_CTRL			(RX_EARC_REG | 0x0006)
	/* (ReadWrite, Bits 0)*/
	/* 1-the input stream has multiple channel (Layout B)
	 * 0-the input stream has two channels (Layout A)
	 * don't care this bit if input is Layout C
	 * This bit is used at eARC Rx side when input is multi-channel
	 * and output is 2-ch SPDIF.
	 * By enabling this bit the "C bit" from all channels of input stream
	 * will be buffered and reallocated to 2-ch SPDIF output during
	 * the channel decimation.
	 */
	#define BIT_MSK__RX_AUD_OUT_CH_CTRL__REG_MULTI_CH_IN	0x01
	/* (ReadWrite, Bits 1)*/
	/* 1- enable the channel decimator; this means only channel 0 and 1
	 *	will be output and other channels will be dropped;
	 * 0- disable the decimation function
	 */
	#define BIT_MSK__RX_AUD_OUT_CH_CTRL__REG_DECI_EN	0x02
	/* (ReadWrite, Bits 1)*/
	/* 1- the output stream has multiple channels
	 * 0- the output stream only has two channels
	 */
	#define BIT_MSK__RX_AUD_OUT_CH_CTRL__REG_MULTI_CH_OUT	0x04
	/*
	output clock divider. This register is used for dividing eARC clock
	 to generate output clock.
		00-no div
		01-div by 2
		10-div by 4
		11-div by 8
	reference configuration:
		I2S output: div by 4
		DSD output: div by 8
		SPDIF output: no-div (if decimator is enabled
		 should use div-by-4)
	*/
	#define BIT_MSK__RX_AUD_OUT_CH_CTRL__REG_OUT_DIV	0x30
	#define BIT_MSK__RX_AUD_OUT_CH_CTRL__REG_OUT_DIV_BY1	0x00
	#define BIT_MSK__RX_AUD_OUT_CH_CTRL__REG_OUT_DIV_BY2	0x10
	#define BIT_MSK__RX_AUD_OUT_CH_CTRL__REG_OUT_DIV_BY4	0x20
	#define BIT_MSK__RX_AUD_OUT_CH_CTRL__REG_OUT_DIV_BY8	0x30

/* Audio Out Channel Status #1 Register*/
#define REG_ADDR__AUD_CHST0				(RX_EARC_REG | 0x0007)
	/* (ReadWrite, Bits 7:0)*/
	/* The information in this register is send in Channel Status field
	 * across HDMI
	 * link Channel Status byte #0.
	 * Please refer to SPDIF spec for detailed description.*/
	#define BIT_MSK__AUD_CHST0__REG_CBIT0			0xFF

/* Audio Out Channel Status #2 Register*/
#define REG_ADDR__AUD_CHST1				(RX_EARC_REG | 0x0008)
	/* (ReadWrite, Bits 7:0)*/
	/* The information in this register is send in Channel Status field
	 * across HDMI link.
	 * Category code.
	 * Please refer to SPDIF spec for detailed description.*/
	#define BIT_MSK__AUD_CHST1__REG_CBIT1			0xFF

/* Audio Out Channel Status #3 Register*/
#define REG_ADDR__I2S_CHST2				(RX_EARC_REG | 0x0009)
	/* (ReadWrite, Bits 7:4)*/
	/* The information in this register is send in Channel Status field
	 * across HDMI link
	 * Channel Number.
	 * Please refer to SPDIF spec for detailed description.*/
	#define BIT_MSK__I2S_CHST2__REG_CBIT2B			0xF0
	/* (ReadWrite, Bits 3:0)*/
	/* The information in this register is send in Channel Status field
	 * across HDMI link.
	 * Source number.
	 * Please refer to SPDIF spec for detailed description.*/
	#define BIT_MSK__I2S_CHST2__REG_CBIT2A			0x0F

/* Audio Out Channel Status #4 Register*/
#define REG_ADDR__AUD_CHST3				(RX_EARC_REG | 0x000A)
	/* (ReadWrite, Bits 7:4)*/
	/* [5:4] Clock accuracy
[7:6] Sampling frequency extension with sampling frequency bits 24 to 27
hbra_on; aud_sample_freq[5:0];  Audio Rate
1'b0; 2'b00; 4'b0100; 22.05 kHz (n/a)
1'b0; 2'b00; 4'b0000; 44.1 kHz
1'b0; 2'b00; 4'b1000; 88.2 kHz
1'b0; 2'b00; 4'b1100; 176.4 kHz
1'b0; 2'b00; 4'b1101; 352.8 kHz
1'b0; 2'b10; 4'b1101; 705.6 kHz
1'b0; 2'b01; 4'b1101; 1411.2 kHz (n/a)
1'b0; 2'b00; 4'b0110; 24 kHz (n/a)
1'b0; 2'b00; 4'b0010; 48 kHz
1'b0; 2'b00; 4'b1010; 96 kHz
1'b0; 2'b00; 4'b1110; 192 kHz
1'b0; 2'b00; 4'b0101; 384 kHz
1'b0; 2'b00; 4'b1001; 768 kHz
1'b0; 2'b01; 4'b0101; 1536 kHz (n/a)
1'b0; 2'b00; 4'b0011; 32 kHz
1'b0; 2'b00; 4'b1011; 64 kHz
1'b0; 2'b10; 4'b1011; 128 kHz
1'b0; 2'b01; 4'b1011; 256 kHz
1'b0; 2'b11; 4'b1011; 512 kHz
1'b0; 2'b11; 4'b0101; 1024 kHz (n/a)
1'b1; 2'b00; 4'b1100; HBRA 176.4 kHz
1'b1; 2'b00; 4'b1101; HBRA 352.8 kHz
1'b1; 2'b10; 4'b1101; HBRA 705.6 kHz
1'b1; 2'b01; 4'b1101; HBRA 1411.2 kHz
1'b1; 2'b00; 4'b1110; HBRA 192 kHz
1'b1; 2'b00; 4'b0101; HBRA 384 kHz
1'b1; 2'b00; 4'b1001; HBRA 768 kHz
1'b1; 2'b01; 4'b0101; HBRA 1536 kHz
1'b1; 2'b10; 4'b1011; HBRA 128 kHz
1'b1; 2'b01; 4'b1011; HBRA 256 kHz
1'b1; 2'b11; 4'b1011; HBRA 512 kHz
1'b1; 2'b11; 4'b0101; HBRA 1024 kHz*/
	#define BIT_MSK__AUD_CHST3__REG_CBIT3B			0xF0
	/* (ReadWrite, Bits 3:0)*/
	/* The information in this register is send in Channel Status
	 * field across HDMI link. Please refer to SPDIF spec for
	 * detailed description.Sampling frequency set by software
	 * (inserted into I2S stream or into SPDIF if fs_overrride is enabled).
	 * These bits correspond to the Channel Status bits 24; 25; 26;27;
	 * where bit 24 = LSB and 27 = MSB:
27 26 25 24
"0   1   0   0" - Fs =   22.05 kHz
"0   0   0   0" - Fs =   44.1 kHz
"1   0   0   0" - Fs =   88.2 kHz
"1   1   0   0" - Fs = 176.4 kHz
"0   1   1   0" - Fs =   24 kHz
"0   0   1   0" - Fs =   48 kHz
"1   0   1   0" - Fs =   96 kHz
"1   1   1   0" - Fs = 192 kHz
"0   0   1   1" - Fs =   32 kHz
"1  0  0  1" -  Fs =  768 kHz*

All other frequencies (default)*/
	#define BIT_MSK__AUD_CHST3__REG_CBIT3A 0x0F

/* Audio Out Channel Status #5 Register*/
#define REG_ADDR__AUD_CHST4				(RX_EARC_REG | 0x000B)
	/* (ReadWrite, Bits 7:4)*/
	/* Original Fs*/
	#define BIT_MSK__AUD_CHST4__REG_CBIT4B			0xF0
	/* (ReadWrite, Bits 3:0)*/
	/* Reg_cbit[32] : The information in this register is send in
	 * Channel Status field
"1011" - 24 bits (default);
"1001" - 23 bits;
"0101" - 22 bits;
"1101" - 21 bits;
"0011" - 20 bits
"1010" - 20 bits
"1000" - 19 bits
"0100" - 18 bits
"1100" - 17 bits
"0010" - 16 bits
"0000" - not indicated*/
	#define BIT_MSK__AUD_CHST4__REG_CBIT4A			0x0F

/* Audio Out Channel Status #6 Register*/
#define REG_ADDR__AUD_CHST5				(RX_EARC_REG | 0x000C)
	/* (ReadWrite, Bits 7:0)*/
	/* Channel Status bits 47:40*/
	#define BIT_MSK__AUD_CHST5__REG_CBIT5 0xFF

/* Audio Out Channel Status #7 Register*/
#define REG_ADDR__AUD_CHST6				(RX_EARC_REG | 0x000D)
	/* (ReadWrite, Bits 7:0)*/
	/* Channel Status bits 55:48*/
	#define BIT_MSK__AUD_CHST6__REG_CBIT6			0xFF

/* Audio Out CS Extra Byte#1 Register*/
#define REG_ADDR__AUD_CS_EXTRA1				(RX_EARC_REG | 0x000E)
	/* (ReadWrite, Bits 7:0)*/
	/* Send as extra Channel Status byte #1.
Location within 24 bytes (last 18) is indicated by I2S_CS_PTR
bit #0 - if "1" the stream is legacy ARC; else eARC
bit #1 - if "1" then stream is HBRA (v bit must be set also)*/
	#define BIT_MSK__AUD_CS_EXTRA1__REG_CS_EXTR1		0xFF

/* Audio Out CS Extra Byte#2 Register*/
#define REG_ADDR__AUD_CS_EXTRA2				(RX_EARC_REG | 0x000F)
	/* (ReadWrite, Bits 7:0)*/
	/* Send as extra Channel Status byte #2
Location within 24 bytes (last 18) is indicated by I2S_CS_PTR*/
	#define BIT_MSK__AUD_CS_EXTRA2__REG_CS_EXTR2		0xFF


/* Audio In SPDIF Channel Status #1 Register */
#define REG_ADDR__RX_CHST0				(RX_EARC_REG | 0x0019)
	#define BIT_MASK__RX_CHST0__MULTI_CHANNEL_LPCM		0x20
/* Audio In SPDIF Channel Status #2 Register */		/* 48-55 */
#define REG_ADDR__RX_CHST1				(RX_EARC_REG | 0x001A)
/* Audio In SPDIF Channel Status #3 Register */		/* 48-55 */
#define REG_ADDR__RX_CHST2				(RX_EARC_REG | 0x001B)
/* Audio In SPDIF Channel Status #4 Register */		/* 48-55 */
#define REG_ADDR__RX_CHST3				(RX_EARC_REG | 0x001C)
/* Audio In SPDIF Channel Status #5 Register */		/* 48-55 */
#define REG_ADDR__RX_CHST4				(RX_EARC_REG | 0x001D)
/* Audio In SPDIF Channel Status #6 Register */		/* 48-55 */
#define REG_ADDR__RX_CHST5				(RX_EARC_REG | 0x001E)
	#define BIT_MASK__RX_CHST5__EARC_LAYOUT			0x08
/* Audio In SPDIF Channel Status #7 Register */		/* 48-55 */
#define REG_ADDR__RX_CHST6				(RX_EARC_REG | 0x001F)

/* RX CS Extra Byte#1 Register*/
#define REG_ADDR__RX_CS_EXTRA1				(RX_EARC_REG | 0x0020)
	#define BIT_MASK__RX_CS_EXTRA1__EARC_CA0_CA7		0xFF
	#define BIT_MASK__RX_CS_EXTRA1__EARC_CA0_CA7_SHIFT	0x00

/* RX CS Extra Byte#2 Register*/
#define REG_ADDR__RX_CS_EXTRA2				(RX_EARC_REG | 0x0021)
#define BIT_MASK__RX_CS_EXTRA2__EARC_LFE			0x03
#define BIT_MASK__RX_CS_EXTRA2__EARC_MUTE			0x04
#define BIT_MASK__RX_CS_EXTRA2__EARC_MUTE_SHIFT			0x02
#define BIT_MASK__RX_CS_EXTRA2__EARC_LSV			0x78
#define BIT_MASK__RX_CS_EXTRA2__EARC_LSV_SHIFT			0x03
#define BIT_MASK__RX_CS_EXTRA2__EARC_DM_INH			0x80
#define BIT_MASK__RX_CS_EXTRA2__EARC_DM_INH_SHIFT		0x07

/* RX CS Extra Pointer Register*/
#define REG_ADDR__I2S_CS_PTR				(RX_EARC_REG | 0x0022)
	/* (ReadWrite, Bits 4:0)*/
	/* tells starting byte of inserting 2 extra channel status bytes*/
	#define BIT_MSK__I2S_CS_PTR__REG_CS_EXTRA_PTR		0x1F
	#define BIT_MASK__I2S_CS_PTR__REG_CS_EXTRA_PTR_OFFSET	0x11

/* Audio In I2S Control Register */
#define REG_ADDR__RX_AUDRX_CTRL				(RX_EARC_REG | 0x0023)

/* Tx SPDIF B Sync Ctrl Register */
#define REG_ADDR__BSYNC_CTRL				(RX_EARC_REG | 0x0027)

/* RX Audio In SPDIF Channel Status Control Register*/
#define REG_ADDR__RX_BPM_CS_CTRL			(RX_EARC_REG | 0x002C)
	/* (ReadWrite, Bits 7)*/
	/* 1: if this register is written by software the channel
	 status registers will be reset and shown again after a whole
	  block (192 frames) is received*/
	#define BIT_MSK__RX_BPM_CS_CTRL__REG_CLR_CS		0x80
	/* (ReadOnly, Bits 6)*/
	/* 1:audio in left and right Channel Stauts are not identical*/
	#define BIT_MSK__RX_BPM_CS_CTRL__AUD_CS_LR_MISMATCH	0x40
	/* (ReadWrite, Bits 5)*/
	/* 1: use left Channel Status as output*/
	#define BIT_MSK__RX_BPM_CS_CTRL__REG_USE_L_CS		0x20
	/* (ReadWrite, Bits 4)*/
	/* 1: enable left and right Channel Status comparing; if this bit
	 is enabled the Channel Status is updated only if left and right
	 channel are the same; related interrupt will be set if left and
	 right channel are not identical.*/
	#define BIT_MSK__RX_BPM_CS_CTRL__REG_LR_MATCH_EN	0x10
	/* (ReadWrite, Bits 3:0)*/
	/* B block max number. This is reset parity error counter once every
	 N blocks where N is value of reset counter. If set to 0 then parity
	 counter will keep accumulating until it hit threshold value.*/
	#define BIT_MSK__RX_BPM_CS_CTRL__REG_B_CNT_MAX	0x0F

/* Rx CS Debug1 Register */
#define REG_ADDR__RX_CS_DEBUG1				(RX_EARC_REG | 0x002D)

/* Rx CS Debug2 Register */
#define REG_ADDR__RX_CS_DEBUG2				(RX_EARC_REG | 0x002E)

/* Rx CS Debug3 Register */
#define REG_ADDR__RX_CS_DEBUG3				(RX_EARC_REG | 0x002F)

/* RX MUTE_CTRL Register 1 */
#define REG_ADDR__MUTE_CTRL				(RX_EARC_REG | 0x0030)
	/* (ReadWrite, Bits 0)*/
	/* enable mute data.
	On Tx will zero data send over eARC
	on Rx will result in assertion of MUTE_OUT and zero out data */
	#define BIT_MSK__MUTE_CTRL__REG_MUTE_EN			0x01

/* RX MUTE_CTRL Register 3 */
#define REG_ADDR__MUTE_EXP_EN_1				(RX_EARC_REG | 0x0032)
	/* (ReadWrite, Bits 7:0)*/
	/* selects which exceptions to enable
	 *(if triget then data will be "0" and MUTE_OUT set on Rx)
	"15" to enable mute_out
	"8" to enable common mode channel rx status register (0xD0) bit[4]
	 being set
	"9" - to enable mute on v bit set in none pcm mode
	"10" - to enable mute on getting mute channel status bit
	Other bits are reserved*/
	/* "15" to enable mute_out */
	#define BIT_MSK__MUTE_EXP_EN_1__REG_MUTE_EN		0x80
	/* "10" - to enable mute on getting mute channel status bit */
	#define BIT_MSK__MUTE_EXP_EN_1__MUTE_EN_ON_CS_BIT	0x04
	#define BIT_MSK__MUTE_EXP_EN_1__REG_MUTE_EXP_EN_B15_B8	0xFF

/* Rx CS Mute Pointer Register*/
#define REG_ADDR__RX_CS_MUTE_PTR			(RX_EARC_REG | 0x0035)
	/* (ReadWrite, Bits 7:0)*/
	/* offset pointer within Channel Status array for mute pattern*/
	#define BIT_MSK__RX_CS_MUTE_PTR__REG_RX_CS_MUTE_PTR	0xFF
	/* Mute : Bit 146 in the channel status
	 * So give offset to beginning of the byte boundary (146) and
	 * Give Mask in RX_CS_MUTE_MASK and RX_CS_MUTE_PATTERN registers */
	#define SII_CS_MUTE_POSITION				0x90

/* Rx CS Mute Mask Register*/
#define REG_ADDR__RX_CS_MUTE_MASK			(RX_EARC_REG | 0x0036)
	/* (ReadWrite, Bits 7:0)*/
	/* mask for the mute pattern in Channel Status array.
	 * Only bits set to "1" are used*/
	#define BIT_MSK__RX_CS_MUTE_MASK__REG_RX_CS_MUTE_MASK	0xFF
	#define SII_CS_BIT_146					0x04

/* Rx CS Mute Pattern Register*/
#define REG_ADDR__RX_CS_MUTE_PATTERN			(RX_EARC_REG | 0x0037)
	/* (ReadWrite, Bits 7:0)*/
	/* pattern to compare against to see if mute is on.
	 * Only bits set by mask will be compared*/
	#define BIT_MSK__RX_CS_MUTE_PATTERN__REG_RX_CS_MUTE_PATTERN 0xFF

/* MCLK OUT ctrl Register */
#define REG_ADDR__MCLK_CTRL				(RX_EARC_REG | 0x003C)
	/* (ReadWrite, Bits 6) */
	/* if "1" then use above overwrite value to select mclk_out;
	 * otherwise use value from CS:
		Fs (kHz) ---> MCLK out
		32; 44.1; 48 --> 512Fs=4*128*Fs
		88.2; 96 -->  212Fs=2*128*Fs
		176.2; 192 --> 128Fs=1*128*Fs
	*/
	#define BIT_MSK__MCLK_CTRL__REG_OVR_EN			0x40
	/* (ReadWrite, Bits 5:0) */
	/* overwrite sampling frequency:
		"00 0   0   1   1" - Fs = 32 kHz
		"00 0   0   0   0" - Fs = 44.1 kHz
		"00 0   0   1   0" - Fs = 48 kHz
		"00 1   0   0   0" - Fs = 88.2 kHz
		"00 1   0   1   0" - Fs = 96 kHz
		"00 1   1   0   0" - Fs = 176.4 kHz
		"00 1   1   1   0" - Fs = 192 kHz
	*/
	#define BIT_MSK__MCLK_CTRL__REG_DIV_SEL			0x3F

/* PKT TOP RX12 Register*/
#define REG_ADDR__PKT_TOP_RX12				(RX_EARC_REG | 0x004C)
	/* (ReadWrite, Bits 0)*/
	/* clear ecc error counter*/
	#define BIT_MSK__PKT_TOP_RX12__REG_CMC_RX_PKT_ECC_ERR_CLR 0x01

/* PKT TOP RX13 Register*/
#define REG_ADDR__PKT_TOP_RX13				(RX_EARC_REG | 0x004D)
	/* (ReadOnly, Bits 7:0)*/
	/* count for ecc errors*/
	#define BIT_MSK__PKT_TOP_RX13__REG_CMC_RX_PKT_ECC_ERR_CNT_B7_B0 0xFF

/* PKT TOP RX14 Register*/
#define REG_ADDR__PKT_TOP_RX14				(RX_EARC_REG | 0x004E)
	/* (ReadOnly, Bits 7:0)*/
	/* */
	#define BIT_MSK__PKT_TOP_RX14__REG_CMC_RX_PKT_ECC_ERR_CNT_B15_B8 0xFF

/* PKT TOP RX15 Register*/
#define REG_ADDR__PKT_TOP_RX15				(RX_EARC_REG | 0x0055)
	/* (ReadWrite, Bits 0)*/
	/* clear ecc error counter*/
	#define BIT_MSK__PKT_TOP_RX15__REG_CMC_RX_PKT_ECC_FIX_CLR 0x01

/* PKT TOP RX16 Register*/
#define REG_ADDR__PKT_TOP_RX16				(RX_EARC_REG | 0x0056)
	/* (ReadOnly, Bits 7:0)*/
	/* low 8 bits of count for fixed ecc errors*/
	#define BIT_MSK__PKT_TOP_RX16__REG_CMC_RX_PKT_ECC_FIX_CNT_B7_B0 0xFF

/* PKT TOP RX17 Register*/
#define REG_ADDR__PKT_TOP_RX17				(RX_EARC_REG | 0x0057)
	/* (ReadOnly, Bits 7:0)*/
	/* high 8 bits of count for fixed ecc errors*/
	#define BIT_MSK__PKT_TOP_RX17__REG_CMC_RX_PKT_ECC_FIX_CNT_B15_B8 0xFF

/* DISC_DISCONN_RX4  Register*/
#define REG_ADDR__DISC_DISCONN_RX4			(RX_EARC_REG | 0x0064)
	/* (ReadWrite, Bits 7)*/
	/* software control override enalbe for COMMA sequence timeout;
	 * when it's high; reg_rx_timeout_sw_ctrl is used to control timeout
	 * without considering reg_rx_timeout*/
	#define BIT_MSK__DISC_DISCONN_RX4__REG_RX_TIMEOUT_SW_CTRL_EN	0x80
	/* (ReadWrite, Bits 6)*/
	/* software control override value for COMMA sequence timeout;
	 *  0-no timeout;
	 *  1- timeout*/
	#define BIT_MSK__DISC_DISCONN_RX4__REG_RX_TIMEOUT_SW_CTRL	0x40

/* DISC_DISCONN_RX7  Register*/
#define REG_ADDR__DISC_DISCONN_RX7			(RX_EARC_REG | 0x0067)
	/* (ReadWrite, Bits 7:4)*/
	/* Comma OFF interval; unit is ms*/
	#define BIT_MSK__DISC_DISCONN_RX7__REG_RX_COMMA_ON_B3_B0	0xF0
	/* (ReadWrite, Bits 3:0)*/
	/* Comma ON interval; unit is ms*/
	#define BIT_MSK__DISC_DISCONN_RX7__REG_RX_COMMA_OFF_B3_B0	0x0F

/* DISC_DISCONN_RX8  Register */
#define REG_ADDR__DISC_DISCONN_RX8			(RX_EARC_REG | 0x0068)
	/* (ReadOnly, Bits 7) */
	/* "1" means rx EARC connection is established;
	 * "0" means rx exits from EARC state and connection is lost */
	#define BIT_MSK__DISC_DISCONN_RX8__REG_RX_EARC_CONNECTION_STATUS 0x80
	/* (ReadOnly, Bits 2:0)*/
	/* state of discovery and disconnect FSM*/
	#define BIT_MSK__DISC_DISCONN_RX8__REG_RX_DISC_DISCONN_STATE_B2_B0 0x07

/* DISC_DISCONN_RX9  Register*/
#define REG_ADDR__DISC_DISCONN_RX9			(RX_EARC_REG | 0x0069)
	/* (ReadWrite, Bits 7)*/
	/* state mode select; 1-new mode; no IDLE2; 0- old mode with IDLE2*/
	#define BIT_MSK__DISC_DISCONN_RX9__REG_RX_DISC_CON_ST_NO_IDLE2	0x80
	/* (ReadWrite, Bits 6)*/
	/* 1-eARC state can be disabled by HPD going low;
	 * 0- eARC state can not be disabled by HDP going low*/
	#define BIT_MSK__DISC_DISCONN_RX9__REG_RX_HPD_DISABLE_EARC_EN 0x40
	/* (ReadWrite, Bits 3)*/
	/* state override enable*/
	#define BIT_MSK__DISC_DISCONN_RX9__REG_RX_DISC_CONN_ST_OVERRIDE_EN 0x08
	/* (ReadWrite, Bits 2:0)*/
	/* state override value*/
	#define BIT_MSK__DISC_DISCONN_RX9__REG_RX_DISC_DISCON_ST_OVR_B2_B0 0x07

/* CMC_SLAVE_RX4  Register */
#define REG_ADDR__CMC_SLAVE_RX4				(RX_EARC_REG | 0x0073)
	/* (ReadOnly, Bits 5) */
	/* lost heartbeat index; 1-true; 0- false*/
	#define BIT_MSK__CMC_SLAVE_RX4__LOST_HEARTBEAT		0x20

/* CMC_SLAVE_RX5  Register */
#define REG_ADDR__CMC_SLAVE_RX5				(RX_EARC_REG | 0x0074)
	/* (ReadOnly, Bits 7:0)*/
	/* eARC Tx/Source Status at offset 0xD1 on eARC.
	 * Fetched by heartbeat over eARC
		bit 0: HDMI_HPD
		bit 7: eARC_Valid
	*/
	/* bit 0: HDMI_HPD */
	#define BIT_MASK__CMC_SLAVE_RX5__HDMI_HPD		0x01
	/* bit 3: CAP_CNG_CONF */
	#define BIT_MASK__CMC_SLAVE_RX5__CAP_CNG_CONF		0x08
	/* bit 4: STAT_CHNG_CONF */
	#define BIT_MASK__CMC_SLAVE_RX5__STAT_CHNG_CONF		0x10
	/* bit 7: eARC_Valid */
	#define BIT_MASK__CMC_SLAVE_RX5__EARC_VALID		0x80

/* CMC_SLAVE_RX4  Register */
/* eARC Rx/Sink status at offset 0xD0 on eARC. Will be sent by
 heartbeat over eARC */
#define REG_ADDR__CMC_SLAVE_RX6				(RX_EARC_REG | 0x0075)
	/* bit 0: eARC_HPD*/
	#define BIT_MSK__CMC_SLAVE_RX6__EARC_HPD		0x01
	/* bit 3: CAP_CHNG*/
	#define BIT_MSK__CMC_SLAVE_RX6__CAP_CHNG		0x08
	/* bit 4: STAT_CHNG*/
	#define BIT_MSK__CMC_SLAVE_RX6__STAT_CHNG		0x10

/* CMC_SLAVE_RX7  Register*/
#define REG_ADDR__CMC_SLAVE_RX7				(RX_EARC_REG | 0x0076)
	/* (ReadWrite, Bits 1)*/
	/* */
	#define BIT_MSK__CMC_SLAVE_RX7__REG_CEC_ARC_TERM	0x02
	/* (ReadWrite, Bits 0)*/
	/* */
	#define BIT_MSK__CMC_SLAVE_RX7__REG_CEC_ARC_INIT	0x01

/* CMC_SLAVE_RX8  Register*/
#define REG_ADDR__CMC_SLAVE_RX8				(RX_EARC_REG | 0x0077)
	/* (ReadOnly, Bits 7:0)*/
	/* this register is written by eARC Tx through Write Operation
	 * (0xD3 address) over Common Mode Channel
	 * local I2C can only read this register to get the value
	 * written by eARC Tx*/
	#define BIT_MSK__CMC_SLAVE_RX8__REG_CMC_RSVD1_W		0xFF

/* CMC_SLAVE_RX9  Register*/
#define REG_ADDR__CMC_SLAVE_RX9				(RX_EARC_REG | 0x0078)
	/* (ReadWrite, Bits 7:0)*/
	/* This register will be accessed by eARC TX through
	 * Read Operation (0xD2 address) over Common Mode Channel.
	 * Write Operation is not allowed for eARC TX.
	 * Local I2C can write this register and read it back*/
	#define BIT_MSK__CMC_SLAVE_RX9__REG_CMC_RSVD0_R_B7_B0	0xFF

/* CMC_SLAVE_RX10  Register*/
#define REG_ADDR__CMC_SLAVE_RX10			(RX_EARC_REG | 0x0079)
	/* (ReadWrite, Bits 1)*/
	/* the override value*/
	#define BIT_MSK__CMC_SLAVE_RX10__HPD_IN_OVRD_VAL	0x02
	/* (ReadWrite, Bits 0)*/
	/* 1- override the input HPD value*/
	#define BIT_MSK__CMC_SLAVE_RX10__HPD_IN_OVRD		0x01

/* Interrupt State Register */
#define REG_ADDR__INTR_STATE				(RX_EARC_REG | 0x00B0)
	/* (ReadOnly, Bits 0) */
	/* Interrupt state bit value. It shows whether the interrupt output
	 * signal on INT pin is active or not. 1' indicates that at least one
	 * masked interrupt has been asserted. This bit can only be cleared by
	 * SW; either by unmasking all asserted interrupts or by clearing all
	 * asserted interrupts.*/
	#define BIT_MSK__INTR_STATE__INTR_STATE			0x01
	/* (ReadOnly, Bits 1)*/
	/* 1 clk_extr is toggling; 0 clk_cxtr is stopping*/
	#define BIT_MSK__INTR_STATE__CLK_EXTR_STATUS		0x02

/* Interrupt Source #0 Register */
#define REG_ADDR__INTR0					(RX_EARC_REG | 0x00B1)
	/* (ReadWrite, Bits 7)*/
	/* Rx sync FIFO betwwen SPDIF & I2S is almost empty (-1) from pop side*/
	#define BIT_MSK__INTR0__REG_INTR_STAT7 0x80
	/* (ReadWrite, Bits 6)*/
	/* Rx sync FIFO betwwen SPDIF & I2S is empty from pop side*/
	#define BIT_MSK__INTR0__REG_INTR_STAT6 0x40
	/* (ReadWrite, Bits 5)*/
	/* Rx sync FIFO betwwen SPDIF & I2S is in error;
	 * because tried to push into full FIFO*/
	#define BIT_MSK__INTR0__REG_INTR_STAT5 0x20
	/* (ReadWrite, Bits 4)*/
	/* Rx sync FIFO betwwen SPDIF & I2S is full from push side*/
	#define BIT_MSK__INTR0__REG_INTR_STAT4 0x10
	/* (ReadWrite, Bits 3)*/
	/* Rx sync FIFO betwwen SPDIF & I2S is almost full (-1) from push side*/
	#define BIT_MSK__INTR0__REG_INTR_STAT3 0x08
	/* (ReadWrite, Bits 2)*/
	/* Rx sync FIFO betwwen SPDIF & I2S is half full (16) from push side*/
	#define BIT_MSK__INTR0__REG_INTR_STAT2 0x04
	/* (ReadWrite, Bits 1)*/
	/* Rx cmdc disvoerty state jump to IDLE2*/
	#define BIT_MSK__INTR0__REG_INTR_STAT1 0x02
	/* (ReadWrite, Bits 0)*/
	/* Rx cmdc disvoerty state jump to IDLE1*/
	#define BIT_MSK__INTR0__REG_INTR_STAT0 0x01

/* Interrupt Source #1 Register*/
#define REG_ADDR__INTR1					(RX_EARC_REG | 0x00B2)
	/* (ReadWrite, Bits 7)*/
	/* Rx SPDIF detected by-phase mark encoding error*/
	#define BIT_MSK__INTR1__REG_INTR_STAT15 0x80
	/* (ReadWrite, Bits 6)*/
	/* Rx SPDIF dropped sample because got new preamble out of order*/
	#define BIT_MSK__INTR1__REG_INTR_STAT14 0x40
	/* (ReadWrite, Bits 5)*/
	/* Rx SPDIF got new preamble unexpectably*/
	#define BIT_MSK__INTR1__REG_INTR_STAT13 0x20
	/* (ReadWrite, Bits 4)*/
	/* Rx SPDIF got parity error on incoming stream*/
	#define BIT_MSK__INTR1__REG_INTR_STAT12 0x10
	/* (ReadWrite, Bits 3)*/
	/* Rx sync FIFO betwwen SPDIF & I2S is in error;
	 * because tried to pop from empty FIFO*/
	#define BIT_MSK__INTR1__REG_INTR_STAT11 0x08
	/* (ReadWrite, Bits 2)*/
	/* Rx cmdc disvoerty state jump to ARC*/
	#define BIT_MSK__INTR1__REG_INTR_STAT10 0x04
	/* (ReadWrite, Bits 1)*/
	/* Rx cmdc disvoerty state jump to DISC2*/
	#define BIT_MSK__INTR1__REG_INTR_STAT9 0x02
	/* (ReadWrite, Bits 0)*/
	/* Rx cmdc disvoerty state jump to DISC1*/
	#define BIT_MSK__INTR1__REG_INTR_STAT8 0x01

/* Interrupt Source #2 Register */
#define REG_ADDR__INTR2					(RX_EARC_REG | 0x00B3)
	/* (ReadWrite, Bits 3)*/
	/* channel status had been changed*/
	#define BIT_MSK__INTR2__REG_INTR_STAT19			0x08

/* (ReadWrite, Bits 2)*/
	/* received Channel Status bits*/
	#define BIT_MSK__INTR2__REG_INTR_STAT18			0x04

	/* (ReadWrite, Bits 0)*/
	/* Rx SPDIF got new Fs*/
	#define BIT_MSK__INTR2__REG_INTR_STAT16			0x01

/* Interrupt Source #3 Register*/
#define REG_ADDR__INTR3					(RX_EARC_REG | 0x00B4)

	/* (ReadWrite, Bits 2)*/
	/* Whenever it enters "MUTE" state, this interrupt will be triggered.
	 * At the same time the "MUTE_OUT" pin will be set.
	 * The list of "MUTE" sources is described in register "MUTE_EXP_EN_0"
	 * (0x6E31) and "MUTE_EXP_EN_1" (ox6E32).*/
	#define BIT_MSK__INTR3__REG_INTR_STAT26 0x04
	/* (ReadWrite, Bits 4)*/
	/* "1"means that the embedded audio clock over differential pair
	 * has either stopped or started.
	 * For status check reg. "INTR_STATE" (0x6EB0) bit #1*/
	#define BIT_MSK__INTR3__REG_INTR_STAT28			0x10

	/* (ReadWrite, Bits 7) */
	/* "1" eARC Tx reads EDID through Common Mode Channel */
	#define BIT_MSK__INTR3__REG_INTR_STAT31			0x80

/* Interrupt Source #4 Register */
#define REG_ADDR__INTR4					(RX_EARC_REG | 0x00B5)
	/* (ReadWrite, Bits 1)*/
	/* rx heartbeat lost*/
	#define BIT_MSK__INTR4__REG_INTR_STAT33			0x02
	/* (ReadWrite, Bits 0)*/
	/* Source (Tx) status byte fetched by heartbeat changed.
	 * The RX side will get the TX status byte via heartbeart
	 * periodically, if any bit of the byte is different with
	 * last time, reg_intr_stat32 will be asserted*/
	#define BIT_MSK__INTR4__REG_INTR_STAT32			0x01

/* Interrupt Source #5 Register*/
#define REG_ADDR__INTR5					(RX_EARC_REG | 0x00B6)
	/* (ReadWrite, Bits 7)*/
	/* pkt_xmit_timeout is asserted in EARC_DATAW state"1" means that
	* no valid earc_done back before timeout when write data
	 * This bit is Related with the internal signal transmission.
	 * When read or write data command is sent out, it is expected
	 * to receive earc_done signal which means the data is ready*/
	#define BIT_MSK__INTR5__REG_INTR_STAT47			0x80
	/* (ReadWrite, Bits 6)*/
	/* pkt_xmit_timeout is asserted in EARC_CONT state; "1" means that
	 * no valid earc_done back before timeout when read data.
	 * This bit is Related with the internal signal transmission.
	 * When read or write data command is sent out, it is expected
	 * to receive earc_done signal which means the data is ready*/
	#define BIT_MSK__INTR5__REG_INTR_STAT46			0x40
	/* (ReadWrite, Bits 5)*/
	/* Whenever the value of measured sample rate becomes invalid
	 * from valid this interrupt will be triggered. The "valid value".
	 * includes sample rate listed in I2S_FS_MEAS_STAT_0 (0x6EF1).
	 * The bit #4 of register I2S_FS_MEAS_STAT_0 (0x6EF1) represents the
	 * valid or invalid status of the measured sample rate.*/
	#define BIT_MSK__INTR5__REG_INTR_STAT45			0x20
	/* (ReadWrite, Bits 4)*/
	/* Whenever the value of measured sample rate becomes valid
	 from invalid this interrupt will be triggered.
	 The "valid value" includes sample rate listed in
	 I2S_FS_MEAS_STAT_0 (0x6EF1). The bit[4] of register
	 I2S_FS_MEAS_STAT_0 (0x6EF1) represents the valid or
	 invalid status of the measured sample rate.*/
	#define BIT_MSK__INTR5__REG_INTR_STAT44			0x10
	/* (ReadWrite, Bits 3)*/
	/* The I2S "WS" signal is measured to determine the Audio sample rate.
	 Whenever the sample rate change is detected this interrupt will
	 be triggered.
	 The sample rate change means:
	   a. Sample rate becomes valid value from invalid one or vice versa;
	   b. Sample rate changes between two valid values;
	   c. The measured Sample rate changes between two invalid values.
	 The "valid value" includes sample rate listed in
	  I2S_FS_MEAS_STAT_0 (0x6EF1)*/
	#define BIT_MSK__INTR5__REG_INTR_STAT43			0x08
	/* (ReadWrite, Bits 0)*/
	/* This interrupt will be triggered whenever the "c" bit
	 in left channel and right channel are not identical and
	 the register bit "reg_lr_match_en" (bit[4] of RX_BPM_CS_CTRL,
	 0x6E2C) is set to "1".*/
	#define BIT_MSK__INTR5__REG_INTR_STAT40 0x01

/* Interrupt Source #6 Register*/
#define REG_ADDR__INTR6					(RX_EARC_REG | 0x00B7)
	/* (ReadWrite, Bits 7)*/
	/* "1" means that the pkt rx received has uncorrectable ECC ERR*/
	#define BIT_MSK__INTR6__REG_INTR_STAT55			0x80
	/* (ReadWrite, Bits 6)*/
	/* "1" means rx heartbeat succeeds include READ and WRITE*/
	#define BIT_MSK__INTR6__REG_INTR_STAT54			0x40
	/* (ReadWrite, Bits 5)*/
	/* "1" means rx discovery entered the pause
	  interval between two comma sequences*/
	#define BIT_MSK__INTR6__REG_INTR_STAT53			0x20
	/* (ReadWrite, Bits 4)*/
	/* "1" means rx discovery timeout is deasserted*/
	#define BIT_MSK__INTR6__REG_INTR_STAT52			0x10
	/* (ReadWrite, Bits 3)*/
	/* "1" means rx discovery timeout is asserted*/
	#define BIT_MSK__INTR6__REG_INTR_STAT51			0x08
	/* (ReadWrite, Bits 2)*/
	/* "1" means rx received wrong packet*/
	#define BIT_MSK__INTR6__REG_INTR_STAT50			0x04
	/* (ReadWrite, Bits 1)*/
	/* "1" means rx received unexpected packets or
	  uncorrectable ECC error*/
	#define BIT_MSK__INTR6__REG_INTR_STAT49			0x02
	/* (ReadWrite, Bits 0)*/
	/* earc_ack is low in EARC_ACKA state and slave will
	  send NACK to master*/
	#define BIT_MSK__INTR6__REG_INTR_STAT48			0x01

/* Interrupt Source #7 Register */
#define REG_ADDR__INTR7					(RX_EARC_REG | 0x00B8)
	/* (ReadWrite, Bits 7)*/
	/* bit7 (eARC Valid) of received tx heartbeat status changed*/
	#define BIT_MSK__INTR7__REG_INTR_STAT63			0x80
	/* (ReadWrite, Bits 4)*/
	/* bit4 (STAT_CHNG_CONF)of received tx heartbeat status changed*/
	#define BIT_MSK__INTR7__REG_INTR_STAT60			0x10
	/* (ReadWrite, Bits 3)*/
	/* bit3 (CAP_CNG_CONF)of received tx heartbeat status changed*/
	#define BIT_MSK__INTR7__REG_INTR_STAT59			0x08
	/* (ReadWrite, Bits 0) */
	/* bit0 (HDMI HPD) of received tx heartbeat status changed */
	#define BIT_MSK__INTR7__REG_INTR_STAT56			0x01

/* Interrupt Source #8 Register */
#define REG_ADDR__INTR8					(RX_EARC_REG | 0x00B9)
	/* (ReadWrite, Bits 2) */
	/* TX write reg_cmc_rsvd0_w via SW access 0xD3 is done successfully*/
	#define BIT_MSK__INTR8__REG_INTR_STAT66			0x04
	/* (ReadWrite, Bits 1) */
	/* TX read reg_cmc_rsvd0_r via SW access 0xD2 is done successfully*/
	#define BIT_MSK__INTR8__REG_INTR_STAT65			0x02
	/* (ReadWrite, Bits 0) */
	/* common mode data channel connection status changed.
	 This registers is related with bit #7 of 0x6E68,
	 when its value changed this register will be asserted*/
	#define BIT_MSK__INTR8__REG_INTR_STAT64			0x01

/* Interrupt #0 Mask Register */
#define REG_ADDR__INTR0_MASK				(RX_EARC_REG | 0x00BA)
	/* TX CMDC discovery state jumps to IDLE2*/
	#define BIT_MASK__INTR0_MASK__STATE_IDLE2		0x02
	/* TX CMDC discovery state jumps to IDLE1*/
	#define BIT_MASK__INTR0_MASK__STATE_IDLE1		0x01

/* Interrupt #1 Mask Register */
#define REG_ADDR__INTR1_MASK				(RX_EARC_REG | 0x00BB)
	/* TX CMDC discovery state jumps to ARC*/
	#define BIT_MASK__INTR1_MASK__STATE_ARC			0x04
	/* TX CMDC discovery state jumps to DISC2*/
	#define BIT_MASK__INTR1_MASK__STATE_DISC2		0x02
	/* TX CMDC discovery state jumps to DISC1*/
	#define BIT_MASK__INTR1_MASK__STATE_DISC1		0x01

/* Interrupt #2 Mask Register */
#define REG_ADDR__INTR2_MASK				(RX_EARC_REG | 0x00BC)

/* Interrupt #3 Mask Register */
#define REG_ADDR__INTR3_MASK				(RX_EARC_REG | 0x00BD)

/* Interrupt #4 Mask Register */
#define REG_ADDR__INTR4_MASK				(RX_EARC_REG | 0x00BE)
	/* TX CMDC discovery state jumps to eARC*/
	#define BIT_MASK__INTR4_MASK__STATE_EARC		0x08
	/* One pkt ecc err is fixed*/
	#define BIT_MASK__INTR4_MASK__PKT_ECC_ERR_FIXED		0x40


/* Interrupt #5 Mask Register */
#define REG_ADDR__INTR5_MASK				(RX_EARC_REG | 0x00BF)

/* Interrupt #6 Mask Register */
#define REG_ADDR__INTR6_MASK				(RX_EARC_REG | 0x00C0)
	/* Pkt rx received has uncorrectable ECC ERR*/
	#define BIT_MASK__INTR4_MASK__PKT_ECC_ERR		0x80
	/* Rx received unexpected packets or uncorrectable ECC error*/
	#define BIT_MASK__INTR4_MASK__ECC_ERR			0x02

/* Interrupt #7 Mask Register */
#define REG_ADDR__INTR7_MASK				(RX_EARC_REG | 0x00C1)

/* Interrupt #8 Mask Register */
#define REG_ADDR__INTR8_MASK				(RX_EARC_REG | 0x00C2)

/* Test Ctrl Register */
#define REG_ADDR__TEST_CTRL				(RX_EARC_REG | 0x00D0)
	/* (ReadWrite, Bits 5:4)*/
	/* Select from which pipe-line to observe data:
	"00" from tx I2S
	"01" from tx spdif
	"10" from rx spdif
	"11" from rx i2s*/
	#define BIT_MSK__TEST_CTRL__REG_OBS_PIPE_SEL		0x30
	/* (ReadWrite, Bits 3:1)*/
	/* control test mode for input data:
	"000" set input data on ch0 to AA; ch1 to 55; ch2 to AA and ch3 to 55
	"001" increment each channel every sample
	"010" set each channel to shifted out test value
	"011" set each channel to constant test value
	"111" will allow input data to go thru
	everything else reserved*/
	#define BIT_MSK__TEST_CTRL__REG_TEST_MODE		0x0E
	/* (ReadWrite, Bits 0)*/
	/* enable test mode based on test_mode control*/
	#define BIT_MSK__TEST_CTRL__REG_TEST_EN			0x01

/* Rx CS BCH ECC Pointer Register*/
#define REG_ADDR__RX_CS_ECC_PTR				(RX_EARC_REG | 0x00DB)
	/* (ReadWrite, Bits 7:0)*/
	/* offset pointer within Channel Status array for BCH ECC pattern*/
	#define BIT_MSK__RX_CS_ECC_PTR__REG_RX_CS_ECC_PTR	0xFF

/* Rx CS BCH ECC Mask Register*/
#define REG_ADDR__RX_CS_ECC_MASK			(RX_EARC_REG | 0x00DC)
	/* (ReadWrite, Bits 7:0)*/
	/* mask for the BCH ECC pattern in Channel Status array.
	 * Only bits set to "1" are used*/
	#define BIT_MSK__RX_CS_ECC_MASK__REG_RX_CS_ECC_MASK	0xFF

/* Rx CS BCH ECC Pattern Register*/
#define REG_ADDR__RX_CS_ECC_PATTERN			(RX_EARC_REG | 0x00DD)
	/* (ReadWrite, Bits 7:0)*/
	/* pattern to compare against to see if BCh ECC is on.
	 * Only bits set by mask will be compared*/
	#define BIT_MSK__RX_CS_ECC_PATTERN__REG_RX_CS_ECC_PATTERN	0xFF

/* BCH Configure Register*/
#define REG_ADDR__BCH_CTRL				(RX_EARC_REG | 0x00DE)
	/* (ReadWrite, Bits 7)*/
	/* BCH interleaving enable; default "0" means disabled*/
	#define BIT_MSK__BCH_CTRL__REG_BCHINTV_EN		0x80
	/* (ReadWrite, Bits 6:5)*/
	/* 00 Disable ECC
	 * 01 Turn on ECC if CS word shows 16-bit word length
	 * 10 Turn on ECC if ECC enable bit is 1
	 * 11 Turn on ECC if both the above items are true*/
	#define BIT_MSK__BCH_CTRL__REG_ECC_EN			0x60
	#define BIT_MSK__BCH_CTRL__REG_ECC_ENABLE		0x40
	#define BIT_MSK__BCH_CTRL__REG_ECC_DISABLE		0x00

	/* (ReadWrite, Bits 4:0)*/
	/* BCH corrected threshold*/
	#define BIT_MSK__BCH_CTRL__REG_BCH_CORRECTED_THRESHOLD	0x1F

/* BCH Status Register*/
#define REG_ADDR__BCH_STAT				(RX_EARC_REG | 0x00DF)
	/* (ReadOnly, Bits 0)*/
	/* 1:ecc decoding is ongoing*/
	#define BIT_MSK__BCH_STAT__ECC_DEC_ON			0x01

/* EDID FIFO Addr Register*/
#define REG_ADDR__EDID_FIFO_ADDR			(RX_EARC_REG | 0x00E1)

/* EDID FIFO Data Register*/
#define REG_ADDR__EDID_FIFO_DATA			(RX_EARC_REG | 0x00E4)

/* I2S Sample Frequency Measure Status 0 Register*/
#define REG_ADDR__I2S_FS_MEAS_STAT_0			(RX_EARC_REG | 0x00F1)
	/* (ReadOnly, Bits 7:6)*/
	/* use 20MHz clock to count the period between adjecent WS
	 rising edges; this register shows the counting value.
	  The counting value could be with minor difference between
	   samples due to sampling error; clock jitter; ws jitter; etc.*/
	#define BIT_MSK__I2S_FS_MEAS_STAT_0__REG_I2S_FS_CNT_MSB	0xC0
	/* (ReadOnly, Bits 4)*/
	/* 1: valid sample frequency has been detected; the sample frequency
	 should be one of followings: 32K; 44.1K; 48K; 88.2K; 96K; 176.4K
	 and 192KHz*/
	#define BIT_MSK__I2S_FS_MEAS_STAT_0__REG_I2S_FS_VALID	0x10
	/* (ReadOnly, Bits 3:2)*/
	/* 00=multiply by 1; 01=multiply by 2; 10=multiply by 4; 11=RSVD
	 for example: 196KHz audio reg_i2s_fs_base=01 and reg_i2s_fs_mult=10*/
	#define BIT_MSK__I2S_FS_MEAS_STAT_0__REG_I2S_FS_MULT	0x0C
	/* (ReadOnly, Bits 1:0)*/
	/* 00=44.1K; 01=48K; 11=32K; 10=RSVD;*/
	#define BIT_MSK__I2S_FS_MEAS_STAT_0__REG_I2S_FS_BASE	0x03

/*****************  PHY (6400-64FF)*********************/
/* Viola status Register*/
#define REG_ADDR__VIOLA_STATUS				(RX_PHY_REG | 0x0060)
	/* (ReadWrite, Bits 7)*/
	/* rx fifo reset*/
	#define BIT_MSK__VIOLA_STATUS__REG_RST_RX_PHY_FIFO_N	0x80
	/* (ReadWrite, Bits 6)*/
	/* dviola rx logic reset*/
	#define BIT_MSK__VIOLA_STATUS__REG_RST_DVIOLA_RX_N	0x40
	/* (ReadWrite, Bits 5)*/
	/* tx fifo reset*/
	#define BIT_MSK__VIOLA_STATUS__REG_RST_TX_PHY_FIFO_N	0x20
	/* (ReadWrite, Bits 4)*/
	/* dviola rx logic reset*/
	#define BIT_MSK__VIOLA_STATUS__REG_RST_DVIOLA_TX_N	0x10
	/* (ReadOnly, Bits 1)*/
	/* earc lock detect*/
	#define BIT_MSK__VIOLA_STATUS__EARC_LOCK_DETECT		0x02
	/* (ReadOnly, Bits 0)*/
	/* earc clock detect*/
	#define BIT_MSK__VIOLA_STATUS__EARC_CLK_DETECT		0x01

/* Viola mode Register */
#define REG_ADDR__VIOLA_MODE				(RX_PHY_REG | 0x0064)
	/* (ReadWrite, Bits 0)*/
	/* PLL enable 0: disable 1: enable*/
	#define BIT_MSK__VIOLA_MODE__PLL_EN			0x01
	/* (ReadWrite, Bits 2) */
	/* Choose Tx/Rx mode 0: ARC/eARC Tx mode 1: ARC/eARC Rx mode*/
	#define BIT_MSK__VIOLA_MODE__REG_ARC_DIR		0x04
	/* (ReadWrite, Bits 3) */
	/* Choose legacy ARC or eARC mode 0: legacy ARC mode 1: eARC mode*/
	#define BIT_MSK__VIOLA_MODE__REG_ARC_MODE		0x08
	/* (ReadWrite, Bits 5) */
	/* overwrite lock status*/
	#define BIT_MSK__VIOLA_MODE__REG_LOCK_OVR		0x20
	/* (ReadWrite, Bits 6) */
	/* reg lock status*/
	#define BIT_MSK__VIOLA_MODE__REG_LOCK			0x40
	/* (ReadWrite, Bits 7) */
	/* auto reg_earc_cfg setting for tx/rx*/
	#define BIT_MSK__VIOLA_MODE__REG_AUTO_CFG		0x80

/* Viola cfg 0 Register*/
#define REG_ADDR__EARC_CFG0				(RX_PHY_REG | 0x0065)
	/* (ReadWrite, Bits 7:0)*/
	/* [0]: PLL clock to Tx datapath
	 *       (0: enable Tx path eARC datapath/ 1: disable)
	 * [1]: PLL clock to Rx datapath
	 *       (0: enable Rx path eARC datapath/ 1: disable)
	 * [3:2]: Tx datapath data polarity change
	 *   (00: eARC (phase inverted);
	      01: common-mode legacy ARC);
	      11: eARC (normal phase))
	 * [4]: CKDT mode setting (0: Not used/ 1: faster clock than 65MHz)
	 * [5]: CKDT block power down (0:power down/ 1: normal operation)
	 * [6]: Rx datapath power down (0: power down/ 1: normal operation)
	 * [7]: ARC data path filter enable*/
	#define BIT_MSK__EARC_CFG0__REG_EARC_CFG_B7_B0		0xFF

	#define BIT_MASK__EARC_CFG0__PLL_CLK_TX_DATAPATH	0x01
	#define BIT_MASK__EARC_CFG0__PLL_CLK_RX_DATAPATH	0x02
	#define BIT_MASK__EARC_CFG0__TX_DATAPATH_POL_CHG	0x0C
	#define BIT_MASK__EARC_CFG0__TX_DP_POL_CM_ARC		0x04
	#define BIT_MASK__EARC_CFG0__CKDT_MODE			0x10
	#define BIT_MASK__EARC_CFG0__CLDT_BLK_PWRDN		0x20
	#define BIT_MASK__EARC_CFG0__RX_DPATH_PWRDN		0x40
	#define BIT_MASK__EARC_CFG0__ARC_DATA_PATH_FIL_EN	0x80

/* Viola cfg 1 Register */
#define REG_ADDR__EARC_CFG_1				(RX_PHY_REG | 0x0066)
	/* (ReadWrite, Bits 7:0)*/
	/* [9:8]: Rx datapath clock output delay control
	 *    (00: No delay/
	 *     01: one delay
	 *     10: opposite phase
	 *     11: opposite phase + one delay)*/
	/* [11:10]: legacy ARC mode control
	 *    (11: common-mode;
	       10: single-mode)*/
	#define BIT_MASK__EARC_CFG_1__COMM_MODE			0x0C
	#define BIT_MASK__EARC_CFG_1__SINGLE_MODE		0x08
	#define BIT_MSK__EARC_CFG1__ARC_MODE_CTRL_CM		0x0C
	#define BIT_MSK__EARC_CFG1__CM_LEVEL_CTRL_SM		0x08

	/* [13:12]: rterm_sel for common-mode signal for replica bias
	 *    (2'b00 : select 50ohm Rterm case;
	       2'b01 : select 30ohm Rterm case)*/
	#define BIT_MSK__EARC_CFG1__RTERM_MASK			0x30
	#define BIT_MSK__EARC_CFG1__RTERM_50OHM			0x00
	#define BIT_MSK__EARC_CFG1__RTERM_30OHM			0x10

	/* [15:14]: common-mode level control
	 *    (2'b00 : 200mV;
	       2'b01 : 250mV;
	 *     2'b10 : 300mV; 2'b11 : 350mV)*/
	#define BIT_MSK__EARC_CFG1__CM_LEVEL_CTRL_200MV		0x00
	#define BIT_MSK__EARC_CFG1__CM_LEVEL_CTRL_250MV		0x40
	#define BIT_MSK__EARC_CFG1__CM_LEVEL_CTRL_300MV		0x80
	#define BIT_MSK__EARC_CFG1__CM_LEVEL_CTRL_350MV		0xC0

	#define BIT_MSK__EARC_CFG1__REG_EARC_CFG_B15_B8		0xFF

	#define BIT_MASK__EARC_CFG_1__COMM_MODE_LEVEL		0xC0
	#define BIT_MASK__EARC_CFG_1__LEGCY_ARC_MODE_CTRL	0x0C

/* Viola term60 Register*/
#define REG_ADDR__VIOLA_TERM60				(RX_PHY_REG | 0x006C)
	/* (ReadWrite, Bits 3)*/
	/* internal termination enable
	 *  0: disable termination
	 *  1: enable termination*/
	#define BIT_MSK__VIOLA_TERM60__REG_TERM60_EN		0x08
	/* (ReadWrite, Bits 2:0)*/
	/* To calibrate the eARC 60 ohm termination which is the binary code;
	 *  000 biggest value;
	 *  111: smallest value voltage calibration*/
	#define BIT_MSK__VIOLA_TERM60__EARC_TERM60_CTL		0x07

/* Viola term300 Register*/
#define REG_ADDR__VIOLA_TERM300				(RX_PHY_REG | 0x006D)
	/* (ReadWrite, Bits 3)*/
	/* internal termination enable
	 *  0: disable termination
	 *  1: enable termination*/
	#define BIT_MSK__VIOLA_TERM300__REG_TERM300_EN 0x08
	/* (ReadWrite, Bits 2:0)*/
	/* To calibrate the eARC 300 ohm termination which is the binary code;
	 *  000 biggest value;
	 *  111: smallest value voltage calibration*/
	#define BIT_MSK__VIOLA_TERM300__EARC_TERM300_CTL 0x07

/* Viola slew n swing Register */
#define REG_ADDR__VIOLA_SLEW_N_SWING			(RX_PHY_REG | 0x006E)
	/* (ReadWrite, Bits 6:4)*/
	/* eARC driver bias current output swing control;
	   which is the binary code;
	   000: smallest value 4mA;
	   111: biggest value 16mA*/
	#define BIT_MSK__VIOLA_SLEW_N_SWING__EARC_SW_CTL	0x70
	/* (ReadWrite, Bits 3:0)*/
	/* slew rate control of the eARC driver;
	   which is the thermometer-code;
	    0000: slowest mode;
	    1111: fastest mode*/
	#define BIT_MSK__VIOLA_SLEW_N_SWING__EARC_DR_SLEW	0x0F

/* Viola sprx Register */
#define REG_ADDR__VIOLA_SPRX				(RX_PHY_REG | 0x006F)

/* Viola VCO calibration Register */
#define REG_ADDR__VIOLA_VCO_CAL				(RX_PHY_REG | 0x0070)
	/* (ReadWrite, Bits 7) */
	/* select i2c zone
	Shared register bit. Manually set Tx PLL zone for Tx and Rx PLL for Rx*/
	#define BIT_MSK__VIOLA_VCO_CAL__REG_ZONE_OVR		0x80
	/* (ReadWrite, Bits 6:5)*/
	/* reg_tx_zone.  Zone in which Tx PLL is supposed to work.
	 if eARC link clock is lower than 20MHz, then 4x zone should be used.
		2'b00: 1x
		2'b01 :2x
		2'b10 :4x
		2'b11 :8x
	*/
	#define BIT_MSK__VIOLA_VCO_CAL__REG_TX_ZONE		0x60
	/* (ReadWrite, Bits 4)*/
	/* bias VCO */
	#define BIT_MSK__VIOLA_VCO_CAL__BIAS_VCO		0x10
	/* (ReadWrite, Bits 3:0)*/
	/* VCO calibration */
	#define BIT_MSK__VIOLA_VCO_CAL__VCOCAL			0x0F
	/* (ReadWrite, Bits 7) */
	/* auto reg_earc_cfg setting for tx/rx*/
	#define BIT_MSK__VIOLA_MODE__REG_TX_ZONE		0x60
	#define BIT_MSK__VIOLA_VCO_CAL__REG_TX_ZONE_SHIFT	0x05

/* Viola CM BW control Register*/
#define REG_ADDR__VIOLA_CM_BW_CTL			(RX_PHY_REG | 0x0074)
	/* (ReadWrite, Bits 6)*/
	/* manual common mode bw control state*/
	#define BIT_MSK__VIOLA_CM_BW_CTL__REG_MAN_CM_FSM	0x40
	/* (ReadWrite, Bits 5:4)*/
	/* common mode bw control state*/
	#define BIT_MSK__VIOLA_CM_BW_CTL__REG_CM_FSM		0x30
	/* (ReadWrite, Bits 3:2)*/
	/* filter_config*/
	#define BIT_MSK__VIOLA_CM_BW_CTL__REG_FLT_CFG		0x0C
	/* (ReadWrite, Bits 0)*/
	/* select i2c bw control*/
	#define BIT_MSK__VIOLA_CM_BW_CTL__REG_MAN_BW_CFG	0x01

/* Viola rx cfg Register */
#define REG_ADDR__VIOLA_RX_CFG				(RX_PHY_REG | 0x0078)

#endif /* _SII9437_REGS_H_ */

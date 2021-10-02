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
 * @file  sii9437_example.h
 *
 * @brief  Sii9437 example application header
 *
 *****************************************************************************/
#ifndef _SII9437_EXAMPLE_H_
#define _SII9437_EXAMPLE_H_

#include "sii9437_regs.h"
#include "sii_datatypes.h"
#include <CoOS.h>
#include <OsTask.h>
#include <NUC123Series.h>
#include <stdint.h>
#include <GPIO.h>
#include <string.h>
#include <I2C.h>
#include <I2C_MASTER.h>
#include <Vcom_Data_Proc.h>
#include <CLOCK.h>
#include <SYS.h>
#include <GPIO.h>
#include <FMC.h>
#include <Host_Core.h>
#include <LibTimer.h>

/* Used GPIO Pin Mapping */
#define SII_NU_DIP1		PC10
#define SII_NU_DIP2		PC11
#define SII_NU_DIP3		PC12
#define SII_NU_DIP4		PC13
#define SII_NU_DIP5		PA13
#define SII_NU_DIP6		PA14


#define SII_NU_LED1		PC0
#define SII_NU_LED2		PC1
#define SII_NU_LED3		PC8
#define SII_NU_LED4		PC9

#define SII_TGPIO2		PC2
#define SII_TGPIO3		PC3

/* GPIOs Used for UART1 */
#define SII_NU_RXD		PB4
#define SII_NU_TXD		PB5

/* INT0 is used SiI9437 Interrupt */
#define SII9437_INT	PB14

/* GPIO used for SiI9612 Interrupt */
#define SII9612_INT	PB8

#define SII_COMM_MODE		SII_NU_DIP1
#define SII_AUD_MODE_0		SII_NU_DIP2
#define SII_ARC_MODE		SII_NU_DIP3
#define SII_DEV_PA		SII_NU_DIP4
#define SII_INTR_POLL_MODE	SII_NU_DIP5
#define SII_ARC_MODE_NONE	SII_NU_DIP6

#define SII_AUD_MODE_VAL0	0x01
#define SII_AUD_MODE_VAL1	0x02
#define SII_AUD_MODE_VAL2	0x04

#endif /* _SII9437_EXAMPLE_H_ */

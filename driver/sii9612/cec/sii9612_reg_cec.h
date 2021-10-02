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
* sii9612_reg_cec.h
*************************************************************************/

#ifndef __SI_REGS_CPI_H__
#define __SI_REGS_CPI_H__

/*-----------------------------------------------------------------------------
* Registers in Page 8
------------------------------------------------------------------------------*/
#define CEC_PAGE							0x2800					   	

#define REG_CEC_DEBUG_2						(CEC_PAGE | 0x86)

#define REG_CEC_DEBUG_3						(CEC_PAGE | 0x87)
#define BIT_SNOOP_EN                        0x01
#define BIT_FLUSH_TX_FIFO                   0x80

#define REG_CEC_TX_INIT						(CEC_PAGE | 0x88)
#define BIT_SEND_POLL                       0x80

#define REG_CEC_TX_DEST						(CEC_PAGE | 0x89)

#define REG_CEC_CONFIG_CPI					(CEC_PAGE | 0x8E)

#define REG_CEC_TX_COMMAND					(CEC_PAGE | 0x8F)
#define REG_CEC_TX_OPERAND_0				(CEC_PAGE | 0x90)

#define REG_CEC_TRANSMIT_DATA				(CEC_PAGE | 0x9F)
#define BIT_TX_BFR_ACCESS                   0x40
#define BIT_TX_AUTO_CALC                    0x20
#define BIT_TRANSMIT_CMD                    0x10

#define REG_CEC_CAPTURE_ID0					(CEC_PAGE | 0xA2)

#define REG_CEC_INT_ENABLE_0				(CEC_PAGE | 0xA4)
#define REG_CEC_INT_ENABLE_1				(CEC_PAGE | 0xA5)

// 0xA6 CPI Interrupt Status Register (R/W)
#define REG_CEC_INT_STATUS_0				(CEC_PAGE | 0xA6)
#define BIT_CEC_LINE_STATE                  0x80
#define BIT_TX_BUFFER_FULL                  0x40
#define BIT_TX_MESSAGE_SENT                 0x20
#define BIT_TX_FIFO_EMPTY                   0x04
#define BIT_RX_MSG_RECEIVED                 0x02
#define BIT_CMD_RECEIVED                    0x01

// 0xA7 CPI Interrupt Status Register (R/W)
#define REG_CEC_INT_STATUS_1				(CEC_PAGE | 0xA7)
#define BIT_RX_FIFO_OVERRUN                 0x08
#define BIT_SHORT_PULSE_DET                 0x04
#define BIT_FRAME_RETRANSM_OV               0x02
#define BIT_START_IRREGULAR                 0x01

#define REG_CEC_RX_CONTROL					(CEC_PAGE | 0xAC)
// CEC  CEC_RX_CONTROL bits
#define BIT_CLR_RX_FIFO_CUR                 0x01
#define BIT_CLR_RX_FIFO_ALL                 0x02

#define REG_CEC_RX_COUNT					(CEC_PAGE | 0xAD)
#define BIT_MSG_ERROR                       0x80


#define REG_CEC_RX_CMD_HEADER				(CEC_PAGE | 0xAE)
#define REG_CEC_RX_OPCODE					(CEC_PAGE | 0xAF)
#define REG_CEC_RX_OPERAND_0				(CEC_PAGE | 0xB0)


#define REG_CEC_RX_COUNT					(CEC_PAGE | 0xAD)

#define CEC_OP_ABORT_0						(CEC_PAGE | 0xC0)
#define CEC_OP_ABORT_31						(CEC_PAGE | 0xDF)


#define REG_CEC_AUTO_DISCOVER_CMD			(CEC_PAGE | 0xE0)
#define BIT_CEC_AUTO_DISCOVER_START         0x01
#define BIT_CEC_AUTO_DISCOVER_CLEAR         0x02
#define BIT_CEC_AUTO_DISCOVER_DONE          0x80

#define REG_CEC_AUTO_DISCOVER_MAP0			(CEC_PAGE | 0xE1)
#define REG_CEC_AUTO_DISCOVER_MAP1			(CEC_PAGE | 0xE2)

#endif  // __SI_REGS_CPI_H__


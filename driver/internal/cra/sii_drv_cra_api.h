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
 * @file sii_drv_cra_api.h
 *
 * @brief Chip Register Access Driver
 *
 *****************************************************************************/

#ifndef SII_DRV_CRA_API_H
#define SII_DRV_CRA_API_H

/***** #include statements ***************************************************/

#include "sii_datatypes.h"

/***** public functions ******************************************************/

void sii_drv_cra_create(uint32_t dev_id);
void sii_drv_cra_delete(uint32_t dev_id);

void sii_drv_cra_write8(uint32_t dev_id, uint16_t addr, uint8_t val);
uint8_t sii_drv_cra_read8(uint32_t dev_id, uint16_t addr);
void sii_drv_cra_bits_set8(uint32_t dev_id, uint16_t addr,
	uint8_t mask, bool_t set);
void sii_drv_cra_bits_mod8(uint32_t dev_id, uint16_t addr,
	uint8_t mask, uint8_t val);

void sii_drv_cra_write16(uint32_t dev_id, uint16_t addr, uint16_t val);
uint16_t sii_drv_cra_read16(uint32_t dev_id, uint16_t addr);
void sii_drv_cra_bits_set16(uint32_t dev_id, uint16_t addr,
	uint16_t mask, bool_t set);
void sii_drv_cra_bits_mod16(uint32_t dev_id, uint16_t addr,
	uint16_t mask, uint16_t val);

void sii_drv_cra_write24(uint32_t dev_id, uint16_t addr, uint32_t val);
uint32_t sii_drv_cra_read24(uint32_t dev_id, uint16_t addr);
void sii_drv_cra_bits_set24(uint32_t dev_id, uint16_t addr,
	uint32_t mask, bool_t set);
void sii_drv_cra_bits_mod24(uint32_t dev_id, uint16_t addr,
	uint32_t mask, uint32_t val);

void sii_drv_cra_write32(uint32_t dev_id, uint16_t addr, uint32_t val);
uint32_t sii_drv_cra_read32(uint32_t dev_id, uint16_t addr);
void sii_drv_cra_bits_set32(uint32_t dev_id, uint16_t addr,
	uint32_t mask, bool_t set);
void sii_drv_cra_bits_mod32(uint32_t dev_id, uint16_t addr,
	uint32_t mask, uint32_t val);

void sii_drv_cra_fifo_write8(uint32_t dev_id, uint16_t addr,
	const uint8_t *p_data, uint16_t size);
void sii_drv_cra_fifo_read8(uint32_t dev_id, uint16_t addr,
	uint8_t *p_data, uint16_t size);
void sii_drv_cra_block_write8(uint32_t dev_id, uint16_t addr,
	const uint8_t *p_data, uint16_t size);
void sii_drv_cra_block_read8(uint32_t dev_id, uint16_t addr,
	uint8_t *p_data, uint16_t size);

#endif /* SII_DRV_CRA_API_H */

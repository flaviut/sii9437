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
 * @file sii_drv_cra.c
 *
 * @brief CRA (common register access) driver
 *
 *****************************************************************************/

/* #define SII_DEBUG 3 */

/***** #include statements ***************************************************/

#include "sii_system_api.h"
#include "sii_platform_api.h"
#include "sii_drv_cra_api.h"
/*#include "sii_sys_log_api.h"*/
/***** Register Module name **************************************************/

SII_MODULE_NAME_SET(DrvCra);

/***** local macro definitions ***********************************************/

#define BITS_SET(inp, mask, set) ((set) ? (inp | mask) : (inp & (~mask)))
#define BITS_MOD(inp, mask, val) ((inp & (~mask)) | (val & mask))

/***** local prototypes ******************************************************/

static void int32_to_data(uint8_t bytes, uint8_t *p_arr, uint32_t val);
static uint32_t data_to_int32(uint8_t bytes, uint8_t *p_arr);
static void reg_write(uint32_t dev_id, uint16_t addr,
		uint8_t bytes, uint32_t val);
static uint32_t reg_read(uint32_t dev_id, uint16_t addr, uint8_t bytes);

/***** local data objects ****************************************************/

/***** public functions ******************************************************/

void sii_drv_cra_create(uint32_t dev_id)
{
	dev_id = dev_id;
}

void sii_drv_cra_delete(uint32_t dev_id)
{
	dev_id = dev_id;
}

void sii_drv_cra_write8(uint32_t dev_id, uint16_t addr, uint8_t val)
{
	reg_write(dev_id, addr, 1, val);
	/*SII_LOG1A("", NULL, ("0x%02x:0x%02x = 0x%02x\n",
			dev_id, addr, val));*/
}

uint8_t sii_drv_cra_read8(uint32_t dev_id, uint16_t addr)
{
	return (uint8_t) reg_read(dev_id, addr, 1);
}

void sii_drv_cra_bits_set8(uint32_t dev_id, uint16_t addr, uint8_t mask,
		bool_t set)
{
	uint8_t temp;

	temp = (uint8_t) reg_read(dev_id, addr, 1);
	temp = BITS_SET(temp, mask, set);
	reg_write(dev_id, addr, 1, temp);

	/*SII_LOG1A("", NULL, ("0x%02x:0x%02x[0x%02x] = 0x%02x\n",
				dev_id, addr, mask, set));*/
}

void sii_drv_cra_bits_mod8(uint32_t dev_id, uint16_t addr, uint8_t mask,
		uint8_t val)
{
	uint8_t temp;

	temp = (uint8_t) reg_read(dev_id, addr, 1);
	temp = BITS_MOD(temp, mask, val);
	reg_write(dev_id, addr, 1, temp);
	/*SII_LOG1A("", NULL, ("0x%02x:0x%02x[0x%02x] = 0x%02x\n",
					dev_id, addr, mask, val));*/
}

void sii_drv_cra_write16(uint32_t dev_id, uint16_t addr, uint16_t val)
{
	reg_write(dev_id, addr, 2, (uint32_t)val);
}

uint16_t sii_drv_cra_read16(uint32_t dev_id, uint16_t addr)
{
	return (uint16_t) reg_read(dev_id, addr, 2);
}

void sii_drv_cra_bits_set16(uint32_t dev_id, uint16_t addr, uint16_t mask,
		bool_t set)
{
	uint16_t temp;

	temp = (uint16_t) reg_read(dev_id, addr, 2);
	temp = BITS_SET(temp, mask, set);
	reg_write(dev_id, addr, 2, temp);
}

void sii_drv_cra_bits_mod16(uint32_t dev_id, uint16_t addr, uint16_t mask,
		uint16_t val)
{
	uint16_t temp;

	temp = (uint16_t) reg_read(dev_id, addr, 2);
	temp = BITS_MOD(temp, mask, val);
	reg_write(dev_id, addr, 2, temp);
}

void sii_drv_cra_write24(uint32_t dev_id, uint16_t addr, uint32_t val)
{
	reg_write(dev_id, addr, 3, val);
}

uint32_t sii_drv_cra_read24(uint32_t dev_id, uint16_t addr)
{
	return (uint32_t) reg_read(dev_id, addr, 3);
}

void sii_drv_cra_bits_set24(uint32_t dev_id, uint16_t addr, uint32_t mask,
		bool_t set)
{
	uint32_t temp;

	temp = (uint32_t) reg_read(dev_id, addr, 3);
	temp = BITS_SET(temp, mask, set);
	reg_write(dev_id, addr, 3, temp);
}

void sii_drv_cra_bits_mod24(uint32_t dev_id, uint16_t addr, uint32_t mask,
		uint32_t val)
{
	uint32_t temp;

	temp = (uint32_t) reg_read(dev_id, addr, 3);
	temp = BITS_MOD(temp, mask, val);
	reg_write(dev_id, addr, 3, temp);
}

void sii_drv_cra_write32(uint32_t dev_id, uint16_t addr, uint32_t val)
{
	reg_write(dev_id, addr, 4, val);
}

uint32_t sii_drv_cra_read32(uint32_t dev_id, uint16_t addr)
{
	return (uint32_t) reg_read(dev_id, addr, 4);
}

void sii_drv_cra_bits_set32(uint32_t dev_id, uint16_t addr, uint32_t mask,
		bool_t set)
{
	uint32_t temp;

	temp = (uint32_t) reg_read(dev_id, addr, 4);
	temp = BITS_SET(temp, mask, set);
	reg_write(dev_id, addr, 4, temp);
}

void sii_drv_cra_bits_mod32(uint32_t dev_id, uint16_t addr, uint32_t mask,
		uint32_t val)
{
	uint32_t temp;

	temp = (uint32_t) reg_read(dev_id, addr, 4);
	temp = BITS_MOD(temp, mask, val);
	reg_write(dev_id, addr, 4, temp);
}

void sii_drv_cra_fifo_write8(uint32_t dev_id, uint16_t addr,
		const uint8_t *p_data, uint16_t size)
{
	while (size--) {
		reg_write(dev_id, addr, 1, *p_data);
		p_data++;
	}
}

void sii_drv_cra_fifo_read8(uint32_t dev_id, uint16_t addr, uint8_t *p_data,
		uint16_t size)
{
	while (size--) {
		*p_data = (uint8_t) reg_read(dev_id, addr, 1);
		p_data++;
	}
}

void sii_drv_cra_block_write8(uint32_t dev_id, uint16_t addr,
		const uint8_t *p_data, uint16_t size)
{
	sii_platform_host_block_write(dev_id, addr, p_data, size);
}

void sii_drv_cra_block_read8(uint32_t dev_id, uint16_t addr, uint8_t *p_data,
		uint16_t size)
{
	sii_platform_host_block_read(dev_id, addr, p_data, size);
}

/***** local functions *******************************************************/

static void int32_to_data(uint8_t bytes, uint8_t *p_arr, uint32_t val)
{
	while (bytes--) {
		*p_arr = (uint8_t) val;
		p_arr++;
		val >>= 8;
	}
}

static uint32_t data_to_int32(uint8_t bytes, uint8_t *p_arr)
{
	uint32_t val = 0;

	p_arr += bytes;
	while (bytes--) {
		val <<= 8;
		p_arr--;
		val += *p_arr;
	}
	return val;
}

static void reg_write(uint32_t dev_id, uint16_t addr, uint8_t bytes,
		uint32_t val)
{
	uint8_t tmp[4];

	int32_to_data(bytes, tmp, val);
	sii_platform_host_block_write(dev_id, addr, tmp, bytes);
}

static uint32_t reg_read(uint32_t dev_id, uint16_t addr, uint8_t bytes)
{
	uint8_t tmp[4];

	sii_platform_host_block_read(dev_id, addr, tmp, bytes);
	return data_to_int32(bytes, tmp);
}

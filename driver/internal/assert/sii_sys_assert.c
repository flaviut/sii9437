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
 * @file sii_sys_assert.c
 *
 * @brief Assertion check library
 *
 *****************************************************************************/

#include "sii_sys_assert_api.h"

bool_t b_assert_status = 0;
uint32_t assert_line_no = 0;
uint8_t assert_module_id = 0;

void *sii_sys_assert(const char *module_str, uint32_t line_no)
{
	/* If the assertion error occurred earlier, don't override
	 * those details */
	if (b_assert_status)
		return NULL;

	assert_line_no = line_no;

	if (module_str) {
		if (strcmp("DrvCra", module_str) == 0)
			assert_module_id = 0x01;
		else if (strcmp("LibEDID", module_str) == 0)
			assert_module_id = 0x02;
		else if (strcmp("lib_obj", module_str) == 0)
			assert_module_id = 0x03;
		else if (strcmp("lib_seq", module_str) == 0)
			assert_module_id = 0x04;
		else if (strcmp("lib_time", module_str) == 0)
			assert_module_id = 0x05;
		else if (strcmp("SiI9437", module_str) == 0)
			assert_module_id = 0x06;
		else if (strcmp("9437RxHal", module_str) == 0)
			assert_module_id = 0x07;
		else if (strcmp("Sii9438", module_str) == 0)
			assert_module_id = 0x08;
		else if (strcmp("9438TxHal", module_str) == 0)
			assert_module_id = 0x09;
		else
			assert_module_id = 0xFF;
	}

	b_assert_status = TRUE;
/*
	SII_LOG1A("", NULL, (" Module - %d, Line No - %d\n",
			assert_module_id, assert_line_no));
*/
	return NULL;
}

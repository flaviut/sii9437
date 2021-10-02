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
 * @file sii_sys_assert_api.h
 *
 * @brief Assertion check library API
 *
 *****************************************************************************/

#ifndef SII_SYS_ASSERT_API_H
#define SII_SYS_ASSERT_API_H

#include <sii_datatypes.h>

#ifdef SII_SYS_ASSERT_ON

#define SII_ASSERT(expr)	\
	((expr) ? (0) : (sii_sys_assert(SII_MODULE_NAME_GET(), __LINE__)))

#else

#ifndef SII_ASSERT
#define SII_ASSERT(expr)
#endif

#endif

extern bool_t b_assert_status;
extern uint32_t assert_line_no;
extern uint8_t assert_module_id;

void *sii_sys_assert(const char *module_str, uint32_t line_no);

#endif /*SII_SYS_ASSERT_API_H*/

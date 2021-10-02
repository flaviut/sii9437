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
 * @file sii_sys_malloc.c
 *
 * @brief Dynamic memory allocation from static memory pool
 *
 *****************************************************************************/

#ifndef SI_SYS_MALLOC_API_H
#define SI_SYS_MALLOC_API_H

/***** #include statements ***************************************************/

#include "sii_datatypes.h"

/***** public functions ******************************************************/

void *sii_sys_malloc_create(uint16_t size);
void sii_sys_malloc_delete(void *p);
uint16_t sii_sys_malloc_bytes_allocated_get(void);
void sii_sys_malloc_lock(void);
void sii_sys_malloc_delete_all(void);

#endif /* SI_SYS_MALLOC_API_H */

/***** end of file ***********************************************************/

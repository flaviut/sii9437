/******************************************************************************
 *
 * 2016 (c) Lattice Semiconductor Corporation
 * 2018 (c) Sonos, Inc
 *
 * This program is free software; you can redistribute it and/or
 * modify it only under the terms of version 2 of the GNU General Public License
 * as published by the Free Software Foundation, and not any later version.
 *
 * This program is distributed AS IS WITHOUT ANY WARRANTY of any kind,
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

#ifndef SII_DATATYPES_H
#define SII_DATATYPES_H

#ifndef __KERNEL__
#error "This file is for building Linux kernel modules only"
#endif


#include <linux/types.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/bug.h>
#include <linux/stddef.h>


#ifndef INST_NULL
#define INST_NULL			((SiiInst_t)0)
#endif

#define SII_INST2OBJ(inst)		((void *)inst)
#define SII_OBJ2INST(pObj)		((sii_inst_t)pObj)
#define SII_INST_NULL			((sii_inst_t)0)

#define SII_BIT0			0x01
#define SII_BIT1			0x02
#define SII_BIT2			0x04
#define SII_BIT3			0x08
#define SII_BIT4			0x10
#define SII_BIT5			0x20
#define SII_BIT6			0x40
#define SII_BIT7			0x80
#define SII_BIT8			0x0100
#define SII_BIT9			0x0200
#define SII_BIT10			0x0400
#define SII_BIT11			0x0800
#define SII_BIT12			0x1000
#define SII_BIT13			0x2000
#define SII_BIT14			0x4000
#define SII_BIT15			0x8000

#define SET_BITS			(0xFF)
#define CLEAR_BITS			(0x00)

typedef signed int			int_t;
typedef u32				SiiRetVal_t;

typedef void *sii_inst_t;

typedef bool bool_t;
#define TRUE	true
#define FALSE	false


#if (SII_ENV_BUILD_ASSERT)
#define SII_PLATFORM_DEBUG_ASSERT(expr) WARN_ON(expr)
#else
#define SII_PLATFORM_DEBUG_ASSERT(expr) ((void)0)
#endif

#define SII_MEMCPY(pdes, psrc, size)		memcpy(pdes, psrc, size)
#define SII_MEMSET(pdes, value, size)		memset(pdes, value, size)
#define SII_STRLEN(str)				strlen(str)
#define SII_ASSERT(expr)			SII_PLATFORM_DEBUG_ASSERT(expr)
#define SII_VSPRINTF(dstr, size, fstr, arg)	vsprintf(dstr, fstr, arg)
#define SII_SPRINTF(str, size, frm, arg)	sprintf(str, frm, arg)
#define SII_STRCPY(dst, src, size)		strlcpy(dst, src, size)
#define SII_STRCAT(dst, src, size)		strlcat(dst, src, size)
#define SII_STRCMP(dst, src)			strcmp(dst,src)

#endif


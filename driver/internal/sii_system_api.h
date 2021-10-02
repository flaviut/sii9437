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
 * @file sii_system_api.h
 *
 * @brief Generic system library interface
 *
 *****************************************************************************/
#ifndef SI_SYSTEM_API_H
#define SI_SYSTEM_API_H

/***** #include statements ***************************************************/

#include "sii_datatypes.h"

/***** public macro definitions **********************************************/

/* Construct to stringify defines */
#define SII_STRINGIFY(x)	#x
#define SII_DEF2STR(x)		SII_STRINGIFY(x)

/* #if 0 */
#ifdef SII_DEBUG
	#if (0 < SII_DEBUG)
		#define SII_MODULE_NAME_SET(name)	\
			static const char *sii_module_name_str = \
						SII_DEF2STR(name)
		#define SII_MODULE_NAME_GET()	sii_module_name_str
	#else
		#define SII_MODULE_NAME_SET(name)
		#define SII_MODULE_NAME_GET()	(NULL)
	#endif /* (0 < SII_DEBUG) */
#else
	#define SII_MODULE_NAME_SET(name)
	#define SII_MODULE_NAME_GET()	(NULL)
#endif /* SII_DEBUG */
/* #endif */

/* Bit manipulation macros */
#define SII_SET_BIT(p, bit)		(*(p) |= (1<<(bit)))
#define SII_CLR_BIT(p, bit)		(*(p) &= (~(1<<(bit))))
#define SII_PUT_BIT(p, bit, b)		(*(p) = (b) ? (*(p)|(1<<(bit))) : \
							(*(p)&(~(1<<(bit)))))
#define SII_MOD_BIT(p, msk, val)	(*(p) = ((*(p))&(~(msk))) | \
							((val)&(msk)))

#define SII_ABS_DIFF(A, B)		(((A) > (B)) ? ((A)-(B)) : ((B)-(A)))
#define SII_UINT_FRAC(num, den)		(((((num)<<1) / (den)) + 1) >> 1)
#define SII_INT_FRAC(num, den)		((0 > (((int)num)*((int)den))) ? \
					((((((int)num)*2)/((int)den))-1)/2) : \
					((((((int)num)*2)/((int)den))+1)/2))
#define SII_FLOAT2INT(flt)		((0.0 > flt) ? ((int_t)(flt-0.5)) : \
							((int_t)(flt+0.5)))

#endif /* SI_SYSTEM_API_H */

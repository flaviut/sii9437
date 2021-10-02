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
 * @file sii_datatypes.h
 *
 * @brief Common data types
 *
 *****************************************************************************/

#ifndef SII_DATATYPES_H
#define SII_DATATYPES_H

#ifdef __cplusplus
extern "C"{
#endif

/***** #include statements ***************************************************/

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <NUC123Series.h>

/***** public macro definitions **********************************************/
#define SII_ENV_BUILD_ASSERT	0

#ifndef NULL
#define NULL	((void *)0)
#endif

#ifndef INST_NULL
#define INST_NULL                            ((SiiInst_t)0)
#endif

#define SII_INST2OBJ(inst)                   ((void *)inst)
#define SII_OBJ2INST(pObj)                   ((sii_inst_t)pObj)
#define SII_INST_NULL                        ((sii_inst_t)0)

/***** public type definitions ***********************************************/
#define SII_BIT0                         0x01
#define SII_BIT1                         0x02
#define SII_BIT2                         0x04
#define SII_BIT3                         0x08
#define SII_BIT4                         0x10
#define SII_BIT5                         0x20
#define SII_BIT6                         0x40
#define SII_BIT7                         0x80
#define SII_BIT8                         0x0100
#define SII_BIT9                         0x0200
#define SII_BIT10                        0x0400
#define SII_BIT11                        0x0800
#define SII_BIT12                        0x1000
#define SII_BIT13                        0x2000
#define SII_BIT14                        0x4000
#define SII_BIT15                        0x8000

#define SET_BITS                         (0xFF)
#define CLEAR_BITS                       (0x00)

typedef signed int			int_t;
typedef uint32_t			SiiRetVal_t;

#if __STDC_VERSION__ >= 199901L
	#include <stdint.h>
#else
	typedef unsigned char                    uint8_t;
	typedef unsigned short                   uint16_t;
	typedef unsigned long int                uint32_t;
	typedef signed char                      int8_t;
	typedef signed short                     int16_t;
	typedef signed long int                  int32_t;
#endif

/**
* @brief Instance type
*/
typedef void *sii_inst_t;

/**
* @brief C++ -like Boolean type
*/
#define true TRUE
#define false FALSE
#define bool_t BOOL

/***** standard functions ****************************************************/

/* extern void SiiPlatformAssert( const char* pFileStr, uint32_t lineNo ); */
void SiiPlatformDebugAssert(const char *pFileName, uint32_t lineNumber,
		uint32_t expressionEvaluation, const char *pConditionText);

#if (SII_ENV_BUILD_ASSERT)
/**
* @brief Assertion macro to check for internal error conditions
*/
#define SII_PLATFORM_DEBUG_ASSERT(expr) \
	((void)((/*lint -e{506}*/!(expr)) ? \
		SiiPlatformDebugAssert(__FILE__, __LINE__,\
		(uint32_t)(expr), NULL), ((void)1) : ((void)0)))
#else /* SII_ENV_BUILD_ASSERT */
/**
* @brief Dummy assertion macro
*/
#define SII_PLATFORM_DEBUG_ASSERT(expr) ((void)0)
#endif /* SII_ENV_BUILD_ASSERT */

#define SII_MEMCPY(pdes, psrc, size)		memcpy(pdes, psrc, size)
#define SII_MEMSET(pdes, value, size)		memset(pdes, value, size)
#define SII_STRLEN(str)				strlen(str)
/* #define SII_ASSERT(expr)			\
 *	( (expr) ? ((void)0) : (SiiPlatformAssert(__FILE__, __LINE__)) ) */
/*#define SII_ASSERT(expr)	SII_PLATFORM_DEBUG_ASSERT(expr)*/
 #define SII_VSPRINTF(dstr, size, fstr, arg)	\
 	vsprintf(dstr, fstr, arg)
 #define SII_SPRINTF(str, size, frm, arg)		\
 	sprintf(str, frm, arg)

#define SII_STRCPY(dst, src, size)	strlcpy(dst, src, size)
#define SII_STRCAT(dst, src, size)	strlcat(dst, src, size)
#define SII_STRCMP(dst, src)		strcmp(dst,src)

#ifdef __cplusplus
}
#endif

#endif /* SII_DATATYPES_H */


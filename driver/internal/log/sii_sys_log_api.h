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
 * @file sii_sys_log_api.h
 *
 * @brief logging library
 *
 *****************************************************************************/

#ifndef SI_SYS_LOG_API_H
#define SI_SYS_LOG_API_H

/***** #include statements ***************************************************/

#include "sii_datatypes.h"
#include "sii_sys_obj_api.h"
#include "sii_lib_pipe_api.h"

/***** public macro definitions **********************************************/

#define SII_LOG_ENABLE(lev)	(lev <= SII_DEBUG)

#define SII_LOG_HEADER(func, obj, str)	\
	{ sii_sys_log_timestamp(sii_module_name_str, \
	(char *)func, (void *)(obj)); sii_sys_log_printf str; }
#define SII_LOG_STRING(str)		{ sii_sys_log_printf str; }
#define SII_LOG_ARRAY(p, l)		\
	{ uint16_t _len = l; uint8_t *ptr = (uint8_t *)p;\
	while (_len--) {\
		sii_sys_log_printf("%02X ", (uint16_t)(*(ptr++) & 0xFF)); };\
	sii_sys_log_printf("\n"); }

#ifndef SII_DEBUG
	#define SII_LOG1A(func, obj, str)
	#define SII_LOG2A(func, obj, str)
	#define SII_LOG3A(func, obj, str)
	#define SII_LOG1B(str)
	#define SII_LOG2B(str)
	#define SII_LOG3B(str)
	#define SII_LOG1B_ARRAY(p, len)
	#define SII_LOG2B_ARRAY(p, len)
	#define SII_LOG3B_ARRAY(p, len)
	#define SII_LOG1_ENABLE	(FALSE)
	#define SII_LOG2_ENABLE	(FALSE)
	#define SII_LOG3_ENABLE	(FALSE)
#else
  #if (0 == SII_DEBUG)
    #define SII_LOG1A(func, obj, str)
    #define SII_LOG2A(func, obj, str)
    #define SII_LOG3A(func, obj, str)
    #define SII_LOG1B(str)
    #define SII_LOG2B(str)
    #define SII_LOG3B(str)
    #define SII_LOG1B_ARRAY(p, len)
    #define SII_LOG2B_ARRAY(p, len)
    #define SII_LOG3B_ARRAY(p, len)
    #define SII_LOG1_ENABLE                      (FALSE)
    #define SII_LOG2_ENABLE                      (FALSE)
    #define SII_LOG3_ENABLE                      (FALSE)
  #elif(1 == SII_DEBUG)
    #define SII_LOG1A(func, obj, str)            SII_LOG_HEADER(func, obj, str)
    #define SII_LOG2A(func, obj, str)
    #define SII_LOG3A(func, obj, str)
    #define SII_LOG1B(str)                       SII_LOG_STRING(str)
    #define SII_LOG2B(str)
    #define SII_LOG3B(str)
    #define SII_LOG1B_ARRAY(p, len)              SII_LOG_ARRAY(p, len)
    #define SII_LOG2B_ARRAY(p, len)
    #define SII_LOG3B_ARRAY(p, len)
    #define SII_LOG1_ENABLE                      (TRUE)
    #define SII_LOG2_ENABLE                      (FALSE)
    #define SII_LOG3_ENABLE                      (FALSE)
  #elif(2 == SII_DEBUG)
    #define SII_LOG1A(func, obj, str)            SII_LOG_HEADER(func, obj, str)
    #define SII_LOG2A(func, obj, str)            SII_LOG_HEADER(func, obj, str)
    #define SII_LOG3A(func, obj, str)
    #define SII_LOG1B(str)                       SII_LOG_STRING(str)
    #define SII_LOG2B(str)                       SII_LOG_STRING(str)
    #define SII_LOG3B(str)
    #define SII_LOG1B_ARRAY(p, len)              SII_LOG_ARRAY(p, len)
    #define SII_LOG2B_ARRAY(p, len)              SII_LOG_ARRAY(p, len)
    #define SII_LOG3B_ARRAY(p, len)
    #define SII_LOG1_ENABLE                      (TRUE)
    #define SII_LOG2_ENABLE                      (TRUE)
    #define SII_LOG3_ENABLE                      (FALSE)
  #else
    #define SII_LOG1A(func, obj, str)            SII_LOG_HEADER(func, obj, str)
    #define SII_LOG2A(func, obj, str)            SII_LOG_HEADER(func, obj, str)
    #define SII_LOG3A(func, obj, str)            SII_LOG_HEADER(func, obj, str)
    #define SII_LOG1B(str)                       SII_LOG_STRING(str)
    #define SII_LOG2B(str)                       SII_LOG_STRING(str)
    #define SII_LOG3B(str)                       SII_LOG_STRING(str)
    #define SII_LOG1B_ARRAY(p, len)              SII_LOG_ARRAY(p, len)
    #define SII_LOG2B_ARRAY(p, len)              SII_LOG_ARRAY(p, len)
    #define SII_LOG3B_ARRAY(p, len)              SII_LOG_ARRAY(p, len)
    #define SII_LOG1_ENABLE                      (TRUE)
    #define SII_LOG2_ENABLE                      (TRUE)
    #define SII_LOG3_ENABLE                      (TRUE)
  #endif
#endif

/***** public type definitions ***********************************************/

/***** public functions ******************************************************/
#ifdef SII_LOG_FIFO
void sii_sys_log_set_pipe(sii_inst_t log_queue);
#endif
void sii_sys_log_timestamp(const char *class_str,
	const char *func_str, void *p_obj);
void sii_sys_log_printf(const char *fmt_str, ...);

#endif /* SI_SYS_LOG_API_H */

/***** end of file ***********************************************************/

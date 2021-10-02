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
 * @file sii_sys_time_api.h
 *
 * @brief Time Library
 *
 *****************************************************************************/

#ifndef SI_SYS_TIME_API_H
#define SI_SYS_TIME_API_H

/***** #include statements ***************************************************/

#include "sii_datatypes.h"

/***** public functions ******************************************************/

/******************************************************************************/
/**
* @brief	Calculates time difference between t1 and t2 and prevent
*		result from roll-over corruption (t1<t2).
*
* @param[in]	t1 - Number of milli seconds
* @param[in]	t2 - Number of milli seconds
*
* @return	Number of milli seconds.
*
*******************************************************************************/
uint32_t sii_sys_time_milli_diff_get(uint32_t t1, uint32_t t2);

/******************************************************************************/
/**
* @brief	Configures a milli time-out object. This object can be used
*		with 'sii_sys_time_out_milli_is' to find out if 'milli_to'
*		object has been expired.
		example:
*		{
*			uint32_t milli_to;
*
*			sii_sys_time_out_milli_set(&milli_to, 100);
*			while(!sii_sys_time_out_milli_is(milli_to))
*			{ .... }
*		}
*
* @param[in]	p_milli_to - pointer to 'milli_to' object.
* @param[in]	time_out - Number of milli seconds.
*
*******************************************************************************/
void sii_sys_time_out_milli_set(uint32_t *p_milli_to, uint32_t time_out);

/******************************************************************************/
/**
* @brief	Finds out if 'milli_to' object has been expired.
*
* @param[in]	p_milli_to - pointer to 'milli_to' object.
*
* @return	TRUE if 'milli_to' object has been expired.
*******************************************************************************/
bool_t sii_sys_time_out_milli_is(const uint32_t *p_milli_to);

/******************************************************************************/
/**
* @brief	Blocks execution for x number of milli seconds.
*
* @param[in]	milli_delay - Number of milli seconds.
*
*******************************************************************************/
void sii_sys_time_milli_delay(uint32_t milli_delay);

#endif /* SI_SYS_TIME_API_H */

/***** end of file ***********************************************************/

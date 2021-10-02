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
 * @file sii_sys_time.c
 *
 * @brief Time library
 *
 *****************************************************************************/
/* #define SII_DEBUG 3 */

/***** #include statements ***************************************************/

#include "sii_system_api.h"
#include "sii_sys_time_api.h"
#include "sii_platform_api.h"
#include "sii_sys_assert_api.h"

/***** Register Module name **************************************************/

SII_MODULE_NAME_SET(lib_time);

/***** local macro definitions ***********************************************/

/* Maximum milli out must be set to less than half the range of
 * SII_SYS_TIME_MILLI_T */
#define MILLI_TO_MAX	(((SII_SYS_TIME_MILLI_T) ~0) >> 1)

/***** local type definitions ************************************************/

/***** local prototypes ******************************************************/

static void s_time_out_milli_set(SII_SYS_TIME_MILLI_T *p_milli_to,
		SII_SYS_TIME_MILLI_T time_out);
static bool_t s_time_out_milli_is(const SII_SYS_TIME_MILLI_T *p_milli_to);

/***** local data objects ****************************************************/

/***** public functions ******************************************************/

SII_SYS_TIME_MILLI_T sii_sys_time_milli_diff_get(SII_SYS_TIME_MILLI_T t1,
		SII_SYS_TIME_MILLI_T t2)
{
	if (t2 < t1)
		return ((SII_SYS_TIME_MILLI_T) ~0) - t1 + t2 + 1;
	else
		return t2 - t1;
}

void sii_sys_time_out_milli_set(SII_SYS_TIME_MILLI_T *p_milli_to,
		SII_SYS_TIME_MILLI_T time_out)
{
	s_time_out_milli_set(p_milli_to, time_out);
}

bool_t sii_sys_time_out_milli_is(const SII_SYS_TIME_MILLI_T *p_milli_to)
{
	return s_time_out_milli_is(p_milli_to);
}

void sii_sys_time_milli_delay(SII_SYS_TIME_MILLI_T milli_delay)
{
	SII_SYS_TIME_MILLI_T milli_to;

	s_time_out_milli_set(&milli_to, milli_delay);

	while (!s_time_out_milli_is(&milli_to))
		;
}

/***** local functions *******************************************************/

static void s_time_out_milli_set(SII_SYS_TIME_MILLI_T *p_milli_to,
		SII_SYS_TIME_MILLI_T time_out)
{
	SII_ASSERT(MILLI_TO_MAX > time_out);
	*p_milli_to = (SII_SYS_TIME_MILLI_T) sii_platform_time_msec_query()
		+ time_out;
}

static bool_t s_time_out_milli_is(const SII_SYS_TIME_MILLI_T *p_milli_to)
{
	SII_SYS_TIME_MILLI_T milli_new =
			(SII_SYS_TIME_MILLI_T) sii_platform_time_msec_query();
	SII_SYS_TIME_MILLI_T milli_diff =
			(*p_milli_to > milli_new) ?
					(*p_milli_to - milli_new) :
					(milli_new - *p_milli_to);

	if (MILLI_TO_MAX < milli_diff)
		return (*p_milli_to > milli_new) ? (TRUE) : (FALSE);
	else
		return (*p_milli_to <= milli_new) ? (TRUE) : (FALSE);
}

/***** end of file ***********************************************************/

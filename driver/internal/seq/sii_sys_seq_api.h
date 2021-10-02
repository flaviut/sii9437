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
 * @file sii_sys_seq.h
 *
 * @brief Sequencer (provides timers)
 *
 *****************************************************************************/

#ifndef SII_SYS_SEQ_API_H
#define SII_SYS_SEQ_API_H

/***** #include statements ****************************************************/

#include "sii_datatypes.h"
#include "sii_sys_time_api.h"
#include "sii_sys_obj_api.h"

/***** public macro definitions ***********************************************/

#define SII_SYS_SEQ_TIMER_CREATE(pstr, func, inst) \
	sii_sys_seq_timer_create(pstr, func, inst, 200)
#define SII_SYS_SEQ_TIMER_REPEAT(inst, time) \
	sii_sys_seq_timer_start(inst, 0, time)
#define SII_SYS_SEQ_TIMER_ONESHOT(inst, time) \
	sii_sys_seq_timer_start(inst, time, 0)

/***** public type definitions ************************************************/

#define		sii_sys_seq_time_ms_t		uint16_t
#define		sii_sys_obj_list_inst_t		sii_inst_t

/**
 * Callback function to be run on timer expiration
 */
typedef	void	(*sii_sys_seq_timer_callback_func)(sii_inst_t inst);

/**
 * Callback function to get/set timer value to be used by the timers
 */
typedef void	(*sii_sys_seq_timer_value_callback)(sii_inst_t inst,
		sii_sys_seq_time_ms_t *timer_value,
		bool_t b_set);

/***** public functions *******************************************************/

/******************************************************************************/
/**
* @brief Constructor/Destructor for sequencer
*
*******************************************************************************/
sii_inst_t sii_sys_seq_create(sii_inst_t dev_inst,
		sii_sys_seq_timer_value_callback timer_value_cb_func);
void sii_sys_seq_delete(sii_inst_t seq_inst);

/******************************************************************************/
/**
* @brief Scheduler suspend/resume control
*
* @param[in]	seq_inst	Instance of sequencer
* @param[in]	b_set		TRUE - Enable suspend mode
*				FALSE - Resume scheduler
*
*******************************************************************************/
void sii_sys_seq_suspend_set(sii_inst_t seq_inst, bool_t b_set);

/******************************************************************************/
/**
* @brief Suspend suspend/resume status
*
* @param[in]	seq_inst	Instance of sequencer
* @retval	Suspend status	TRUE or FALSE
*
*******************************************************************************/
bool_t sii_sys_seq_suspended_is(sii_inst_t seq_inst);

/******************************************************************************/
/**
* @brief Create timer instance
*
* @param[in]	seq_inst	Instance of sequencer
* @param[in]	name_str	Name of timer
* @param[in]	callback_func	Function pointer of local handler function
* @param[in]	parent_inst	Pointer reference to parent object
*
* @return	Timer instance
*
*******************************************************************************/
sii_inst_t sii_sys_seq_timer_create(sii_inst_t seq_inst,
		const char *name_str,
		sii_sys_seq_timer_callback_func callback_func,
		sii_inst_t parent_inst);

/******************************************************************************/
/**
* @brief Destroy timer instance
*
* @param[in]	timer_inst	Reference to timer instance
*
*******************************************************************************/
void sii_sys_seq_timer_delete(sii_inst_t timer_inst);

/******************************************************************************/
/**
* @brief Start timer instance
*
* @param[in]	timer_inst	Reference to timer instance
* @param[in]	time_ms		Timer interval between this Start function and
*				next call-back function.
* @param[in]	period_ms	Time interval of periodic timer
*				(use '0' if one-shot timer)
*
*******************************************************************************/
void sii_sys_seq_timer_start(sii_inst_t timer_inst,
		sii_sys_seq_time_ms_t time_ms,
		sii_sys_seq_time_ms_t period_ms);

/******************************************************************************/
/**
* @brief Stop timer instance
*
* @param[in]	timer_inst	Reference to timer instance
*
*******************************************************************************/
void sii_sys_seq_timer_stop(sii_inst_t timer_inst);

/******************************************************************************/
/**
* @brief Timer running status
*
* @param[in]	timer_inst		Reference to timer instance
* @retval	Timer run status	TRUE or FALSE
*
*******************************************************************************/
bool_t sii_sys_seq_timer_running_is(sii_inst_t timer_inst);

/******************************************************************************/
/**
* @brief Timer period set
*
* @param[in]	timer_inst	Reference to timer instance
* @param[in]	period_ms	Timer period in ms
*
*******************************************************************************/
void sii_sys_seq_timer_period_set(sii_inst_t timer_inst,
		sii_sys_seq_time_ms_t period_ms);

/******************************************************************************/
/**
* @brief Timer period get
*
* @param[in]	timer_inst	Reference to timer instance
*
* @retval	Timer period
*
*******************************************************************************/
sii_sys_seq_time_ms_t sii_sys_seq_timer_period_get(sii_inst_t timer_inst);

/******************************************************************************/
/**
* @brief Sequence handler to be run on timer expiration
*
** @param[in]	seq_inst	Instance of sequencer
**
*******************************************************************************/
void sii_sys_seq_handle(sii_inst_t seq_inst);

#endif /* SII_SYS_SEQ_API_H */

/***** end of file ***********************************************************/

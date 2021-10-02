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
 * @file sii_sys_seq.c
 *
 * @brief Sequencer (provides timers)
 *
 *****************************************************************************/
#define SII_DEBUG 1

/***** #include statements ***************************************************/

#include "sii_system_api.h"
#include "sii_sys_obj_api.h"
#include "sii_sys_seq_api.h"
#include "sii_sys_log_api.h"
#include "sii_sys_assert_api.h"

/***** Register Module name **************************************************/

SII_MODULE_NAME_SET(lib_seq);

/***** local macro definitions ***********************************************/

#define STOP_VALUE	((sii_sys_seq_time_ms_t) -1)

/***** local type definitions ************************************************/

struct timer_obj_t {

	/* Sequencer instance to with the timer obj belongs to */
	sii_inst_t	seq_inst;

	/* Function pointer to timer callback */
	sii_sys_seq_timer_callback_func	callback_func;

	/* Periodic time in msec. set by user */
	sii_sys_seq_time_ms_t	time_periodic;

	/* Previous time sample */
	sii_sys_seq_time_ms_t	time_prev;

	/* Remaining time left in msec */
	sii_sys_seq_time_ms_t	time_run;

	/* Flag to indicate whether timer has expired. Used for
	 * running timer callback routine */
	bool_t b_expired_is;
};

struct seq_obj_t {
	/* Instance of Device for which timer sequence is being created */
	sii_inst_t			dev_inst;

	/* Reference to linked-list of timers instantiations */
	sii_sys_obj_list_inst_t		timer_list_inst;

	/* Reference to current processed timer instance */
	struct timer_obj_t		*p_curr_timer_obj;

	/* Flag to indicate whether sequence is started */
	bool_t				b_is_started;

	/* Flag to indicate whether seq is suspended or enabled */
	bool_t				b_wake;

	/* Callback to set/get the timer value */
	sii_sys_seq_timer_value_callback	timer_value_callback_func;
};

/***** local prototypes ******************************************************/

static void s_sequence_handler(sii_inst_t seq_inst, bool_t b_timer_expired);

/***** public functions ******************************************************/

sii_inst_t sii_sys_seq_create(sii_inst_t dev_inst,
	sii_sys_seq_timer_value_callback timer_value_cb_func)
{
	struct seq_obj_t	*p_seq_obj;

	/* Allocate memory for object */
	p_seq_obj = (struct seq_obj_t *)sii_sys_obj_singleton_create(
			SII_MODULE_NAME_GET(),
			SII_INST_NULL, sizeof(struct seq_obj_t));
	SII_ASSERT(p_seq_obj);

	/* Create list for timers */
	SII_ASSERT(!p_seq_obj->timer_list_inst);
	p_seq_obj->timer_list_inst = sii_sys_obj_list_create(
			SII_MODULE_NAME_GET(),
			sizeof(struct timer_obj_t));
	SII_ASSERT(p_seq_obj->timer_list_inst);

	p_seq_obj->dev_inst = dev_inst;
	p_seq_obj->timer_value_callback_func = timer_value_cb_func;

	/* If the timer value callback is not provided, suspend the timer
	 * sequence as timers can't be run */
	p_seq_obj->b_wake = p_seq_obj->timer_value_callback_func ?
				TRUE : FALSE;
	p_seq_obj->p_curr_timer_obj = NULL;
	p_seq_obj->b_is_started   = FALSE;

	return SII_OBJ2INST(p_seq_obj);
}

void sii_sys_seq_delete(sii_inst_t seq_inst)
{
	struct seq_obj_t	*p_seq_obj = (struct seq_obj_t *) seq_inst;
	SII_ASSERT(p_seq_obj);

	/* Delete list of objects */
	sii_sys_obj_list_delete(p_seq_obj->timer_list_inst);
	p_seq_obj->timer_list_inst = SII_INST_NULL;

	sii_sys_obj_singleton_delete(p_seq_obj);
}

void sii_sys_seq_suspend_set(sii_inst_t seq_inst, bool_t b_set)
{
	struct seq_obj_t	*p_seq_obj = (struct seq_obj_t *) seq_inst;
	SII_ASSERT(p_seq_obj);
	p_seq_obj->b_wake = !b_set;
}

sii_inst_t sii_sys_seq_timer_create(sii_inst_t seq_inst,
		const char *timer_name_str,
		sii_sys_seq_timer_callback_func callback_func,
		sii_inst_t parent_inst)
{
	struct seq_obj_t	*p_seq_obj = (struct seq_obj_t *) seq_inst;
	struct timer_obj_t *p_timer_obj = NULL;

	SII_ASSERT(p_seq_obj);
	SII_ASSERT(callback_func);

	/* Allocate memory for object */
	p_timer_obj = (struct timer_obj_t *)sii_sys_obj_instance_create(
			p_seq_obj->timer_list_inst,
			parent_inst, timer_name_str);
	SII_ASSERT(p_timer_obj);

	/* Initialise instance */
	p_timer_obj->seq_inst = seq_inst;
	p_timer_obj->callback_func = callback_func;
	p_timer_obj->time_periodic = 0;
	p_timer_obj->time_prev = 0;
	p_timer_obj->time_run = STOP_VALUE;
	p_timer_obj->b_expired_is = FALSE;

	return SII_OBJ2INST(p_timer_obj);
}

void sii_sys_seq_timer_delete(sii_inst_t timer_inst)
{
	struct seq_obj_t	*p_seq_obj = NULL;
	struct timer_obj_t	*p_timer_obj = NULL;

	SII_ASSERT(timer_inst);
	if (!timer_inst)
		return;

	p_seq_obj = (struct seq_obj_t *)
		((struct timer_obj_t *)SII_INST2OBJ(timer_inst))->seq_inst;
	SII_ASSERT(p_seq_obj);
	if (!p_seq_obj)
		return;

	p_timer_obj = (struct timer_obj_t *)
		SII_SYS_OBJ_CHECK(p_seq_obj->timer_list_inst, timer_inst);
	SII_ASSERT(p_timer_obj);

	/* De-allocate instantiation memory */
	sii_sys_obj_instance_delete(p_timer_obj);
}

void sii_sys_seq_timer_start(sii_inst_t timer_inst,
		sii_sys_seq_time_ms_t timeMs,
		sii_sys_seq_time_ms_t periodMs)
{
	struct seq_obj_t	*p_seq_obj = NULL;
	struct timer_obj_t	*p_timer_obj = NULL;

	SII_ASSERT(timer_inst);
	if (!timer_inst)
		return;

	p_seq_obj = (struct seq_obj_t *)
		((struct timer_obj_t *)SII_INST2OBJ(timer_inst))->seq_inst;
	SII_ASSERT(p_seq_obj);
	if (!p_seq_obj)
		return;

	p_timer_obj = (struct timer_obj_t *)
		SII_SYS_OBJ_CHECK(p_seq_obj->timer_list_inst, timer_inst);
	SII_ASSERT(p_timer_obj);

	/* Skip starting the timer if the timer sequence is suspended or
	 * if the timer is set to run for 0ms */
	if (!p_seq_obj->b_wake || !timeMs)
		return;

	p_timer_obj->time_prev = 0;
	p_timer_obj->time_run = timeMs;
	p_timer_obj->time_periodic = periodMs;
	s_sequence_handler(p_seq_obj, FALSE);
}

void sii_sys_seq_timer_stop(sii_inst_t timer_inst)
{
	struct seq_obj_t	*p_seq_obj = NULL;
	struct timer_obj_t	*p_timer_obj = NULL;

	SII_ASSERT(timer_inst);
	if (!timer_inst)
		return;

	p_seq_obj = (struct seq_obj_t *)
		((struct timer_obj_t *)SII_INST2OBJ(timer_inst))->seq_inst;
	SII_ASSERT(p_seq_obj);
	if (!p_seq_obj)
		return;

	p_timer_obj = (struct timer_obj_t *)
		SII_SYS_OBJ_CHECK(p_seq_obj->timer_list_inst, timer_inst);
	SII_ASSERT(p_timer_obj);

	p_timer_obj->time_run = STOP_VALUE;

	s_sequence_handler(p_seq_obj, FALSE);
}

bool_t sii_sys_seq_timer_running_is(sii_inst_t timer_inst)
{
	struct seq_obj_t	*p_seq_obj = NULL;
	struct timer_obj_t	*p_timer_obj = NULL;

	SII_ASSERT(timer_inst);
	if (!timer_inst)
		return FALSE;

	p_seq_obj = (struct seq_obj_t *)
		((struct timer_obj_t *)SII_INST2OBJ(timer_inst))->seq_inst;
	SII_ASSERT(p_seq_obj);
	if (!p_seq_obj)
		return FALSE;

	p_timer_obj = (struct timer_obj_t *)
		SII_SYS_OBJ_CHECK(p_seq_obj->timer_list_inst, timer_inst);
	SII_ASSERT(p_timer_obj);

	return (STOP_VALUE == p_timer_obj->time_run) ? (FALSE) : (TRUE);
}

void sii_sys_seq_timer_period_set(sii_inst_t timer_inst,
		sii_sys_seq_time_ms_t period_ms)
{
	struct seq_obj_t	*p_seq_obj = NULL;
	struct timer_obj_t	*p_timer_obj = NULL;

	SII_ASSERT(timer_inst);
	if (!timer_inst)
		return;

	p_seq_obj = (struct seq_obj_t *)
		((struct timer_obj_t *)SII_INST2OBJ(timer_inst))->seq_inst;
	SII_ASSERT(p_seq_obj);
	if (!p_seq_obj)
		return;

	p_timer_obj = (struct timer_obj_t *)
		SII_SYS_OBJ_CHECK(p_seq_obj->timer_list_inst, timer_inst);
	SII_ASSERT(p_timer_obj);

	p_timer_obj->time_periodic = period_ms;
	s_sequence_handler(p_seq_obj, FALSE);
}

sii_sys_seq_time_ms_t sii_sys_seq_timer_period_get(sii_inst_t timer_inst)
{
	struct seq_obj_t	*p_seq_obj = NULL;
	struct timer_obj_t	*p_timer_obj = NULL;

	SII_ASSERT(timer_inst);
	if (!timer_inst)
		return 0;

	p_seq_obj = (struct seq_obj_t *)
		((struct timer_obj_t *)SII_INST2OBJ(timer_inst))->seq_inst;
	SII_ASSERT(p_seq_obj);
	if (!p_seq_obj)
		return 0;

	p_timer_obj = (struct timer_obj_t *)
		SII_SYS_OBJ_CHECK(p_seq_obj->timer_list_inst, timer_inst);
	SII_ASSERT(p_timer_obj);

	return p_timer_obj->time_periodic;
}

void sii_sys_seq_handle(sii_inst_t seq_inst)
{
	struct seq_obj_t	*p_seq_obj = (struct seq_obj_t *) seq_inst;

	SII_ASSERT(p_seq_obj);

	s_sequence_handler(p_seq_obj, TRUE);
}

/***** local functions *******************************************************/

/* Timer interrupt handler */
static void s_sequence_handler(sii_inst_t seq_inst, bool_t b_timer_expired)
{
	struct seq_obj_t	*p_seq_obj = (struct seq_obj_t *) seq_inst;
	struct timer_obj_t	*p_timer_obj = NULL;
	struct timer_obj_t	*p_next_timer = SII_INST_NULL;
	sii_sys_seq_time_ms_t	time_new = 0;
	sii_sys_seq_time_ms_t	time_wakeup = STOP_VALUE;

	SII_ASSERT(p_seq_obj);

	if (!p_seq_obj)
		return;

	p_timer_obj = (struct timer_obj_t *)
		sii_sys_obj_first_get(p_seq_obj->timer_list_inst);
	SII_ASSERT(p_timer_obj);

	SII_LOG1A("", NULL, ("Timer sequence handler\n"));
	if (!p_timer_obj)
		return;

	/* If timer expiration happened, stop the current timer */
	if (b_timer_expired) {

		SII_LOG1A("", NULL, ("Handling timer expired event\n"));

		/* Check if there is any running timer, if not exit */
		if (!p_seq_obj->p_curr_timer_obj)
			return;

		if (STOP_VALUE > p_seq_obj->p_curr_timer_obj->time_run) {
			SII_LOG1A("", NULL,
				("Timer expired - %s\n",
				sii_sys_obj_name_get((void *)
				p_seq_obj->p_curr_timer_obj)));
			p_seq_obj->p_curr_timer_obj->time_run = STOP_VALUE;
			p_seq_obj->p_curr_timer_obj->b_expired_is = TRUE;
			p_seq_obj->b_is_started = FALSE;
			p_seq_obj->p_curr_timer_obj = SII_INST_NULL;
		}

	} else if (p_seq_obj->b_is_started) {
		p_seq_obj->timer_value_callback_func(
				p_seq_obj->dev_inst, &time_new, FALSE);
		SII_LOG1A("", NULL, ("Current timer value - %dms\n", time_new));
	}

	while (p_timer_obj) {
		if (STOP_VALUE > p_timer_obj->time_run) {
			sii_sys_seq_time_ms_t timeDiff;

			/* If it is a new timer added, timediff will be 0*/
			if (p_timer_obj->time_prev == 0)
				timeDiff = 0;
			else
				timeDiff = p_timer_obj->time_prev - time_new;

			SII_LOG1A("", NULL,
				("Time diff from prev timer value: %s - %dms\n",
				sii_sys_obj_name_get((void *)p_timer_obj),
				timeDiff));

			/* Update previous time */
			p_timer_obj->time_prev = time_new;

			if (p_timer_obj->time_run > timeDiff) {
				/*Update Running Time*/
				p_timer_obj->time_run -= timeDiff;

				SII_LOG1A("", NULL,
				("Remaining timer run time: %s - %dms\n",
				sii_sys_obj_name_get((void *)p_timer_obj),
					p_timer_obj->time_run));

				/*Find time of earliest timer that get expired*/
				if (p_timer_obj->time_run < time_wakeup) {
					time_wakeup = p_timer_obj->time_run;
					p_next_timer = p_timer_obj;
					SII_LOG1A("", NULL,
					("Timer updated to run for %s - %dms\n",
					sii_sys_obj_name_get(
						(void *)p_timer_obj),
						time_wakeup));
				}
			} else {
				/* Another timer got expired */
				p_timer_obj->time_run = STOP_VALUE;
				p_timer_obj->time_prev = 0;
				p_timer_obj->b_expired_is = TRUE;
				SII_LOG1A("", NULL,
					("Timer expired - %s\n",
					sii_sys_obj_name_get(
						(void *)p_timer_obj)));
			}
		}

		/*Go to next timer instance*/
		p_timer_obj = (struct timer_obj_t *)
				sii_sys_obj_next_get(p_timer_obj);
	}

	/* Update if there is a new timer value */
	if ((time_wakeup < STOP_VALUE) && (p_next_timer != SII_INST_NULL)) {
		if (p_next_timer != p_seq_obj->p_curr_timer_obj) {
			p_seq_obj->p_curr_timer_obj = p_next_timer;
			p_seq_obj->timer_value_callback_func(
				p_seq_obj->dev_inst,
				&time_wakeup, TRUE);
			p_seq_obj->b_is_started = TRUE;
		} else {
			SII_LOG1A("", NULL,
				("Keeping the current timer value %s - %dms\n",
				sii_sys_obj_name_get((void *)p_next_timer),
				p_next_timer->time_run));
		}
	} else {
		sii_sys_seq_time_ms_t	time_wakeup_temp = 0;
		p_seq_obj->b_is_started = FALSE;
		p_seq_obj->p_curr_timer_obj = SII_INST_NULL;
		p_seq_obj->timer_value_callback_func(
				p_seq_obj->dev_inst,
				&time_wakeup_temp, TRUE);
	}

	/* Update prev time of each timer to the new timeout value */
	if (p_seq_obj->b_is_started) {
		p_timer_obj = (struct timer_obj_t *) sii_sys_obj_first_get(
				p_seq_obj->timer_list_inst);
		while (p_timer_obj) {
			if (STOP_VALUE > p_timer_obj->time_run) {
				p_timer_obj->time_prev = time_wakeup;
				SII_LOG1A("", NULL,
				(" Time prev of %s updated to - %d\n",
				sii_sys_obj_name_get((void *)p_timer_obj),
				time_wakeup));
			}
			p_timer_obj = (struct timer_obj_t *)
					sii_sys_obj_next_get(p_timer_obj);
		}
	}

	/* Run callback routines for all expired timers */
	p_timer_obj = (struct timer_obj_t *) sii_sys_obj_first_get(
					p_seq_obj->timer_list_inst);
	while (p_timer_obj) {
		if (p_timer_obj->b_expired_is) {
			p_timer_obj->b_expired_is = FALSE;
			p_timer_obj->callback_func(sii_sys_obj_parent_inst_get(
					p_timer_obj));
		}
		p_timer_obj = (struct timer_obj_t *)
				sii_sys_obj_next_get(p_timer_obj);
	}
}

/***** end of file ***********************************************************/

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
#include <sii_platform_api.h>
#include <sii_sys_log_api.h>
#include "sii_datatypes.h"
#include <stdio.h>
#include <CoOS.h>
#include <OsTask.h>
#include <NUC123Series.h>
#include <stdint.h>
#include <GPIO.h>
#include <string.h>
#include <I2C.h>
#include <I2C_MASTER.h>
#include <Vcom_Data_Proc.h>
#include <CLOCK.h>
#include <SYS.h>
#include <GPIO.h>
#include <FMC.h>
#include <Host_Core.h>
#include <LibTimer.h>
#include "sii943x_user_def.h"

#define EPERM 1
#define CO_FAILURE (-1)
#define CO_SUCCESS  0
/* CoOS does not have signals since it is RTOS */
#define SIGINT  0
#define SIGTERM 0
#define SIGKILL 0

typedef BOOL bool;
typedef int            prefuint_t; /* Platform-specific efficient integers */
typedef unsigned int   uint_t;
typedef OS_MutexID spinlock_t;
typedef OS_TID     wait_queue_head_t;
typedef void irqreturn_t;
typedef uint8_t u8;

struct semaphore {
	OS_MutexID resource_count;

};

/* Timers related */
struct sii_timer {
	OS_TCID  timerId;
	uint32_t time_msec;
	bool     periodicity;
	void (*callback_handler)(void *callback_param);
	void *callback_param;
	uint8_t timer_thread_wake;
	struct sii_timer *next;
};


/* FIXME COOCOX */
#define GPIOF_IN FALSE
#define GPIOF_OUT_INIT_LOW FALSE
#define GPIOF_OUT_INIT_LOW FALSE
#define GPIOF_OUT_INIT_HIGH (1 << 1)

/* CoOS RTOS does not have signalling */
#define allow_signal(signal)

#define TASK_RET_TYPE void
#define TASK_STRUCT wait_queue_head_t
#define WAIT_QUEUE_HEAD_T wait_queue_head_t
#define WORK_STRUCT OS_TID

#define WORK_QUEUE_STRUCT OS_TID

#define TASK_ARG_TYPE  void

struct completion {
	unsigned int done;
	WAIT_QUEUE_HEAD_T wait;
};
struct timer_list {
	unsigned long data;
};

struct hrtimer {
	unsigned long state;
};

struct mutex {
	/* 1: unlocked, 0: locked, negative: locked, possible waiters */
	/*atomic_t                count; */
	spinlock_t              wait_lock;
	struct list_head        wait_list;
#if defined(CONFIG_DEBUG_MUTEXES) || defined(CONFIG_MUTEX_SPIN_ON_OWNER)
	TASK_STRUCT *owner;
#endif
#ifdef CONFIG_MUTEX_SPIN_ON_OWNER
	struct optimistic_spin_queue osq; /* Spinner MCS lock */
#endif
	#ifdef CONFIG_DEBUG_MUTEXES
	void                    *magic;
#endif
#ifdef CONFIG_DEBUG_LOCK_ALLOC
	struct lockdep_map      dep_map;
#endif
};


bool KheapCreated = FALSE;
extern void   CoCreateKheap(void);

/***** local function definitions ***********************************************/
static uint32_t sii_i2c_slave_addr_get(uint32_t dev_id, uint16_t addr,
				uint8_t *i2c_slave_addr,
				uint8_t *offset_size);

/***** public functions ******************************************************/

uint32_t sii_platform_host_block_write( uint32_t dev_id, uint16_t addr,
		const uint8_t *p_data, uint16_t size )
{
	uint8_t offset_size = 1;
	uint8_t i2c_slave_addr;
	BOOL CoRet;

	if (sii_i2c_slave_addr_get(dev_id, addr, &i2c_slave_addr, &offset_size))
	{
		return CO_FAILURE;
	}

	CoRet = sii_i2c_write(i2c_slave_addr, addr, offset_size, p_data, size);

	if(!CoRet) {
		return CO_FAILURE;
	} else {
		return CO_SUCCESS;
	}
}

uint32_t sii_platform_host_block_read( uint32_t dev_id, uint16_t addr,
		uint8_t *p_data, uint16_t size )
{
	uint8_t offset_size = 1;
	uint8_t i2c_slave_addr;
	BOOL CoRet;

	if (sii_i2c_slave_addr_get(dev_id, addr, &i2c_slave_addr, &offset_size))
	{
		return CO_FAILURE;
	}

	CoRet = sii_i2c_read(i2c_slave_addr, addr, offset_size, p_data, size);

	if(!CoRet) {
		return CO_FAILURE;
	} else {
		return CO_SUCCESS;
	}
}

void sii_platform_device_reset_set( uint32_t dev_id, bool_t b_on )
{
	sii_platform_gpio_set(dev_id, SII_UC_RST,  !(b_on));
}

static uint32_t sii_i2c_slave_addr_get(uint32_t dev_id, uint16_t addr,
				uint8_t *i2c_slave_addr,
				uint8_t *offset_size)
{
	uint32_t ret_val = 0;

	switch (dev_id) {
	case SII9437_INST_1:
		switch ((addr >> 8) & 0xff)
		{
		case PAGE_0:
			*i2c_slave_addr = SII9437_INST_1_I2C_ADDR_PAGE0;
			break;
		case PAGE_1:
			*i2c_slave_addr = SII9437_INST_1_I2C_ADDR_PAGE1;
			break;
		case PAGE_2:
			*i2c_slave_addr = SII9437_INST_1_I2C_ADDR_PAGE2;
			break;
		default:
			ret_val = CO_FAILURE;
			break;
		}
		*offset_size = 1;
		break;
	case SII9437_INST_2:
		break;
	case SII9438_INST_1:
		switch ((addr >> 8) & 0xff)
		{
		case PAGE_0:
			*i2c_slave_addr = SII9438_INST_1_I2C_ADDR_PAGE0;
			break;
		case PAGE_1:
			*i2c_slave_addr = SII9438_INST_1_I2C_ADDR_PAGE1;
			break;
		case PAGE_2:
			*i2c_slave_addr = SII9438_INST_1_I2C_ADDR_PAGE2;
			break;
		case PAGE_3:
			*i2c_slave_addr = SII9438_INST_1_I2C_ADDR_PAGE3;
			break;
		case PAGE_4:
			*i2c_slave_addr = SII9438_INST_1_I2C_ADDR_PAGE4;
			break;
		default:
			ret_val = CO_FAILURE;
			break;
		}
		*offset_size = 1;
		break;
	case SII9438_INST_2:
		break;
	case SII9612_RX_INST_1:
		*i2c_slave_addr = SII9612_I2C_ADDRESS_RX;
		*offset_size = 2;
		break;
	case SII9612_TX_INST_1:
		*i2c_slave_addr = SII9612_I2C_ADDRESS_TX;
		*offset_size = 2;
		break;
	default:
		ret_val = CO_FAILURE;
		break;
	}
	return ret_val;
}

/* Dynamic Memory Management Wrappers */
void *sii_memalloc(uint32_t dev_id, uint32_t Size)
{
	uint8_t *ptr;
	if(!KheapCreated) {
		CoCreateKheap();
		KheapCreated = TRUE;
	}

	ptr = CoKmalloc(Size);
	memset(ptr, 0, Size);

	return (void *) ptr;
}

uint32_t sii_memfree(uint32_t dev_id, void *p)
{
	CoKfree(p);
	return CO_SUCCESS;
}

/* Dynamic Memory Management Wrappers */
void *kzalloc(uint32_t Size, uint8_t flag)
{
	uint8_t *ptr;
	if(!KheapCreated) {
		CoCreateKheap();
		KheapCreated = TRUE;
	}

	ptr = CoKmalloc(Size);
	memset(ptr, 0, Size);

	return (void *) ptr;
}

void kfree(void *p)
{
	CoKfree(p);
	return;
}

void sii_platform_i2c_init(struct i2c_adapter *adapter)
{
	/* Initialize I2C Port - 400K */
	I2C_Port_Open(adapter->port, adapter->clock);
	return;
}

uint32_t sii_platform_gpio_config(uint32_t dev_id, uint32_t gpio,
		enum sii_platform_gpio_direction direction)
{
	GPIO_T *port;

	switch (gpio & GPIO_PORT_MASK ) {
	case PA_BASE:
		port = PA;
		break;
	case PB_BASE:
		port = PB;
		break;
	case PC_BASE:
		port = PC;
		break;
	case PD_BASE:
		port = PD;
		break;
	case PF_BASE:
		port = PF;
		break;
	default:
		return CO_FAILURE;
	}

	GPIO_SET_MODE(port, ((gpio & GPIO_PIN_MASK) >> 2), (uint32_t) direction);
	return CO_SUCCESS;
}

uint32_t sii_platform_gpio_set(uint32_t dev_id, uint32_t gpio, bool_t level)
{
	if(level)
		GPIO_SET_PIN(gpio);
	else
		GPIO_CLR_PIN(gpio);

	return CO_SUCCESS;
}

bool_t sii_platform_gpio_status_get(uint32_t dev_id, uint32_t gpio)
{
	if (GPIO_GET_PIN(gpio))
		return TRUE;
	else
		return FALSE;
}

uint32_t sii_platform_time_msec_query( void )
{
#ifdef SII_BUILD_COOS
	return ((uint32_t) CoGetOSTime());
#else
	return LibTimerGetTick();
#endif
}

void sii_platform_log_print(const char *message)
{
    printf(message);
}

/* Mutex Related Functions */
void mutex_init(void *mutex)
{
	OS_MutexID *Comtx;
	Comtx = (OS_MutexID *)mutex;
	*Comtx = CoCreateMutex ();
	if(E_CREATE_FAIL == *Comtx) {
		SII_LOG1A("", NULL, (" Mutex Creation failed\n"));
	}
	return;
}
void mutex_lock(void *mutex)
{
	if (CoEnterMutexSection(*((OS_MutexID *)mutex)) != E_OK) {
		SII_LOG1A("", NULL,
				(" Mutex CoEnterMutexSection lock ERROR\n"));
	}
	return;
}
void mutex_unlock(void *mutex)
{
	if (CoLeaveMutexSection(*((OS_MutexID *)mutex)) != E_OK) {
		SII_LOG1A("", NULL,
				(" Mutex CoLeaveMutexSection unlock ERROR\n"));
	}
	return;
}


/* Delay Function */
void sii_platform_sleep_msec(uint32_t millisec)
{
#ifdef SII_BUILD_COOS
	U8 hour, minute, sec;
	U16 millisecond;

	/* convert to millisec, sec, min and hour */
	millisecond = millisec % 1000;
	sec = millisec/1000;
	minute = sec/60;
	hour = minute/60;
	CoTimeDelay(hour, minute, sec, millisecond);
#else
	uint32_t u32TimerTickVal, u32TimerTickVal1;
	uint32_t timediff;
	u32TimerTickVal = LibTimerGetTick();

	do {
		u32TimerTickVal1 = LibTimerGetTick();
		if(u32TimerTickVal1 >= u32TimerTickVal)
			timediff = u32TimerTickVal1 - u32TimerTickVal;
		else
			timediff = u32TimerTickVal - u32TimerTickVal1;
	} while (timediff < millisec);
#endif
	return;
}

int sii_timer_create(void (*callback_handler)(void *callback_param),
	void *callback_param, void **timer_handle, uint32_t time_msec,
	bool periodicity)
{
	struct sii_timer *tmr;

	tmr =  kzalloc(sizeof(*tmr), GFP_KERNEL);
	if (tmr == NULL) {
		SII_LOG1A("", NULL, (" Invalid Timer Instance\n"));
		return -ENOMEM;
	}

	if(periodicity) {
		periodicity = TMR_TYPE_PERIODIC;
	}
	else {
		periodicity = TMR_TYPE_ONE_SHOT;
	}

	/*time_msec /= 10;*/

	tmr->timerId =  CoCreateTmr(periodicity,
		time_msec, time_msec,
		(FUNCPtr)callback_handler,
		callback_param);
	if(tmr->timerId > CFG_MAX_TMR)
		return E_CREATE_FAIL;

	tmr->time_msec   = time_msec;
	tmr->periodicity = periodicity;
	tmr->timer_thread_wake = 0;
	tmr->callback_handler = callback_handler;
	tmr->callback_param = callback_param;
	tmr->next = NULL;
	*timer_handle = tmr;
	return 0;
}

int sii_timer_start(void **timer_handle)
{
	StatusType status;
	struct sii_timer *tmr = (struct sii_timer *)*timer_handle;

	if (!tmr) {
		SII_LOG1A("", NULL, (" Invalid Timer Instance\n"));
		return -ENODEV;
	}

	status = CoStartTmr(tmr->timerId);

	if(E_OK != status) {
		return CO_FAILURE;
	}
	else {
		return CO_SUCCESS;
	}
}

int sii_timer_stop(void **timer_handle)
{
	int status;
	StatusType Costat;
	struct sii_timer *tmr = (struct sii_timer *)*timer_handle;

	if (!tmr) {
		SII_LOG1A("", NULL, (" Invalid Timer Instance\n"));
		return -ENODEV;
	}

	tmr->timer_thread_wake = 0;
	Costat = CoStopTmr(tmr->timerId);
	if(E_OK != Costat) {
		status = CO_FAILURE;
	}
	else {
		status = CO_SUCCESS;
	}
	return status;
}

int sii_timer_delete(void **timer_handler)
{
	int status = 0;
	StatusType Costat;
	struct sii_timer *tmr = (struct sii_timer *)*timer_handler;

	if (!tmr) {
		SII_LOG1A("", NULL, (" Invalid Timer Instance\n"));
		return -ENODEV;
	}
	tmr->timer_thread_wake = 0;
	Costat = CoDelTmr(tmr->timerId);
	if(E_OK != Costat) {
		status = CO_FAILURE;
	}
	else {
		status = CO_SUCCESS;
	}
	kfree(tmr);
	return status;
}

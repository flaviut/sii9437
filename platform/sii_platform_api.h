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
 * @file sii_platform_api.h
 *
 * @brief Minimum Platform API function set for Sii9437/Sii9438 Driver
 *
 *****************************************************************************/

#ifndef SII_PLATFORM_API_H
#define SII_PLATFORM_API_H

#ifdef __cplusplus
extern "C"{
#endif

/***** #include statements ***************************************************/

#ifdef BUILD_NUC123
#include <stdio.h>
#endif /* BUILD_NUC123 */
#include "sii_datatypes.h"

/***** public macro definitions ***********************************************/

#if SII943X_USER_DEF__MULTI_THREAD
#define SII_OSAL_MUTEX_CREATE(dev_id)
#define SII_OSAL_MUTEX_DELETE(p_mutex)
#define SII_OSAL_MUTEX_SET(p_mutex)
#define SII_OSAL_MUTEX_CLR(p_mutex)
#error "SII_OSAL_MUTEX macros are undefined"
#else
#define SII_OSAL_MUTEX_CREATE(dev_id)
#define SII_OSAL_MUTEX_DELETE(p_mutex)
#define SII_OSAL_MUTEX_SET(p_mutex)
#define SII_OSAL_MUTEX_CLR(p_mutex)
#endif

#ifdef BUILD_NUC123
#define GPIO_PORT_MASK	(PA_BASE | PB_BASE | PC_BASE | PD_BASE | PF_BASE)
#define GPIO_PIN_MASK	0x0000003C

/* GPIO used to Reset Sii943x and SiI9612 */
#define SII_UC_RST		PD0
#endif /* BUILD_NUC123 */

/***** public type definitions ***********************************************/

struct sii_platform_interface {
	uint32_t dev_id;
};

#define SII_SYS_TIME_MILLI_T	uint32_t

/**
* @brief Platform operation (read/write/platform initialisation) status
*/
enum {
	/* !< Platform operation successful */
	SII_PLATFORM_STATUS__SUCCESS = 0,
	/* !< platform operation failed */
	SII_PLATFORM_STATUS__FAILED = (-1)
};

/**
* @brief GPIO direction
*/
enum sii_platform_gpio_direction {
	SII_PLATFORM_GPIO_DIRECTION__INPUT, /* !< GPIO pin as Input */
	SII_PLATFORM_GPIO_DIRECTION__OUTPUT,/* !< GPIO pin as output */
	SII_PLATFORM_GPIO_DIRECTION__OPEN_DRAIN,/* !< GPIO pin as Open drain */
	SII_PLATFORM_GPIO_DIRECTION__INOUT, /* !< GPIO pin as bi-directional */
};

enum {
	PAGE_0	= 0x00,
	PAGE_1	= 0x01,
	PAGE_2	= 0x02,
	PAGE_3	= 0x03,
	PAGE_4	= 0x04,
	PAGE_5	= 0x05
};

/* SiI9438 Driver Instance Device IDs and associated Slave IDs*/
#define SII9437_INST_1		1
#ifdef SII_BUILD_FPGA
#define	SII9437_INST_1_I2C_ADDR_PAGE0	0x62 /* main page */
#else
#define	SII9437_INST_1_I2C_ADDR_PAGE0	0x60 /* main page */
#endif
#define	SII9437_INST_1_I2C_ADDR_PAGE1	0x6E /* eArcRxAddr */
#define	SII9437_INST_1_I2C_ADDR_PAGE2	0x64 /* phyAddr */

/*--------------------------------------------------------------*/
#define SII9437_INST_2		3

/*--------------------------------------------------------------*/
/* SiI9438 Driver Instance Device IDs and associated Slave IDs */
#define SII9438_INST_1		2

#define	SII9438_INST_1_I2C_ADDR_PAGE0	0x60 /* main page */
#define	SII9438_INST_1_I2C_ADDR_PAGE1	0x68 /* swTpiAddr */
#define	SII9438_INST_1_I2C_ADDR_PAGE2	0x6C /* hwTpiAddr */
#define	SII9438_INST_1_I2C_ADDR_PAGE3	0x6A /* eArcTxAddr */
#ifdef SII_BUILD_FPGA
#define	SII9438_INST_1_I2C_ADDR_PAGE4	0x54 /* phyAddr */
#else
#define	SII9438_INST_1_I2C_ADDR_PAGE4	0x64 /* phyAddr */
#endif
/*--------------------------------------------------------------*/
#define SII9438_INST_2		4

/*--------------------------------------------------------------*/
/* SiI9612 Rx Driver Instance Device IDs and associated Slave IDs*/
#define SII9612_RX_INST_1	0x30
/* Phalanx Rx Driver Instance Device IDs */
#define SII9612_I2C_ADDRESS_RX			0x30

/*--------------------------------------------------------------*/
/* SiI9612 Tx Driver Instance Device IDs and associated Slave IDs*/
#define SII9612_TX_INST_1	0x32
#ifdef SII_BUILD_FPGA
#define SII9612_I2C_ADDRESS_TX			0x32
#else
#define SII9612_I2C_ADDRESS_TX			0x30
#endif
/*--------------------------------------------------------------*/
#define SII9438_OFFSET_SIZE_BYTES		1
#define SII9612_OFFSET_SIZE_BYTES		2

#define SII_I2C_ADDR				0x60

#if 0
/* SONOS: Unused and collides with Linux kernel declarations */
#define IRQ_HANDLED
#define IRQ_NONE
#endif /* 0 */

#define SII_DRIVER_NAME "sii9438"
#define COMPATIBLE_NAME "simg,sii9438"

#if 0
/* SONOS: Unused */
struct list_node {
	uint8_t data[28];
	uint16_t header;
	struct list_node *link;
};
#endif /* 0 */

#if 0
/* SONOS: Unused and duplicates sii_memalloc and sii_memfree */
/* Dynamic Memory Management APIs */
void *sii_mem_alloc(uint32_t Size);
void sii_mem_free(void *p);
#endif /* 0 */

#if 0
/* SONOS: Unused and collides with Linux kernel declarations */
#undef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
/*
#define container_of(ptr, type, member) ({                      \
	const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
	(type *)( (char *)__mptr - offsetof(type,member) );})
*/
#define container_of(ptr, type, member)    \
	((type *)((char *)ptr - offsetof(type, member)));

#define list_add_tail(a, b) do {} while (0)
#define list_del(a) do {} while (0)
#define list_for_each_entry(a, b, c) while (0)

#define PRINT_LOGS

#if defined(PRINT_LOGS)
/* #define printf */
#define pr_cmd(format, ...)     do {} while (0)
#define pr_info(format, ...)    tsprintf(format, ## __VA_ARGS__)

#define pr_debug(format, ...)   do {} while (0)

#define pr_warn(format, ...)    tsprintf(format, ## __VA_ARGS__)

#define pr_info_timer(format, ...)   do {} while (0)
#define pr_debug_timer(format, ...)  do {} while (0)
#define pr_debug1(format, ...)       tsprintf(format, ## __VA_ARGS__)
#define pr_debug2(format, ...)       do {} while (0)

#define dev_err(format, ...)         do {} while (0)

#define pr_intr(format, ...)    tsprintf(format, ## __VA_ARGS__)

#else

#define pr_cmd(format, ...)     do {} while (0)
#define pr_info(format, ...)    do {} while (0)

#define pr_debug(format, ...)   do {} while (0)

#define pr_warn(format, ...) do {} while (0)

#define pr_info_timer(format, ...)   do {} while (0)
#define pr_debug_timer(format, ...)  do {} while (0)
#define pr_debug1(format, ...)       do {} while (0)
#define pr_debug2(format, ...)       do {} while (0)

#define dev_err(format, ...)         do {} while (0)

#define pr_intr(format, ...)    do {} while (0)

#endif


struct workqueue_struct {
	uint8_t dummy;
};

struct device {
	void *dev_context;
};

static __inline int test_bit(uint32_t nr, unsigned long *addr)
{
	return 0x00000001 & ((*addr) >> nr);
}
static __inline void clear_bit(uint32_t nr, unsigned long *addr)
{
	(*addr) &= ~(0x1 << nr);
	return;
}
static __inline void set_bit(uint32_t nr, unsigned long *addr)
{
	(*addr) |= (0x1 << nr);
}

void msleep(uint32_t millisec);
#endif /* 0 */

#define MAX_THREAD_STACK_SIZE 256

#define SUCCESS 0
#define FAIL    (-1)

#if 0
/* SONOS: Unused and collides with Linux kernel declarations */
#define GFP_KERNEL FALSE

#define EPERM            1      /* Operation not permitted */
#define ENOENT           2      /* No such file or directory */
#define ESRCH            3      /* No such process */
#define EINTR            4      /* Interrupted system call */
#define EIO              5      /* I/O error */
#define ENXIO            6      /* No such device or address */
#define E2BIG            7      /* Argument list too long */
#define ENOEXEC          8      /* Exec format error */
#define EBADF            9      /* Bad file number */
#define ECHILD          10      /* No child processes */
#define EAGAIN          11      /* Try again */
#define ENOMEM          12      /* Out of memory */
#define EACCES          13      /* Permission denied */
#define EFAULT          14      /* Bad address */
#define ENOTBLK         15      /* Block device required */
#define EBUSY           16      /* Device or resource busy */
#define EEXIST          17      /* File exists */
#define EXDEV           18      /* Cross-device link */
#define ENODEV          19      /* No such device */
#define ENOTDIR         20      /* Not a directory */
#define EISDIR          21      /* Is a directory */
#define EINVAL          22      /* Invalid argument */
#define ENFILE          23      /* File table overflow */
#define EMFILE          24      /* Too many open files */
#define ENOTTY          25      /* Not a typewriter */
#define ETXTBSY         26      /* Text file busy */
#define EFBIG           27      /* File too large */
#define ENOSPC          28      /* No space left on device */
#define ESPIPE          29      /* Illegal seek */
#define EROFS           30      /* Read-only file system */
#define EMLINK          31      /* Too many links */
#define EPIPE           32      /* Broken pipe */
#define EDOM            33      /* Math argument out of domain of func */
#define ERANGE          34      /* Math result not representable */

extern unsigned long volatile jiffies;
#endif /* 0 */

#if 0
/* SONOS: Unused */
#define ANSI_ESC_RESET_TEXT "\x1b[0m"
#define ANSI_ESC_YELLOW_BG "\x1b[43m"
#define ANSI_ESC_WHITE_BG "\x1b[47m"
#define ANSI_ESC_RED_TEXT "\x1b[31m"
#define ANSI_ESC_YELLOW_TEXT "\x1b[33m"
#define ANSI_ESC_GREEN_TEXT "\x1b[32m"
#define ANSI_ESC_BLACK_TEXT "\1b[30m"
#define ANSI_ESC_WHITE_TEXT "\x1b[37m\x1b[1m"
#define ANSI_ESC_MAGENTA_TEXT "\x1b[35m"
#define ANSI_ESC_CYAN_TEXT "\x1b[36m"
#endif /* 0 */

/* Sii9438 Specific */

#if 0
/* SONOS: Unused and collides with Linux kernel declarations */
struct gpio {
	unsigned        gpio;
	unsigned long   flags;
	const char      *label;
};
#endif /* 0 */

#ifdef BUILD_NUC123
/* SONOS: Collides with Linux kernel declarations */
struct list_head {
	struct list_head *next, *prev;
};
#endif /* BUILD_NUC123 */

#if 0
/* SONOS: Unused and collides with Linux kernel declarations */
struct cdev {
	/*struct kobject kobj;
	struct module *owner;
	const struct file_operations *ops; */
	struct list_head list;
	/* dev_t dev; */
	unsigned int count;
};
#endif /* 0 */


/***** public functions ******************************************************/

/*****************************************************************************/
/**
* @brief Blocking sleep function
*
* This function should not exit until ’mSec’ milliseconds of time.
* The sii9436 host driver uses this function for brief delays
* (less than 100 milliseconds).
*
* @param[in]	time_to_sleep	Number of milliseconds that this function
				should wait before it returns.
*
******************************************************************************/
void sii_platform_sleep_msec(uint32_t time_to_sleep);

/*****************************************************************************/
/**
* @brief Time inquiry in milliseconds
*
* Provides time in milliseconds passed since system was powered up.
*
* @retval	Absolute time in milliseconds started from system power up.
*
******************************************************************************/
uint32_t sii_platform_time_msec_query(void);

/*****************************************************************************/
/**
* @brief Time inquiry in microseconds
*
* Provides time in microseconds passed since system was powered up.
*
* @param[out]	Absolute time in seconds started from system power up.
* @param[out]	Microseconds since last seconds
*
******************************************************************************/
void sii_platform_log_time_query(uint32_t *sec, uint32_t *usec);

/*****************************************************************************/
/**
* @brief I2C/SPI Data write function
*
* Implements either I2C or SPI write access to the sii9436 by writing
* ‘size’ bytes to memory starting at ‘addr’. If multiple devices are
* present on the platform ‘devId’ is used by this function to determine
*  which device to access. This function should return ‘0’ if all bytes
* are successfully written and should return ‘-1’ if the write fails.
*
* @param[in]	dev_id	Hardware device ID passed to ‘sii943xCreate()’.
*			This ID is associated with a physical hardware device
*			and is allocated by the user when this function is
*			implemented.
* @param[in]	address		16 bit address of register.
* @param[in]	p_data_out	Pointer to array of bytes that is required to be
*				written to register 'addr'.
* @param[in]	length		Number of bytes in array that is required to be
*				written to device.
*
* @retval	0 : Write was successful
*		-1 : Write has failed
*
******************************************************************************/
uint32_t sii_platform_host_block_write(uint32_t dev_id, uint16_t address,
		const uint8_t *p_data_out, uint16_t length);


/*****************************************************************************/
/**
* @brief I2C/SPI Data read function
*
* Implements either I2C or SPI read access to the sii9436 by reading 'size'
* bytes from memory starting at ‘addr’. If multiple devices are present on
* the platform ‘devId’ is used by this function to determine which device
* to access. This function should return ‘0’ if all bytes are successfully
* written and should return ‘-1’ if the write fails.
*
* @param[in]	dev_id	Hardware device ID passed to ‘sii9436Create()’.
*			This ID is associated with a physical hardware device
*			and is allocated by the user when this function is
*			implemented.
* @param[in]	address		16 bit address of register.
* @param[in]	p_data_in	Pointer to array of bytes that is required to
*				be read from register 'addr'.
* @param[in]	length		Number of bytes in array that is required to be
*				read from device.
*
* @retval               0 : Read was successful
*                      -1 : Read has failed
*
******************************************************************************/
uint32_t sii_platform_host_block_read(uint32_t dev_id, uint16_t address,
		uint8_t *p_data_in,
		uint16_t length);

/*****************************************************************************/
/**
* @brief Device Hardware Reset
*
* Controls level of reset signal connected to device's RST pin.
*
* @param[in]	dev_id		Hardware device ID passed to ‘sii9436Create()’.
*				This ID is associated with a physical hardware
*				device and is allocated by the user when this
*				function is implemented.
*
* @param[in]	on		Requested level of reset signal:
*				FALSE : No reset
*				TRUE  : Active reset
*
******************************************************************************/
void sii_platform_device_reset_set(uint32_t devId, bool_t on);

/**
* @brief API to configure the GPIO pin as input or output are bidirectional.
*
* @param[in]	dev_id		Device Id of the driver instance.
* @param[in]	gpio_pin_mask	GPIO pin Mask
*				Bit 0 - GPIO 0
*				Bit 1 - GPIO 1 ...
* @param[in]	direction	Direction of GPIO pin operation
*
* @retval returns the status of operation
*/
uint32_t sii_platform_gpio_config(uint32_t dev_id, uint32_t gpio_pin_mask,
		enum sii_platform_gpio_direction direction);

/**
* @brief API to drive the GPIO pin.
*
* @param[in]	dev_id		Device Id of the driver instance.
* @param[in]	gpio_pin_mask	Output GPIO pin Mask
*				Bit 0 - GPIO 0
*				Bit 1 - GPIO 1 ...
* @param[in]	level		Output level GPIO pin
*
* @retval returns the status of operation
*/
uint32_t sii_platform_gpio_set(uint32_t dev_id, uint32_t gpio_pin_mask,
		bool_t level);

/**
* @brief API to read the status of GPIO
*
* @param[in]	dev_id		Device Id of the driver instance.
* @param[in]	gpio_pin_mask	input GPIO pin mask
*				Bit 0 - GPIO 0
*				Bit 1 - GPIO 1 ...
*
* @retval	level	Level on the GPIO pin 0 - Low, 1 - High
*/
bool_t sii_platform_gpio_status_get(uint32_t dev_id, uint32_t gpio_pin_mask);

bool_t sii_platform_interrupt_query(uint32_t dev_id);

uint32_t sii_platform_semaphore_create(const char *name_str,
		uint32_t max_count, uint32_t initial_value,
		uint32_t *p_sem_id);
uint32_t sii_platform_semaphore_delete(uint32_t sem_id);

/*****************************************************************************/
/**
* @brief Semaphore-Give wrapper
*
* Optional function that releases ‘SiiPlatformIpcSemaphoreTake()’
*  from being blocked.
* This function is called by sii9436 host driver directly after host driver has
* received an acknowledgement to a message from sii9436 device.
* This function is only called if ‘bIpcSemaphore’ flag in the sii9436Config_t
* structure is is set to ‘TRUE’. If ‘bIpcSemaphore’ is set to ‘FALSE’ an empty
* implementation of this function must be provided to avoid linkage errors.
*  Please refer ‘sii9436rx_PortingGuide’ for guidance on how to implement.
*
******************************************************************************/
uint32_t sii_platform_semaphore_give(uint32_t sem_id);

/*****************************************************************************/
/**
* @brief Semaphore-Take wrapper
*
* Optional function for implementing a semaphore function that blocks
* until either 'SiiPlatformIpcSemaphoreGive()' is called by the sii9436
* host driver or when 'maxBlockTime' expires. This function is called by
* sii9436 host driver directly after host driver has sent a request to
* sii9436 device and is waiting for a response from the sii9436 firmware.
* This function is only called if 'bIpcSemaphore' flag in the sii9436Config_t
* structure is is set to 'TRUE'. If 'bIpcSemaphore' is set to 'FALSE' an empty
* implementation of this function must be provided to avoid linkage errors.
* Please refer ‘sii9436rx_PortingGuide’ for guidance on how to implement.
*
* @param[in]  maxBlockTime    Maximum time in milliseconds that the
*                             semaphore remains blocking
*
* @retval                     Describes reason that semaphore stops blocking:
*                             0 : Released by SiiPlatformIpcSemaphoreGive
*                             1 : Released when maxBlockTime expires
*
******************************************************************************/
uint32_t sii_platform_semaphore_take(uint32_t sem_id, int32_t time_msec);

/*****************************************************************************/
/**
* @brief ASCII string logger
*
* Function for outputting NULL terminated log strings to a platform specific
* logging mechanism such as a UART or log file.
*
* @param[in]	msg_str	Pointer to string to be logged.
*
******************************************************************************/
void sii_platform_log_print(const char *msg_str);

/*****************************************************************************/
/**
* @brief Allocates memory of requested size
*
* @param[in]	dev_id	Hardware device ID passed to 'create()'.
*			This ID is associated with a physical hardware device
*			and is allocated by the user when this function
*			is implemented.
* @param[in]	size	Size of memory required
*
* @returns	pointer to allocated memory
*
*****************************************************************************/
void *sii_memalloc(uint32_t dev_id, uint32_t size);

/*****************************************************************************/
/**
* @brief Frees the  memory specified by the pointer
*
* @param[in]	dev_id	Hardware device ID passed to 'create()'.
*			This ID is associated with a physical hardware device
*			and is allocated by the user when this function
*			is implemented.
* @param	p	Pointer to the memory location to be freed
*
* @returns	Pointer to allocated memory
*
*****************************************************************************/
uint32_t sii_memfree(uint32_t dev_id, void *p);

#ifdef BUILD_NUC123
/* SONOS: Collides with Linux kernel declarations */
/**
 *  @brief Allocates memory of requested size
 *
 *  @param	Size		size of memory required
 *  @param	flag
 *
 *  @returns   pointer to allocated to memory
 *
 *****************************************************************************/

void *kzalloc(uint32_t Size, uint8_t flag);
#endif // BUILD_NUC123

#ifdef __cplusplus
}
#endif

#endif /* SII_PLATFORM_API_H */

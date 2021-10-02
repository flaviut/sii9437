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
 * @file sii_sys_malloc.c
 *
 * @brief Dynamic memory allocation from static memory pool
 *
 *****************************************************************************/
/* #define SII_DEBUG 3 */

/***** #include statements ***************************************************/

#include "sii_system_api.h"
#include "sii_sys_assert_api.h"

/***** Register Module name **************************************************/

SII_MODULE_NAME_SET(lib_malloc);

/***** local macro definitions ***********************************************/

#define MEMPOOL_SIZE_IN_BYTES   (0x0500) /* 1KB + 256 bytes memory pool */
#define MEMPOOL_SIZE_IN_WORDS   ((MEMPOOL_SIZE_IN_BYTES>>2)+2)

/***** local prototypes ******************************************************/

static void memory_clear(uint16_t size, uint16_t ptr);

/***** local data objects ****************************************************/

/* Ensure memory pool location at 4-byte boundary */
static uint32_t mem_pool[MEMPOOL_SIZE_IN_WORDS];

/* Pointer to next available memory location */
static uint16_t mem_ptr;

/* Memory lock */
static bool_t lock = FALSE;

/***** public functions ******************************************************/

void *sii_sys_malloc_create(uint16_t size)
{
	/* Round up to nearest number of words (1 word = 4 bytes) */
	uint16_t words = ((size >> 2) + 1);

	/* Check if memory pool is locked */
	SII_ASSERT(!lock);

	if (MEMPOOL_SIZE_IN_WORDS > (mem_ptr + words)) {
		uint16_t ptr = mem_ptr;

		/* Clear memory */
		memory_clear(words, mem_ptr);

		/* Increase pointer to next available memory location */
		mem_ptr += words;

		return (void *) (&mem_pool[ptr]);
	} else {
		SII_ASSERT(0);
		return NULL;
	}
}

void sii_sys_malloc_delete(void *p)
{
	/* Make sure that delete pointer is in allocated memory space */
	SII_ASSERT((uint32_t *) p >= &mem_pool[0]);
	SII_ASSERT((uint32_t *) p < &mem_pool[mem_ptr]);

	/* Make sure that delete pointer is on a even 4 byte boundary */
	SII_ASSERT(!((((uint8_t *) p) - ((uint8_t *) &mem_pool[0])) % 4));

	mem_ptr = (uint16_t)((((uint8_t *) p) -
			((uint8_t *) &mem_pool[0])) >> 2);
}

uint16_t sii_sys_malloc_bytes_allocated_get(void)
{
	/* Return total amount of bytes allocated */
	return (uint16_t)(mem_ptr << 2);
}

void sii_sys_malloc_lock(void)
{
	lock = TRUE;
}

void sii_sys_malloc_delete_all(void)
{
	lock = FALSE;
	mem_ptr = 0;
}

/***** local functions *******************************************************/

static void memory_clear(uint16_t size, uint16_t ptr)
{
	uint32_t *p_data = &mem_pool[ptr];

	while (size--) {
		*p_data = 0;
		p_data++;
	}
}

/***** end of file ***********************************************************/

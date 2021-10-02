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
/*************************************************************************
* sii_lib_pipe.c
*************************************************************************/
#include "sii_system_api.h"
#include "sii_lib_pipe_api.h"
#include "sii_sys_obj_api.h"
#include "sii_sys_malloc_api.h"
#include "sii_sys_assert_api.h"

/************************************************************************/
/*  Constants Definitions                                               */
/************************************************************************/

/************************************************************************/
/*  Type Definitions                                                    */
/************************************************************************/
struct pipe_obj {
	uint8_t		*p_mem;		/* Pointer to memory pool object */
	uint8_t		obj_size;	/* Size of memory pool in bytes */
	uint16_t	volume;		/* Pipe depth */
	uint16_t	rd_ptr;		/* Read Pointer */
	uint16_t	wr_ptr;		/* Write Pointer */
	bool_t		b_new_data;	/* New data indicator */
};

/************************************************************************/
/*  Local Prototypes                                                    */
/************************************************************************/
static uint16_t s_get_fill_size(struct pipe_obj *p_obj);
static uint16_t s_get_free_size(struct pipe_obj *p_obj);

/************************************************************************/
/*  Application Interface                                               */
/************************************************************************/
sii_inst_t sii_lib_pipe_create(const char *name_str,
		uint16_t volume, uint8_t obj_size)
{
	struct pipe_obj *p_obj = NULL;

	SII_ASSERT(name_str);

	/* Request memory for object */
	p_obj = (struct pipe_obj *)sii_sys_malloc_create
			    (sizeof(struct pipe_obj));
	SII_ASSERT(p_obj);

	/* Initialise Instance */
	p_obj->p_mem  = (uint8_t *)sii_sys_malloc_create(obj_size * volume);
	p_obj->obj_size  = obj_size;
	p_obj->volume  = volume;
	p_obj->rd_ptr = 0;
	p_obj->wr_ptr = 0;
	p_obj->b_new_data = FALSE;

	return SII_OBJ2INST(p_obj);
}

void sii_lib_pipe_delete(sii_inst_t inst)
{
	struct pipe_obj *p_obj = (struct pipe_obj *)inst;

	p_obj->rd_ptr      = 0;
	p_obj->wr_ptr      = 0;
	p_obj->obj_size = 0;
	p_obj->volume = 0;
	sii_sys_malloc_delete(p_obj->p_mem);
	sii_sys_malloc_delete(p_obj);
}

enum sii_lib_pipe_error sii_lib_pipe_write(sii_inst_t inst, void *p_data)
{
	struct pipe_obj *p_obj = (struct pipe_obj *) inst;
	uint8_t *p_src = (uint8_t *) p_data;
	uint8_t *p_des = (uint8_t *) (p_obj->p_mem + p_obj->wr_ptr *
						p_obj->obj_size);

	/* Check for full buffer */
	if (0 == s_get_free_size(p_obj)) {
		uint8_t *p_override_loc;

		/* Increment read pointer */
		if (p_obj->volume > p_obj->rd_ptr)
			p_obj->rd_ptr++;
		else
			p_obj->rd_ptr = 0;

		/* Write '*' to indicate the buffer got truncated due to
		 * override */
		p_override_loc = (uint8_t *)(p_obj->p_mem + p_obj->rd_ptr
			* p_obj->obj_size);

		*p_override_loc = '*';
	}

	/* Copy data object into pipe */
	memcpy(p_des, p_src, p_obj->obj_size);

	/* Set event flag any time when current pipe is empty */
	if (p_obj->rd_ptr == p_obj->wr_ptr)
		p_obj->b_new_data = TRUE;

	/* Increment Write pointer */
	if (p_obj->volume > p_obj->wr_ptr)
		p_obj->wr_ptr++;
	else
		p_obj->wr_ptr = 0;

	return SII_LIB_PIPE_ERROR_NONE;
}

enum sii_lib_pipe_error sii_lib_pipe_read(sii_inst_t inst, void *p_data)
{
	struct pipe_obj *p_obj = (struct pipe_obj *)inst;
	uint8_t *p_src = (uint8_t *)(p_obj->p_mem + p_obj->rd_ptr
		* p_obj->obj_size);
	uint8_t *p_des = (uint8_t *)p_data;

	/* Check for empty buffer */
	if (0 == s_get_fill_size(p_obj))
		return SII_LIB_PIPE_ERROR_EMPTY;

	/* Read data out of pipe */
	SII_MEMCPY(p_des, p_src, p_obj->obj_size);

	/* Increment Write pointer */
	if (p_obj->volume > p_obj->rd_ptr)
		p_obj->rd_ptr++;
	else
		p_obj->rd_ptr = 0;

	return SII_LIB_PIPE_ERROR_NONE;
}

enum sii_lib_pipe_error sii_lib_pipe_check(sii_inst_t inst, void *p_data)
{
	struct pipe_obj *p_obj = (struct pipe_obj *)inst;
	uint8_t *p_src = (uint8_t *)(p_obj->p_mem +
			p_obj->rd_ptr * p_obj->obj_size);
	uint8_t *p_des = (uint8_t *)p_data;

	/* Check for empty buffer */
	if (0 == s_get_fill_size(p_obj))
		return SII_LIB_PIPE_ERROR_EMPTY;

	/* Read data out of pipe */
	memcpy(p_des, p_src, p_obj->obj_size);
	return SII_LIB_PIPE_ERROR_NONE;
}

uint16_t sii_lib_pipe_get_fill_size(sii_inst_t inst)
{
	struct pipe_obj *p_obj = (struct pipe_obj *)inst;

	return s_get_fill_size(p_obj);
}

uint16_t sii_lib_pipe_get_free_size(sii_inst_t inst)
{
	struct pipe_obj *p_obj = (struct pipe_obj *)inst;

	return s_get_free_size(p_obj);
}

uint16_t sii_lib_pipe_get_total_size(sii_inst_t inst)
{
	struct pipe_obj *p_obj = (struct pipe_obj *)inst;

	return p_obj->volume;
}

bool_t sii_lib_pipe_new_data_is(sii_inst_t inst)
{
	struct pipe_obj *p_obj = (struct pipe_obj *)inst;
	bool_t b_new_data = p_obj->b_new_data;

	b_new_data = p_obj->b_new_data;
	if (b_new_data)
		p_obj->b_new_data = FALSE;

	return b_new_data;
}

/************************************************************************/
/*  Local Functions                                                     */
/************************************************************************/
static uint16_t s_get_fill_size(struct pipe_obj *p_obj)
{
	uint16_t wr_ptr = p_obj->wr_ptr;
	uint16_t rd_ptr = p_obj->rd_ptr;

	return (wr_ptr < rd_ptr) ? (p_obj->volume - rd_ptr + wr_ptr + 1) :
		(wr_ptr-rd_ptr);
}

static uint16_t s_get_free_size(struct pipe_obj *p_obj)
{
	return p_obj->volume - s_get_fill_size(p_obj);
}

/** END of File *********************************************************/

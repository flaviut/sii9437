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
 * @file sii_sys_obj.c
 *
 * @brief Object creater
 *
 *****************************************************************************/
/* #define SII_DEBUG 3 */

/***** #include statements ***************************************************/

#include "sii_system_api.h"
#include "sii_sys_obj_api.h"
#include "sii_sys_malloc_api.h"
#include "sii_sys_assert_api.h"

/***** Register Module name **************************************************/

SII_MODULE_NAME_SET(lib_obj);

/***** local macro definitions ***********************************************/

#define LIST_OBJ2INST(p_list_obj)	((void *) SII_OBJ2INST(p_list_obj))
#define LIST_INST2OBJ(list_inst)	\
	((struct lst_obj *) SII_INST2OBJ(list_inst))

#define OBJECT2HEADER(p_void)	\
	((struct hdr_obj *)((char *)p_void-sizeof(struct hdr_obj)))
#define HEADER2OBJECT(p_hdr_obj)	\
	((void *)(((char *)p_hdr_obj)+sizeof(struct hdr_obj)))


/***** local type definitions ************************************************/

struct hdr_obj {
	const char	*inst_str;
	void		*p_list_inst;
	void		*p_parent_inst;
	struct hdr_obj	*p_hdr_obj_next;
};

struct lst_obj {
	const char	*class_str;
	struct hdr_obj	*p_hdr_obj_last;
	struct lst_obj	*p_lst_obj_next;
	uint16_t	obj_size;
};

/***** local prototypes ******************************************************/

static void add_list(struct lst_obj *p_lst_obj_new);
static void remove_list(struct lst_obj *p_lst_obj_del);
static struct hdr_obj *prev_header_get(struct hdr_obj *p_hdr_obj);
static void post_insert_header(struct hdr_obj *p_hdr_obj_des,
	struct hdr_obj *p_hdr_obj_src);
static void insert_header(struct hdr_obj *p_hdr_obj);
static void remove_header(struct hdr_obj *p_hdr_obj);

/***** local data objects ****************************************************/

static struct lst_obj *p_lst_obj_first;

/***** public functions ******************************************************/

void *sii_sys_obj_singleton_create(const char *class_str, void *p_parent_inst,
		uint16_t size)
{
	struct lst_obj *p_lst_obj = NULL;
	void *p_void = NULL;

	/* Create new list */
	p_lst_obj = (struct lst_obj *) sii_sys_malloc_create(
			sizeof(struct lst_obj));

	if (p_lst_obj) {
		struct hdr_obj *p_hdr_obj = NULL;

		p_lst_obj->class_str = class_str;
		p_lst_obj->p_hdr_obj_last = NULL;
		p_lst_obj->p_lst_obj_next = NULL;
		p_lst_obj->obj_size = size;

		/* Create link between each list of instantiations */
		add_list(p_lst_obj);

		/* Request memory for instance header */
		p_hdr_obj = (struct hdr_obj *) sii_sys_malloc_create(
				sizeof(struct hdr_obj) + size);
		if (p_hdr_obj) {
			/* Configure header */
			p_hdr_obj->inst_str = "Singleton";
			p_hdr_obj->p_list_inst = LIST_OBJ2INST(p_lst_obj);
			p_hdr_obj->p_parent_inst = p_parent_inst;
			p_hdr_obj->p_hdr_obj_next = NULL;

			/* Insert instance to linked list */
			insert_header(p_hdr_obj);

			/* Make sure that object is linked-in */
			SII_ASSERT(p_hdr_obj->p_hdr_obj_next);

			p_void = HEADER2OBJECT(p_hdr_obj);
		}
	}
	return p_void;
}

void sii_sys_obj_singleton_delete(void *p_obj)
{
	struct hdr_obj *p_hdr_obj;
	struct lst_obj *p_lst_obj;

	SII_ASSERT(p_obj);
	p_hdr_obj = OBJECT2HEADER(p_obj);
	p_lst_obj = LIST_INST2OBJ(p_hdr_obj->p_list_inst);

	/* Remove instance from linked list */
	SII_ASSERT(p_hdr_obj->p_hdr_obj_next);
	remove_header(p_hdr_obj);
	SII_ASSERT(!p_hdr_obj->p_hdr_obj_next);

	/* Delete instance memory */
	sii_sys_malloc_delete(p_hdr_obj);

	/* Remove list */
	remove_list(p_lst_obj);

	/* Delete list memory */
	sii_sys_malloc_delete(p_lst_obj);
}

void *sii_sys_obj_list_create(const char *class_str, uint16_t size)
{
	struct lst_obj *p_lst_obj;

	/* Create new list */
	p_lst_obj = (struct lst_obj *) sii_sys_malloc_create(
			sizeof(struct lst_obj));
	SII_ASSERT(p_lst_obj);

	if (p_lst_obj) {
		p_lst_obj->class_str = class_str;
		p_lst_obj->p_hdr_obj_last = NULL;
		p_lst_obj->p_lst_obj_next = NULL;
		p_lst_obj->obj_size = size;

		/* Create link between each list of instantiations */
		add_list(p_lst_obj);
	}
	return LIST_OBJ2INST(p_lst_obj);
}

void sii_sys_obj_list_delete(void *p_list_inst)
{
	struct lst_obj *p_lst_obj = LIST_INST2OBJ(p_list_inst);

	SII_ASSERT(p_lst_obj);

	/* Remove all existing linked instances */
	while (p_lst_obj->p_hdr_obj_last) {
		/* Remove instance from linked list */
		remove_header(p_lst_obj->p_hdr_obj_last);

		/* Delete instance memory */
		sii_sys_malloc_delete(p_lst_obj->p_hdr_obj_last);
	}

	/* Remove list */
	remove_list(p_lst_obj);

	/* Delete list memory */
	sii_sys_malloc_delete(p_lst_obj);
}

void *sii_sys_obj_instance_create(void *p_list_inst, void *p_parent_inst,
		const char *inst_str)
{
	struct lst_obj *p_lst_obj = NULL;
	struct hdr_obj *p_hdr_obj = NULL;
	void *p_void = NULL;

	SII_ASSERT(p_list_inst);
	p_lst_obj = LIST_INST2OBJ(p_list_inst);

	/* Request memory for instance header */
	p_hdr_obj = (struct hdr_obj *) sii_sys_malloc_create(
			sizeof(struct hdr_obj) + p_lst_obj->obj_size);
	if (p_hdr_obj) {
		/* Configure header */
		p_hdr_obj->inst_str = inst_str;
		p_hdr_obj->p_list_inst = p_list_inst;
		p_hdr_obj->p_parent_inst = p_parent_inst;
		p_hdr_obj->p_hdr_obj_next = NULL;

		/* Insert instance to linked list */
		insert_header(p_hdr_obj);

		/* Make sure that object is linked-in */
		SII_ASSERT(p_hdr_obj->p_hdr_obj_next);

		p_void = HEADER2OBJECT(p_hdr_obj);
	}
	return p_void;
}

void sii_sys_obj_instance_delete(void *p_obj)
{
	struct hdr_obj *p_hdr_obj;

	SII_ASSERT(p_obj);
	p_hdr_obj = OBJECT2HEADER(p_obj);

	/* Remove instance from linked list */
	SII_ASSERT(p_hdr_obj->p_hdr_obj_next);
	remove_header(p_hdr_obj);
	SII_ASSERT(!p_hdr_obj->p_hdr_obj_next);

	/* Delete instance memory */
	sii_sys_malloc_delete(p_hdr_obj);
}

void *sii_sys_obj_first_get(void *p_list_inst)
{
	struct lst_obj *p_lst_obj = LIST_INST2OBJ(p_list_inst);

	return (p_lst_obj->p_hdr_obj_last) ?
		(HEADER2OBJECT(p_lst_obj->p_hdr_obj_last->p_hdr_obj_next)) :
		(NULL);
}

#if (SII_ENV_BUILD_ASSERT)
bool_t sii_sys_obj_check(void *p_list_inst, void *p_obj)
{
	struct lst_obj *p_lst_obj = NULL;

	SII_ASSERT(p_list_inst);
	p_lst_obj = LIST_INST2OBJ(p_list_inst);
	if (p_lst_obj) {
		struct hdr_obj *p_hdr_obj = OBJECT2HEADER(p_obj);

		/* Check if object is registered to the same list */
		if (p_list_inst == p_hdr_obj->p_list_inst)
			return TRUE;
	}
	return FALSE;
}
#endif

void *sii_sys_obj_parent_inst_get(void *p_obj)
{
	return OBJECT2HEADER(p_obj)->p_parent_inst;
}


const char *sii_sys_obj_list_name_get(void *p_obj)
{
	struct hdr_obj *p_hdr_obj = NULL;
	struct lst_obj *p_lst_obj = NULL;

	SII_ASSERT(p_obj);
	p_hdr_obj = OBJECT2HEADER(p_obj);
	p_lst_obj = LIST_INST2OBJ(p_hdr_obj->p_list_inst);

	return p_lst_obj->class_str;
}

const char *sii_sys_obj_name_get(void *p_obj)
{
	struct hdr_obj *p_hdr_obj = NULL;

	SII_ASSERT(p_obj);
	p_hdr_obj = OBJECT2HEADER(p_obj);
	return p_hdr_obj->inst_str;
}

void *sii_sys_obj_next_get(void *p_obj)
{
	struct hdr_obj *p_hdr_obj = NULL;
	struct lst_obj *p_lst_obj = NULL;

	SII_ASSERT(p_obj);
	p_hdr_obj = OBJECT2HEADER(p_obj);
	p_lst_obj = LIST_INST2OBJ(p_hdr_obj->p_list_inst);
	SII_ASSERT(p_lst_obj);
	if (p_hdr_obj != p_lst_obj->p_hdr_obj_last)
		return HEADER2OBJECT(p_hdr_obj->p_hdr_obj_next);

	/* Reached end of list */
	return NULL;
}

void sii_sys_obj_move(void *p_obj_des, void *p_obj_src)
{
	struct hdr_obj *p_hdr_obj_src = NULL;
	struct lst_obj *p_lst_obj = NULL;

	SII_ASSERT(p_obj_src);

	/* No mvove is needed if pSrc and pDes pointing to the same object */
	if (p_obj_des == p_obj_src)
		return;

	p_hdr_obj_src = OBJECT2HEADER(p_obj_src);
	p_lst_obj = LIST_INST2OBJ(p_hdr_obj_src->p_list_inst);

	/* Make sure that source object is currently linked in */
	SII_ASSERT(p_hdr_obj_src->p_hdr_obj_next);

	/* Temporarily remove source object from linked list */
	remove_header(p_hdr_obj_src);

	/* With a valid destination object provided the source object will be
	 * replaced to directly behind the destination object.
	 * However if null pointer is provided for destination object,
	 * then source object will be moved to the first position of list. */
	if (p_obj_des) {
		struct hdr_obj *p_hdr_obj_des = OBJECT2HEADER(p_obj_des);

		/* Make sure that both instantiations belong to the same
		 * list (type) */
		SII_ASSERT(p_hdr_obj_des->p_list_inst ==
				p_hdr_obj_src->p_list_inst);

		/* Make sure that destination object is a linked in object */
		SII_ASSERT(p_hdr_obj_des->p_hdr_obj_next);

		/* Insert in linked list */
		post_insert_header(p_hdr_obj_des, p_hdr_obj_src);

		/* Update last pointer if inserted at end of list */
		if (p_hdr_obj_des == p_lst_obj->p_hdr_obj_last)
			p_lst_obj->p_hdr_obj_last = p_hdr_obj_src;
	} else {
		if (p_lst_obj->p_hdr_obj_last) {
			/* Insert as first object in list */
			post_insert_header(p_lst_obj->p_hdr_obj_last,
					p_hdr_obj_src);
		} else {
			/* First inserted object should point to itself to
			 * ensure a circular linked list */
			p_hdr_obj_src->p_hdr_obj_next = p_hdr_obj_src;
			p_lst_obj->p_hdr_obj_last = p_hdr_obj_src;
		}
	}
}

/***** local functions *******************************************************/

static void add_list(struct lst_obj *p_lst_obj_new)
{
	if (p_lst_obj_first) {
		struct lst_obj *p_lst_obj = p_lst_obj_first;

		while (p_lst_obj->p_lst_obj_next)
			p_lst_obj = p_lst_obj->p_lst_obj_next;
		p_lst_obj->p_lst_obj_next = p_lst_obj_new;
	} else {
		p_lst_obj_first = p_lst_obj_new;
	}
}

static void remove_list(struct lst_obj *p_lst_obj_del)
{
	/* Make sure that at least one list exists */
	SII_ASSERT(p_lst_obj_first);
	if (p_lst_obj_first == p_lst_obj_del)
		p_lst_obj_first = p_lst_obj_first->p_lst_obj_next;
	else {
		struct lst_obj *p_lst_obj = p_lst_obj_first;

		/* search linked list */
		while (p_lst_obj->p_lst_obj_next) {
			if (p_lst_obj->p_lst_obj_next == p_lst_obj_del)
				break;

			p_lst_obj = p_lst_obj->p_lst_obj_next;

		}
		/* Generate assertion if provided obj-list is not found */
		SII_ASSERT(p_lst_obj->p_lst_obj_next);

		/* remove and relink linked list */
		p_lst_obj->p_lst_obj_next = p_lst_obj_del->p_lst_obj_next;
	}
}

static struct hdr_obj *prev_header_get(struct hdr_obj *p_hdr_obj)
{
	struct hdr_obj *p_hdr_obj_tmp = p_hdr_obj;

	while (p_hdr_obj != p_hdr_obj_tmp->p_hdr_obj_next)
		p_hdr_obj_tmp = p_hdr_obj_tmp->p_hdr_obj_next;

	return p_hdr_obj_tmp;
}

static void post_insert_header(struct hdr_obj *p_hdr_obj_des,
		struct hdr_obj *p_hdr_obj_src)
{
	p_hdr_obj_src->p_hdr_obj_next = p_hdr_obj_des->p_hdr_obj_next;
	p_hdr_obj_des->p_hdr_obj_next = p_hdr_obj_src;
}

static void insert_header(struct hdr_obj *p_hdr_obj)
{
	struct lst_obj *p_lst_obj = NULL;

	SII_ASSERT(p_hdr_obj);

	/* Make sure that new header is un-linked */
	SII_ASSERT(!p_hdr_obj->p_hdr_obj_next);

	p_lst_obj = LIST_INST2OBJ(p_hdr_obj->p_list_inst);
	if (p_lst_obj->p_hdr_obj_last) {
		/* Insert at the end of list of headers */
		post_insert_header(p_lst_obj->p_hdr_obj_last, p_hdr_obj);
	} else {
		/* First inserted object should point to itself to ensure
		 * a circular linked list */
		p_hdr_obj->p_hdr_obj_next = p_hdr_obj;
	}
	p_lst_obj->p_hdr_obj_last = p_hdr_obj;
}

static void remove_header(struct hdr_obj *p_hdr_obj)
{
	struct lst_obj *p_lst_obj = NULL;

	SII_ASSERT(p_hdr_obj);

	/* Make sure that header is currently linked in */
	SII_ASSERT(p_hdr_obj->p_hdr_obj_next);

	p_lst_obj = LIST_INST2OBJ(p_hdr_obj->p_list_inst);

	/* check if this is the last object to be removed */
	if (p_hdr_obj == p_hdr_obj->p_hdr_obj_next) {
		/* Clear reference to linked list */
		p_lst_obj->p_hdr_obj_last = NULL;
	} else {
		struct hdr_obj *p_hdr_obj_prev = prev_header_get(p_hdr_obj);

		/* remove instance out of linked list */
		p_hdr_obj_prev->p_hdr_obj_next = p_hdr_obj->p_hdr_obj_next;

		/* Change last instance reference in case last
		 * instance is removed */
		if (p_lst_obj->p_hdr_obj_last == p_hdr_obj)
			p_lst_obj->p_hdr_obj_last = p_hdr_obj_prev;
	}

	/* Clear next reference to indicate that instance
	 * has been removed from list */
	p_hdr_obj->p_hdr_obj_next = NULL;
}

/***** end of file ***********************************************************/

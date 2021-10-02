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
 * @file sii_sys_obj_api.h
 *
 * @brief Object base class
 *
 *****************************************************************************/

#ifndef SI_SYS_OBJ_API_H
#define SI_SYS_OBJ_API_H

/***** #include statements ***************************************************/

#include "sii_datatypes.h"

/***** public macro definitions **********************************************/

/******************************************************************************/
/**
* @brief	Converts external instantiation reference to pointer of
*		instantiation object as defined locally in module.
*		Creates an assertion if user would provide a reference
*		to a different class of instantiation.
*
* @param[in]	inst	External reference to instantiation.
*
* @return	Pointer to instantiation object.
*
*******************************************************************************/

#ifdef SII_ENV_BUILD_ASSERT
  #if (SII_ENV_BUILD_ASSERT)
    #define SII_SYS_OBJ_CHECK(list_inst, obj_inst)  \
		((sii_sys_obj_check(list_inst, SII_INST2OBJ(obj_inst))) ? \
		(SII_INST2OBJ(obj_inst)) : (SII_ASSERT(0)))
  #else
    #define SII_SYS_OBJ_CHECK(list_inst, obj_inst)	(SII_INST2OBJ(obj_inst))
  #endif
#else
  #define SII_SYS_OBJ_CHECK(list_inst, obj_inst)	(SII_INST2OBJ(obj_inst))
#endif

#define SII_SYS_OBJ_PARENT_INST_GET(child_inst) \
		(sii_sys_obj_parent_inst_get(SII_INST2OBJ(child_inst)))

/***** public functions ******************************************************/

void *sii_sys_obj_singleton_create(const char *class_str,
	void *p_parent_inst, uint16_t size);
void sii_sys_obj_singleton_delete(void *p_obj);

void *sii_sys_obj_list_create(const char *class_str, uint16_t size);
void sii_sys_obj_list_delete(void *p_list_inst);

void *sii_sys_obj_instance_create(void *p_list_inst, void *p_parent_inst,
	const char *inst_str);
void sii_sys_obj_instance_delete(void *p_obj);

void *sii_sys_obj_first_get(void *p_list_inst);
bool_t sii_sys_obj_check(void *p_list_inst, void *p_obj);
void *sii_sys_obj_parent_inst_get(void *p_obj);

const char *sii_sys_obj_list_name_get(void *p_obj);
const char *sii_sys_obj_name_get(void *p_obj);
void *sii_sys_obj_next_get(void *p_obj);
void sii_sys_obj_move(void *p_obj_des, void *p_obj_src);

#endif /* SI_SYS_OBJ_API_H */

/***** end of file ***********************************************************/

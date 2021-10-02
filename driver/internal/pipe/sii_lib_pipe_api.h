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
 * @file sii_lib_pipe_api.h
 *
 * @brief FIFO logging library
 *
 ************************************************************************/
#ifndef SI_LIB_PIPE_API_H
#define SI_LIB_PIPE_API_H

/* Enable Log Queue buffer */
/*#define SII_LOG_QUEUE*/

#include "sii_datatypes.h"

/************************************************************************/
/*  Type Definitions                                                    */
/************************************************************************/
enum sii_lib_pipe_error {
	SII_LIB_PIPE_ERROR_NONE,
	SII_LIB_PIPE_ERROR_EMPTY,
	SII_LIB_PIPE_ERROR_FULL
};

/************************************************************************/
/*	Function Prototype                                              */
/************************************************************************/

sii_inst_t sii_lib_pipe_create(const char *name_str,
		uint16_t volume, uint8_t obj_size);
void sii_lib_pipe_delete(sii_inst_t inst);

enum sii_lib_pipe_error sii_lib_pipe_write(sii_inst_t inst, void *p_data);
enum sii_lib_pipe_error sii_lib_pipe_read(sii_inst_t inst, void *p_data);
enum sii_lib_pipe_error sii_lib_pipe_check(sii_inst_t inst, void *p_data);

uint16_t sii_lib_pipe_get_fill_size(sii_inst_t inst);
uint16_t sii_lib_pipe_get_free_size(sii_inst_t inst);
uint16_t sii_lib_pipe_get_total_size(sii_inst_t inst);

#endif /* SI_LIB_PIPE_API_H */

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
 * @file sii943x_user_def.h
 *
 * @brief User defined variables
 *
 *****************************************************************************/

#ifndef SII943X_USER_DEF_H
#define SII943X_USER_DEF_H

#include "NUC123Series.h"

/*****************************************************************************/
/** All definitions below can be modified by user.
******************************************************************************/

/* Max Sii9437 threads */
#define	SII943X_MAX_THREADS	1

struct i2c_adapter {
	I2C_T *port;
	uint32_t clock;
};

void sii_platform_i2c_init(struct i2c_adapter *adapter);

#endif /* SII943X_USER_DEF_H */

/***** end of file ***********************************************************/

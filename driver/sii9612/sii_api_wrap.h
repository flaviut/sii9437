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
#defines to match Phalanx driver platform API dependencies to that of Salzburg
*/

#ifndef SII9612_DRV_API_WRAP_H
#define SII9612_DRV_API_WRAP_H

#include "sii_sys_obj_api.h"
#include "sii_sys_log_api.h"

/* Sii9612 is using C++ boolean types everywhere. This
is to fix compilation errors if the compiler is not C++ 
*/
#define SiiDrvCraWrite16	sii_drv_cra_write16
#define SiiDrvCraWrite8		sii_drv_cra_write8
#define SiiDrvCraRead8		sii_drv_cra_read8
#define SiiDrvCraBitsSet8	sii_drv_cra_bits_set8
#define SiiDrvCraBitsMod8	sii_drv_cra_bits_mod8
#define SiiDrvCraWrite24	sii_drv_cra_write24
#define SiiDrvCraBlockWrite8	sii_drv_cra_block_write8
#define SiiDrvCraBlockRead8	sii_drv_cra_block_read8
typedef void *SiiInst_t;

typedef enum sii9437_extraction_mode sii9437_extraction_mode;
typedef enum sii_arc_mode sii_arc_mode;

#define SiiSysObjSingletonCreate sii_sys_obj_singleton_create

#endif /* SII9612_DRV_API_WRAP_H */

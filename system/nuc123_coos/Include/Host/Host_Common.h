/******************************************************************************
*
* Copyright 2012-2013, Silicon Image, Inc.  All rights reserved.
* No part of this work may be reproduced, modified, distributed, transmitted,
* transcribed, or translated into any language or computer format, in any form
* or by any means without written permission of: Silicon Image, Inc., 1060
* East Arques Avenue, Sunnyvale, California 94085
*
*****************************************************************************/

#ifndef __M3RT_HOST_COMMON_H__
#define __M3RT_HOST_COMMON_H__

/*-------------------------------------------*/
/*        Local Macro Definition             */
/*-------------------------------------------*/
#define ARRAY_LENGTH(x)          (sizeof(x)/sizeof(x[0]))

#define DELAY_1_SECONDS()        { int i; for( i = 0; i < 4; i++ )  { SYS_SysTickDelay(250000); } }
#define DELAY_2_SECONDS()        { int i; for( i = 0; i < 8; i++ )  { SYS_SysTickDelay(250000); } }
#define DELAY_3_SECONDS()        { int i; for( i = 0; i < 12; i++ ) { SYS_SysTickDelay(250000); } }

#define UINT32_TO_UINT8(x,y)     { y[3] = (uint8_t)((((x) >> 8) >> 8) >> 8); y[2] = (uint8_t)(((x) >> 8) >> 8); y[1] = (uint8_t)((x)>>8); y[0] = (uint8_t)(x&0xFF); }
#define UINT8_TO_UINT32(x)       ((uint32_t)x[0]+(((uint32_t)x[1]<<8)&0x0000FF00)+(((uint32_t)x[2]<<16)&0x00FF0000)+(((uint32_t)x[3]<<24)&0xFF000000))
#define UINT32_TO_UINT8_ID(x,y)  { y[0] = (uint8_t)((((x) >> 8) >> 8) >> 8); y[1] = (uint8_t)(((x) >> 8) >> 8); y[2] = (uint8_t)((x)>>8); y[3] = (uint8_t)(x&0xFF); }

/*-------------------------------------------*/
/*              Enum Definition              */
/*-------------------------------------------*/

/*-------------------------------------------*/
/*        External Variable Definition       */
/*-------------------------------------------*/

/*-------------------------------------------*/
/*        External Function Definition       */
/*-------------------------------------------*/

#endif

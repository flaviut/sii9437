/******************************************************************************
*
* Copyright 2012-2013, Silicon Image, Inc.  All rights reserved.
* No part of this work may be reproduced, modified, distributed, transmitted,
* transcribed, or translated into any language or computer format, in any form
* or by any means without written permission of: Silicon Image, Inc., 1060
* East Arques Avenue, Sunnyvale, California 94085
*
*****************************************************************************/

#ifndef __LIB_TIMER_H__
#define __LIB_TIMER_H__

#include <stdlib.h>
#include <stdint.h>

/*-------------------------------------------*/
/*        Local Macro Definition             */
/*-------------------------------------------*/

/*-------------------------------------------*/
/*            Enum Definition                */
/*-------------------------------------------*/
typedef enum
{
    eTIMER0_COUNT_USE_FOR_I2C = 0,
    eTIMER0_COUNT_USE_FOR_USB,
    eTIMER0_COUNT_USE_FOR_LED,
    eTIMER0_COUNT_USE_FOR_EDID_READ,
    eTIMER0_COUNT_USE_FOR_EDID_READ1,
    eNUMBER_OF_TIMER0_COUNT_USE
}eTIMER0_COUNT_USE;


/*-------------------------------------------*/
/*            Struct Definition              */
/*-------------------------------------------*/


/*-------------------------------------------*/
/*        External Function Definition       */
/*-------------------------------------------*/
/*********************************************
@brief:This Function initialize Timer

@param:[IN]None

@return:[IN]None
*********************************************/
void LibTimerInit( void );

/*********************************************
@brief:This Function Get Timer Tick

@param:[IN]None

@return:[IN]uint32_t
*********************************************/
uint32_t LibTimerGetTick( void );

/*********************************************
@brief:This Function Calculate Timer Tick Interval

@param:[IN]uint32_t u32StartTick
       [IN]uint32_t u32EndTick

@return:[IN]uint32_t
*********************************************/
uint32_t LibTimerCalTickInterval( uint32_t u32StartTick, uint32_t u32EndTick );

/*********************************************
@brief:This Function Set Timer

@param:[IN]eTIMER0_COUNT_USE eTimer0CountUse
       [IN]uint32_t u32msec

@return:[IN]None
*********************************************/
void LibTimerSetTimer( eTIMER0_COUNT_USE eTimer0CountUse, uint32_t u32msec );

/*********************************************
@brief:This Function Check if Timer is expired or not

@param:[IN]eTIMER0_COUNT_USE eTimer0CountUse

@return:[IN]TRUE - Time is expired
*********************************************/
int LibTimerIfTimerExpired( eTIMER0_COUNT_USE eTimer0CountUse );

#endif   /* __LIB_TIMER_H__ */


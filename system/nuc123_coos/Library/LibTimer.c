/******************************************************************************
*
* Copyright 2012-2013, Silicon Image, Inc.  All rights reserved.
* No part of this work may be reproduced, modified, distributed, transmitted,
* transcribed, or translated into any language or computer format, in any form
* or by any means without written permission of: Silicon Image, Inc., 1060
* East Arques Avenue, Sunnyvale, California 94085
*
*****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "NUC123Series.h"
#include "DrvTimer.h"
#include "LibTimer.h"
#include "CLOCK.h"

/*-------------------------------------------*/
/*        Local Macro Definition             */
/*-------------------------------------------*/

/*-------------------------------------------*/
/*              Enum Definition              */
/*-------------------------------------------*/

/*-------------------------------------------*/
/*            Struct Definition              */
/*-------------------------------------------*/

/*-------------------------------------------*/
/*        Internal Variable Definition       */
/*-------------------------------------------*/
static uint32_t u32TimerCounters[eNUMBER_OF_TIMER0_COUNT_USE];
static uint32_t u32TimerTick;

/*-------------------------------------------*/
/*        External Variable Definition       */
/*-------------------------------------------*/

/*-------------------------------------------*/
/*        Internal Function Definition       */
/*-------------------------------------------*/

/*-------------------------------------------*/
/*        Interrupt Function Definition      */
/*-------------------------------------------*/
/*********************************************
@brief:This Function TIMER0 Interrupt Function - 1ms Interval

@param:[IN]None

@return:[IN]None
*********************************************/
void TMR0_IRQHandler(void)
{
    uint8_t i;

    /* Clear TIMER0 Timeout Interrupt Flag */
    TIM_CLEAR_INTERRUPT_FLAG(TIMER0);

    /* Increment the Timer Tick */
    u32TimerTick++; 

    /* Decrement all active timers */
    for( i = 0; i < eNUMBER_OF_TIMER0_COUNT_USE; i++ )
    {
        if ( u32TimerCounters[i] > 0 )
        {
            u32TimerCounters[i]--;
        }
    }

    return;
}
/*********************************************
@brief:This Function TIMER1 Interrupt Function - 1ms Interval

@param:[IN]None

@return:[IN]None
*********************************************/
void TMR1_IRQHandler(void)
{
    /* Clear TIMER0 Timeout Interrupt Flag */
    TIM_CLEAR_INTERRUPT_FLAG(TIMER1);

    return;
}

/*-------------------------------------------*/
/*             External Function             */
/*-------------------------------------------*/
/*********************************************
@brief:This Function initialize Timer

@param:[IN]None

@return:[IN]None
*********************************************/
void LibTimerInit( void )
{
    uint8_t i;

    /* Initializer timer counters */
    for ( i = 0; i < eNUMBER_OF_TIMER0_COUNT_USE; i++ )
    {
        u32TimerCounters[i] = 0;
    }

    /* Timer0 Initialize - Timer Interval 1ms */
    SET_TMR0_CLKSRC(CLK_TMR0_HXT); 
    DrvTIMER_Init(TIMER0, 0, (__XTAL/1000), TIM_MODE_PERIODIC);
    DrvTIMER_EnableInt(TIMER0, TIM_INT_EN);
    TIM_ENABLE(TIMER0);


    /* Timer1 Initialize - Timer Interval 80ms */
    SET_TMR1_CLKSRC(CLK_TMR1_HXT);
    DrvTIMER_Init(TIMER1, 0, (__XTAL/15), TIM_MODE_PERIODIC);
    DrvTIMER_EnableInt(TIMER1, TIM_INT_EN);
    TIM_ENABLE(TIMER1);

    return;
}

/*********************************************
@brief:This Function Get Timer Tick

@param:[IN]None

@return:[IN]uint32_t
*********************************************/
uint32_t LibTimerGetTick( void )
{
    uint32_t u32TimerTickVal;

    /* Disable Timer0 Interrupt */
    DrvTIMER_DisableInt(TIMER0);

    /* Copy Timer Tick to Local Parameter */
    u32TimerTickVal = u32TimerTick;

    /* Ensable Timer0 Interrupt */
    DrvTIMER_EnableInt(TIMER0, TIM_INT_EN);

    return u32TimerTickVal;
}

/*********************************************
@brief:This Function Calculate Timer Tick Interval

@param:[IN]uint32_t u32StartTick
       [IN]uint32_t u32EndTick

@return:[IN]uint32_t
*********************************************/
uint32_t LibTimerCalTickInterval( uint32_t u32StartTick, uint32_t u32EndTick )
{
    uint32_t u32TimerTickInterval = 0;

    /* Check ( Start Tick - End Tick ) Number */
    if( u32EndTick >= u32StartTick )
    {
        u32TimerTickInterval = ( u32EndTick - u32StartTick ); 
    }
    else
    {
        u32TimerTickInterval = ( u32EndTick + 0xFFFFFFFF - u32StartTick ); 
    }

    return u32TimerTickInterval;
}

/*********************************************
@brief:This Function Set Timer

@param:[IN]eTIMER0_COUNT_USE eTimer0CountUse
       [IN]uint32_t u32msec

@return:[IN]None
*********************************************/
void LibTimerSetTimer( eTIMER0_COUNT_USE eTimer0CountUse, uint32_t u32msec )
{
    /* Disable Timer0 Interrupt */
    DrvTIMER_DisableInt(TIMER0);

    switch( eTimer0CountUse )
    {
        case eTIMER0_COUNT_USE_FOR_I2C:
        case eTIMER0_COUNT_USE_FOR_USB:
        case eTIMER0_COUNT_USE_FOR_LED:
        case eTIMER0_COUNT_USE_FOR_EDID_READ:
        case eTIMER0_COUNT_USE_FOR_EDID_READ1:
        {
            u32TimerCounters[eTimer0CountUse] = u32msec;
        }
        break;
        case eNUMBER_OF_TIMER0_COUNT_USE:
		break;
    }

    /* Ensable Timer0 Interrupt */
    DrvTIMER_EnableInt(TIMER0, TIM_INT_EN);

    return;
}

/*********************************************
@brief:This Function Check if Timer is expired or not

@param:[IN]eTIMER0_COUNT_USE eTimer0CountUse

@return:[IN]TRUE - Time is expired
*********************************************/
int LibTimerIfTimerExpired( eTIMER0_COUNT_USE eTimer0CountUse )
{
    if( eTimer0CountUse < eNUMBER_OF_TIMER0_COUNT_USE )
    {
        if( u32TimerCounters[eTimer0CountUse] == 0)
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }
}

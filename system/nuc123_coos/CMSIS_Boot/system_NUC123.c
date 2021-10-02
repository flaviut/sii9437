/**************************************************************************//**
 * @file     system_NUC123.c
 * @version  V3.0
 * $Revision: 4 $
 * $Date: 14/07/29 4:50p $
 * @brief    NUC123 Series CMSIS System File
 *
 * @note
 * Copyright (C) 2014 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdint.h>
#include "NUC123Series.h"
#include "system_NUC123.h"
#include "CLOCK.h"
/*----------------------------------------------------------------------------
  Clock Variable definitions
 *----------------------------------------------------------------------------*/
            /*!< System Clock Frequency (Core Clock) */



/*----------------------------------------------------------------------------
  Clock functions
  This function is used to update the variable SystemCoreClock
  and must be called whenever the core clock is changed.
 *----------------------------------------------------------------------------*/
/*recheck akshay*/

#if 0
void SystemCoreClockUpdate(void)             /* Get Core Clock Frequency      */
{
    uint32_t u32Freq, u32ClkSrc;
    uint32_t u32HclkDiv;

    /* Update PLL Clock */
    PllClock = CLK_GetPLLClockFreq();

    u32ClkSrc = CLK->CLKSEL0 & CLK_CLKSEL0_HCLK_S_Msk;

    if(u32ClkSrc == CLK_CLKSEL0_HCLK_S_PLL)
    {
        /* Use PLL clock */
        u32Freq = PllClock;
    }
    else if(u32ClkSrc == CLK_CLKSEL0_HCLK_S_PLL_DIV2)
    {
        /* Use PLL/2 clock */
        u32Freq = PllClock >> 1;
    }
    else
    {
        /* Use the clock sources directly */
        u32Freq = gau32ClkSrcTbl[u32ClkSrc];
    }

    u32HclkDiv = (CLK->CLKDIV & CLK_CLKDIV_HCLK_N_Msk) + 1;

    /* Update System Core Clock */
    SystemCoreClock = u32Freq / u32HclkDiv;

    CyclesPerUs = (SystemCoreClock + 500000) / 1000000;
}
#endif
/*---------------------------------------------------------------------------------------------------------*/
/* Function: SystemInit                                                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*      None                                                                                               */
/*                                                                                                         */
/* Returns:                                                                                                */
/*      None                                                                                               */
/*                                                                                                         */
/* Description:                                                                                            */
/*      The necessary initialization of system.                                                           */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#if 0
void SystemInit(void)
{

}
#endif

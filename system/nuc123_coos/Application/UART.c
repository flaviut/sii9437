
/**************************************************************************//**
 * @file     UARTApp.c
 * @version  V1.00
 * $Revision: 1 $
 * $Date: 12/08/20 11:27a $ 
 * @brief    Main function which exposes UART transmit and recieve .
 *
 * @note
 * Copyright (C) 2011 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include "NUC123Series.h"
#include <UART.h>
#include <Clock.h>
#include <GPIO.h>
#include "I2C.h"
#include "Vcom_Data_Proc.h"
#include "I2C_MASTER.h"
#include "CLOCK.h"
#include "SYS.h"
#include "GPIO.h"
#include "FMC.h"
#include "Host_Core.h"
#include "LibTimer.h"
#include "com_interface.h"
volatile uint8_t comRbuf[RXBUFSIZE];
volatile uint16_t RX_Index = 0;
volatile uint16_t TX_Index 	= 0;

/**
 * @brief       System init
 *
 * @param       None
 *
 * @return      None
 *
 * @details     Init system clock
 */
void UART_SYS_Init(void)
{
	SYS_UnlockReg();		//En:Unlock protected registers
	GPIO_XT1();
	CLK_HXT_ENABLE();		//En:Enable External XTAL (4~24 MHz)
	while(!CLK_IS_STABLE(CLK_HXT_STB));//En:Waiting for 12MHz clock ready
	SET_HCLK_CLKSRC(CLK_HCLK_HXT);	//En:Switch HCLK clock source to XTAL
	SystemCoreClockUpdate();//En:Update the variable SystemCoreClock
	SYS_LockReg();		//En:Lock protected registers
}

/**
 * @brief       UART1 init
 *
 * @param       None
 *
 * @return      None
 *
 * @details     Set UART1 8n1, 115200bps
 */
void UART1_Init(void)
{   
	STR_UART_T sParam;
	//En: Set UART0 all pins to PB0=RX,PB1=TX,PB2=RTS,PB3=CTS
	GPIO_UART1();
	
	/* UART Setting */
	sParam.u32ClkSrc	= CLK_UART_HIRC;
	sParam.u32ClkDiv	= 1;
	//HXT/1 == __XTAL
	sParam.u32ClkFreq	= __IRC22M;
	sParam.u32BaudRate 	= 921600;
	sParam.u32cDataBits = UART_WORD_LEN_8;
	sParam.u32cStopBits = UART_STOP_BIT_1;
	sParam.u32cParity 	= UART_PARITY_NONE;
	sParam.u32cRxTriggerLevel	= UART_FCR_RFITL_14BYTES;
	sParam.u32BaudMode    = UART_BAUD_MODE2;
	sParam.u8RxTimeOut	= 200;
	
	/* Set UART Configuration */
	DrvUART_Init(UART1,&sParam);
	/* Enable UART Interrupt */
	//NVIC_SetPriority (UART1_IRQn, (1<<__NVIC_PRIO_BITS) - 2);
	DrvUART_EnableInt(UART1,
		UART_IER_RDA_IEN|UART_IER_RTO_IEN|UART_IER_TIMEOUT_COUNTER_EN);
}

/**
 * @brief       UART1 ISR routine 
 *
 * @param       None
 *
 * @return      None
 *
 * @details     Recieve data in ISR routine
 */
void UART1_IRQHandler(void)
{
#ifdef SII_BUILD_COOS
	CoEnterISR();
#endif
	 //en:Check if receive interrupt
	if(UART_GET_RDA_INT(UART1)|UART_GET_TOUT_INT(UART1))
	{
		//en:Check if received data available
		while(UART_GET_RX_EMPTY(UART1)==0)
		{
			if(UART_GET_RLS_FLAG(UART1))
			{
				UART_CLEAR_RLS_FLAG(UART1);
				UART_READ(UART1); //discard the byte
			}
			else
			{
				//en:Read the char
				comRbuf[RX_Index]=UART_READ(UART1);
			}
			RX_Index++;
			//en:Check if buffer full
			if(RX_Index >= RXBUFSIZE)
			{
				 RX_Index=0;
			}
		}
	}
	//NVIC_DisableIRQ(UART1_IRQn);
	// Read the command until newline character occurs,
	//No need to process till newline.
	if(comRbuf[RX_Index-1] == '\r')
	{
		comRbuf[RX_Index-1] = 0;
		/*process_comport_data((uint8_t *) comRbuf,
				(uint16_t *) &RX_Index);*/
		RX_Index = 0;
		memset((uint8_t *) comRbuf, 0, RXBUFSIZE);
	}
#ifdef SII_BUILD_COOS
	CoExitISR();
#endif
	return;
}

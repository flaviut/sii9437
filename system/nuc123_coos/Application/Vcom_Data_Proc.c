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
#include <ctype.h>
#include "GPIO.h"
#include "SYS.h"
//#include "DrvUART.h"
#include "DrvUSB.h"
#include "VCOMSys.h"
#include "RomData.h"
//#include "LibRingBuf.h"
//#include "Host_Core.h"
#include "LibTimer.h"
#include "Vcom_Data_Proc.h"

/*-------------------------------------------*/
/*        Local Macro Definition             */
/*-------------------------------------------*/
#define MESSAGE_UNIT_LEN   (64)

/*-------------------------------------------*/
/*              Macro Function               */
/*-------------------------------------------*/
#define SET_DATA_IN_BEGIN()    ( eVCOMDataInState = eVCOM_DATA_IN_BEGIN )
#define SET_DATA_IN_END()      ( eVCOMDataInState = eVCOM_DATA_IN_END   )
#define SET_DATA_IN_DONE()     ( eVCOMDataInState = eVCOM_DATA_IN_DONE  )
#define IF_DATA_IN_BEGIN()     ( (eVCOMDataInState == eVCOM_DATA_IN_BEGIN) ? (TRUE) : (FALSE) )
#define IF_DATA_IN_END()       ( (eVCOMDataInState == eVCOM_DATA_IN_END)   ? (TRUE) : (FALSE) )

/*-------------------------------------------*/
/*              Struct Definition            */
/*-------------------------------------------*/

/*-------------------------------------------*/
/*              Enum Definition              */
/*-------------------------------------------*/
typedef enum
{
    eVCOM_DATA_IN_DONE = 1,
    eVCOM_DATA_IN_BEGIN,
    eVCOM_DATA_IN_END,
    eVOM_NUM_OF_DATA_IN_STATE
}eVCOM_DATA_IN_STATE;

/*-------------------------------------------*/
/*        Internal Variable Definition       */
/*-------------------------------------------*/
static void (*pVcomSendOutCallback)( uint8_t *pu8SendOutBuf, uint32_t u32SendOutLen );

static eVCOM_DATA_IN_STATE eVCOMDataInState = eVCOM_DATA_IN_DONE;
static uint8_t firmware_comm_mode = FALSE;

/*-------------------------------------------*/
/*        External Variable Definition       */
/*-------------------------------------------*/

/*-------------------------------------------*/
/*        Internal Function Definition       */
/*-------------------------------------------*/
static void s_Vcom_GetInData(uint8_t *pu8EpBuf, uint32_t u32Size);
static void s_Vcom_SetOutData(uint8_t *pu8EpBuf, uint32_t u32Size);
static void s_Vcom_DataIn_Func(uint8_t *pu8DataInMsg, uint32_t u32DataInLen);

/*-------------------------------------------*/
/*        External Function Definition       */
/*-------------------------------------------*/

/*-------------------------------------------*/
/*             External Function             */
/*-------------------------------------------*/
/*********************************************
@brief:This function Configs the Virtual Com Function

@param:[In]pu8Message : Return Message

@return:None
*********************************************/
void Vcom_Config_Proc(void)
{
	uint8_t uTimes;
	    E_DRVUSB_STATE eUsbState;

	    /* Open USB Function */
	    if( DrvUSB_Open((void*)DrvUSB_DispatchEvent) != E_SUCCESS )
	    {
	        return;
	    }

	    /* Disable USB-related interrupts. */
	    _DRVUSB_ENABLE_MISC_INT(0);
	    /* Enable float-detection interrupt. */
	    _DRVUSB_ENABLE_FLDET_INT();

	    /* Wait for USB connected. */
	    for( uTimes = 0; uTimes < 20; uTimes++ )
	    {
	        /* Order here is significant.
	         Give a chance to handle remaining events before exiting this loop. */
	        eUsbState = DrvUSB_GetUsbState();
	        if( ( eUsbState >= eDRVUSB_ATTACHED ) &&
	            ( eUsbState != eDRVUSB_SUSPENDED ) )
	        {
	           break;
	        }
	        SYS_SysTickDelay(100000); /* Delay 100ms */
	    }

	     /* Start VCOM and install the callback functions to handle in/out data */
	     VCOM_Open((void *)s_Vcom_GetInData, (void *)s_Vcom_SetOutData);
	    /* Enable USB-related interrupts */
	    _DRVUSB_ENABLE_MISC_INT(INTEN_WAKEUP | INTEN_WAKEUPEN | INTEN_FLDET | INTEN_USB | INTEN_BUS);

	    /* Wait for VCOM connected.*/
	    for( uTimes = 0; uTimes < 60; uTimes++ )
	    {
	        /* USB EVENT handle */
	        /* When usb connect, PC will enumerate it, after PC install driver, the USB state will be "eDRVUSB_CONFIGURED" */
	        eUsbState = DrvUSB_GetUsbState();
	        if( eUsbState == eDRVUSB_CONFIGURED )
	        {
	            break;
	        }
	        SYS_SysTickDelay(10000); /* Delay 10ms */
	    }

	    return;
}
#if defined(BILL_CLASS)
void usb_connect(void)
{
	uint8_t uTimes;
	    E_DRVUSB_STATE eUsbState;

	    /* Open USB Function */
	    if( DrvUSB_Open((void*)DrvUSB_DispatchEvent) != E_SUCCESS )
	    {
	        return;
	    }

	    /* Disable USB-related interrupts. */
	    _DRVUSB_ENABLE_MISC_INT(0);
	    /* Enable float-detection interrupt. */
	    _DRVUSB_ENABLE_FLDET_INT();

	    /* Wait for USB connected. */
	    for( uTimes = 0; uTimes < 20; uTimes++ )
	    {
	        /* Order here is significant.
	         Give a chance to handle remaining events before exiting this loop. */
	        eUsbState = DrvUSB_GetUsbState();
	        if( ( eUsbState >= eDRVUSB_ATTACHED ) &&
	            ( eUsbState != eDRVUSB_SUSPENDED ) )
	        {
	           break;
	        }
	        usleep(100 * 1000); /* Delay 100ms */
	    }

	     /* Start VCOM and install the callback functions to handle in/out data */
	     VCOM_Open((void *)s_Vcom_GetInData, (void *)s_Vcom_SetOutData);
	    /* Enable USB-related interrupts */
	    _DRVUSB_ENABLE_MISC_INT(INTEN_WAKEUP | INTEN_WAKEUPEN | INTEN_FLDET | INTEN_USB | INTEN_BUS);

	    /* Wait for VCOM connected.*/
	    for( uTimes = 0; uTimes < 60; uTimes++ )
	    {
	        /* USB EVENT handle */
	        /* When usb connect, PC will enumerate it, after PC install driver, the USB state will be "eDRVUSB_CONFIGURED" */
	        eUsbState = DrvUSB_GetUsbState();
	        if( eUsbState == eDRVUSB_CONFIGURED )
	        {
	            break;
	        }
	        usleep(10 * 1000); /* Delay 10ms */
	    }

	    return;
}

void usb_disconnect(void)
{
	DrvUSB_Close();
	return;
}
#endif





/*********************************************
@brief:This function Return Message to PC

@param:[In]pu8Message : Return Message

@return:None
*********************************************/
void Vcom_Data_Return_Proc(uint8_t *pu8Message, uint32_t u32MessageLen)
{
    uint8_t u8Count = (u32MessageLen/MESSAGE_UNIT_LEN);
    uint8_t u8Rest = (u32MessageLen%MESSAGE_UNIT_LEN);
    int i;

    /* Need to trigger for the First return message */
    //s_Vcom_DataIn_Func( pu8Message, 0 );

    for( i = 0; i < u8Count; i++ )
    {
        s_Vcom_DataIn_Func( pu8Message, MESSAGE_UNIT_LEN );
        pu8Message += MESSAGE_UNIT_LEN;
    }

    if( u8Rest != 0 )
    {
        s_Vcom_DataIn_Func( pu8Message, u8Rest );
    }

    /* Need to trigger for the Last return message */
    //s_Vcom_DataIn_Func( pu8Message, 0 );

    return;
}

/*********************************************
@brief:This function Install the CallBack Function for Data SendOut( PC -> Nuvoton )

@param:[In]pu8Message : Return Message

@return:None
*********************************************/
void Vcom_Set_DataSendOutCallback( void (*pFunc)( uint8_t *pu8SendOutBuf, uint32_t u32SendOutLen ) )
{
    pVcomSendOutCallback = pFunc;

    return;
}

/*-------------------------------------------*/
/*             Internal Function             */
/*-------------------------------------------*/
/* This callback will be executed whenever the interrupt IN transaction is received from HOST(PC) to get
   report. */
/*********************************************
@brief:This function exexutes the DataGetIn( Nuvoton -> PC) Process

@param:[In]None

@return:None
*********************************************/
static void s_Vcom_GetInData(uint8_t *pu8EpBuf, uint32_t u32Size)
{
    if( IF_DATA_IN_BEGIN() != FALSE )
    {
         SET_DATA_IN_END();
    }

    return;
}

/* This callback will be executed whenever the interrupt OUT transaction is received from HOST(PC) to set
   report. */
/*********************************************
@brief:This function exexutes the DataSendOut ( PC -> Nuvoton ) Process

@param:[In]None

@return:None
*********************************************/
static void s_Vcom_SetOutData(uint8_t *pu8EpBuf, uint32_t u32Size)
{
    if( pVcomSendOutCallback != NULL )
    {
        pVcomSendOutCallback( pu8EpBuf, u32Size );
    }

    return;
}

/*********************************************
@brief:This function executes Data In Function for VCOM

@param:[In]pu8Message : Return Message

@return:None
*********************************************/
static void s_Vcom_DataIn_Func(uint8_t *pu8DataInMsg, uint32_t u32DataInLen)
{
    uint32_t u32EpId;

    /* Set Data In Begin Flag */
    SET_DATA_IN_BEGIN();

   /* Set Timer0 Count for USB - 1ms */
    LibTimerSetTimer( eTIMER0_COUNT_USE_FOR_USB, 3 );

    /* USB Data In Process */
    DrvUSB_DataIn( BULK_IN_EP_NUM, pu8DataInMsg, u32DataInLen );

    /* Wait for USB Data In Process End Flag Set */
    while( IF_DATA_IN_END() == FALSE )
    {
        /* Check if USB is still Attached */
        if( DrvUSB_GetUsbState() != eDRVUSB_CONFIGURED )
        {
            break;
        }
        /* Check if USB Timer Count is expired */
        if( LibTimerIfTimerExpired( eTIMER0_COUNT_USE_FOR_USB ) != FALSE)
        {
            /* USB Timer Count expired - This is happen when USB is attached but SWAM3 is NOT opened */
            u32EpId = DrvUSB_GetEpIdentity(BULK_IN_EP_NUM, EP_INPUT);
            _DRVUSB_CLEAR_EP_READY(u32EpId);
            break;
        }
    }

    /* Set Data In Done Flag */
    SET_DATA_IN_DONE();

    return;
}

void VCOM_IntInAckCallback(void* pVoid)
{
    /* Not used */
}

uint32_t Vcom_Firmware_comm_mode_query(uint8_t *is_fw_comm_mode)
{
	*is_fw_comm_mode = firmware_comm_mode;

	return 0;
}

uint32_t Vcom_Firmware_comm_mode_set(uint8_t *is_fw_comm_mode)
{
	firmware_comm_mode = *is_fw_comm_mode;

	return 0;
}


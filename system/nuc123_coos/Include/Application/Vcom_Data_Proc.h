/******************************************************************************
*
* Copyright 2012-2013, Silicon Image, Inc.  All rights reserved.
* No part of this work may be reproduced, modified, distributed, transmitted,
* transcribed, or translated into any language or computer format, in any form
* or by any means without written permission of: Silicon Image, Inc., 1060
* East Arques Avenue, Sunnyvale, California 94085
*
*****************************************************************************/

#ifndef __VCOM_DATA_PROC_H__
#define __VCOM_DATA_PROC_H__

#include "NUC123Series.h"

/*-------------------------------------------*/
/*        Local Macro Definition             */
/*-------------------------------------------*/

/*-------------------------------------------*/
/*              Enum Definition              */
/*-------------------------------------------*/

/*-------------------------------------------*/
/*        External Variable Definition       */
/*-------------------------------------------*/

/*-------------------------------------------*/
/*        External Function Definition       */
/*-------------------------------------------*/
/*********************************************
@brief:This function Configs the Virtual Com Function

@param:[In]pu8Message : Return Message

@return:None
*********************************************/
void Vcom_Config_Proc(void);

void usb_connect(void);
void usb_disconnect(void);

/*********************************************
@brief:This function Return Message to PC

@param:[In]pu8Message : Return Message

@return:None
*********************************************/
void Vcom_Data_Return_Proc(uint8_t *pu8Message, uint32_t u32MessageLen);

/*********************************************
@brief:This function Install the CallBack Function for Data SendOut( PC -> Nuvoton )

@param:[In]pu8Message : Return Message

@return:None
*********************************************/
void Vcom_Set_DataSendOutCallback( void (*pFunc)( uint8_t *pu8SendOutBuf, uint32_t u32SendOutLen ) );

uint32_t Vcom_Firmware_comm_mode_query(uint8_t *is_fw_comm_mode);
uint32_t Vcom_Firmware_comm_mode_set(uint8_t *is_fw_comm_mode);

#endif /*_VCOM_DATA_PROC_H_*/

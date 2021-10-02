/******************************************************************************
*
* Copyright 2012-2013, Silicon Image, Inc.  All rights reserved.
* No part of this work may be reproduced, modified, distributed, transmitted,
* transcribed, or translated into any language or computer format, in any form
* or by any means without written permission of: Silicon Image, Inc., 1060
* East Arques Avenue, Sunnyvale, California 94085
*
*****************************************************************************/

#ifndef __HOST_CORE_H__
#define __HOST_CORE_H__

#include "NUC123Series.h"
#include "Host_Build_Info.h"

/*-------------------------------------------*/
/*        Local Macro Definition             */
/*-------------------------------------------*/

/*-------------------------------------------*/
/*              Enum Definition              */
/*-------------------------------------------*/
typedef enum
{
    ePIN_LOW = 0,
    ePIN_HIGH = 1,
    eNUMBER_OF_PIN_STATE
}ePIN_STATE;

typedef enum
{
    eDUT_BOARD_TYPE_UNKNOWN = 0,
    eDUT_BOARD_TYPE_ARYAX02,
    eDUT_BOARD_TYPE_TYRION,
    eDUT_BOARD_TYPE_MARBLE,
    eDUT_BOARD_TYPE_CERSEI,
    eDUT_BOARD_TYPE_TARA,
    eDUT_BOARD_TYPE_TARA4k_TX,
    eDUT_BOARD_TYPE_TARA4k_RX_IO,
    eDUT_BOARD_TYPE_NUM_OF_BOARD_TYPE
}eDUT_BOARD_TYPE;

typedef enum
{
    eUART_STATUS_TYPE_UNKNOWN = 0,
    eUART_STATUS_TYPE_AVAILABLE,
    eUART_STATUS_TYPE_NOT_AVAILABLE,
    eUART_STATUS_TYPE_NUM_OF_TYPE
}eUART_STATUS_TYPE;

/*-------------------------------------------*/
/*        External Variable Definition       */
/*-------------------------------------------*/
extern volatile uint8_t u8I2cProcCount;
extern eDUT_BOARD_TYPE eDUTBoardType;
extern eUART_STATUS_TYPE eUartStatusType;

/*-------------------------------------------*/
/*        External Function Definition       */
/*-------------------------------------------*/
void Host_Core_Proc(void);
void sii_vcom_command_process(void);

#endif


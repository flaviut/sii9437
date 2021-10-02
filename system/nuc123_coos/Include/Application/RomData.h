/******************************************************************************
*
* Copyright 2012-2013, Silicon Image, Inc.  All rights reserved.
* No part of this work may be reproduced, modified, distributed, transmitted,
* transcribed, or translated into any language or computer format, in any form
* or by any means without written permission of: Silicon Image, Inc., 1060
* East Arques Avenue, Sunnyvale, California 94085
*
*****************************************************************************/

#ifndef __ROM_DATA_H__
#define __ROM_DATA_H__

#include "NUC123Series.h"

/*-------------------------------------------*/
/*        Local Macro Definition             */
/*-------------------------------------------*/
#define CTRL_EP_NUM         0
#define INT_IN_EP_NUM       3
#define BULK_OUT_EP_NUM     2
#define BULK_IN_EP_NUM      1

/*-------------------------------------------*/
/*              Enum Definition              */
/*-------------------------------------------*/

/*-------------------------------------------*/
/*        External Variable Definition       */
/*-------------------------------------------*/
extern const uint8_t gau8DeviceDescriptor[];
extern const uint8_t gau8ConfigDescriptor[];
extern const uint8_t gau8VendorStringDescriptor[];
extern const uint8_t gau8ProductStringDescriptor[];
extern const uint8_t gau8StringSerial[];
extern const uint8_t gau8StringConf[];
extern const uint8_t gau8StringIntf[];
extern const uint8_t gau8StringURL[];
extern const uint8_t gau8StringAlt1[];
extern const uint8_t gau8StringAlt2[];
extern const uint8_t gau8BosHdrDescriptor[];
extern const uint8_t gau8StringLang[];
extern const uint8_t gau8StringAudio[];

#endif /*__ROM_DATA_H__*/


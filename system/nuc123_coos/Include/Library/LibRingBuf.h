/******************************************************************************
*
* Copyright 2012-2013, Silicon Image, Inc.  All rights reserved.
* No part of this work may be reproduced, modified, distributed, transmitted,
* transcribed, or translated into any language or computer format, in any form
* or by any means without written permission of: Silicon Image, Inc., 1060
* East Arques Avenue, Sunnyvale, California 94085
*
*****************************************************************************/

#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

#include <stdlib.h>
#include <stdint.h>
#include "Host_Build_Info.h"

/*-------------------------------------------*/
/*            Struct Definition              */
/*-------------------------------------------*/
typedef struct sRingBufInfo{
    int      bufSize;    /* Size of Buffer */
    uint8_t *pBuf;       /* Pointer to Buffer */
    int      pWr;        /* Writing Index */
    int      pRd;        /* Reading Index */
    int      count;      /* Data Count in Ring Buffer */
}RINGBUF_INFO;

/*-------------------------------------------*/
/*        External Function Definition       */
/*-------------------------------------------*/
/*********************************************
@brief:This function executes Ring Buffer Initialization

@param:[In]u32IntStatus: Interrupt Type

@return:None
*********************************************/
void LibRingBufInit( RINGBUF_INFO *pInfo, uint8_t *pBuf, int bufSize );

/*********************************************
@brief:This function executes Ring Buffer Flush

@param:[In]u32IntStatus: Interrupt Type

@return:None
*********************************************/
void LibRingBufFlush( RINGBUF_INFO *pInfo );

/*********************************************
@brief:This function Set One Byte Data to Ring Buffer

@param:[In]u32IntStatus: Interrupt Type

@return:None
*********************************************/
int LibRingBufSetOneByteData( RINGBUF_INFO* pInfo, uint8_t Data );

/*********************************************
@brief:This function Get One Byte Data from Ring Buffer

@param:[In]u32IntStatus: Interrupt Type

@return:None
*********************************************/
int LibRingBufGetOneByteData( RINGBUF_INFO*  pInfo, uint8_t* pData );

/*********************************************
@brief:This function Get Specified Bytes of Data from Ring Buffer

@param:[In]u32IntStatus: Interrupt Type

@return:None
*********************************************/
int LibRingBufGetBlkData( RINGBUF_INFO *pInfo, uint8_t *pData, int len );

/*********************************************
@brief:This function Get the Data Number of Ring Buffer

@param:[In]u32IntStatus: Interrupt Type

@return:None
*********************************************/
int LibRingBufGetDataCnt( const RINGBUF_INFO* pInfo );

#endif


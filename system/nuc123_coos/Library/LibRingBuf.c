/******************************************************************************
*
* Copyright 2012-2013, Silicon Image, Inc.  All rights reserved.
* No part of this work may be reproduced, modified, distributed, transmitted,
* transcribed, or translated into any language or computer format, in any form
* or by any means without written permission of: Silicon Image, Inc., 1060
* East Arques Avenue, Sunnyvale, California 94085
*
*****************************************************************************/

#include "LibRingBuf.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "NUC123Series.h"

/*********************************************
@brief:This function executes Ring Buffer Initialization

@param:[In]u32IntStatus: Interrupt Type

@return:None
*********************************************/
void LibRingBufInit( RINGBUF_INFO *pInfo, uint8_t *pBuf, int bufSize )
{
    pInfo->pBuf     = pBuf;
    pInfo->pWr      = 0;
    pInfo->pRd      = 0;
    pInfo->bufSize  = bufSize;
    pInfo->count    = 0;

    return;
}

/*********************************************
@brief:This function executes Ring Buffer Flush

@param:[In]u32IntStatus: Interrupt Type

@return:None
*********************************************/
void LibRingBufFlush( RINGBUF_INFO *pInfo )
{
    pInfo->pWr   = 0;
    pInfo->pRd   = 0;
    pInfo->count = 0;

    return;
}

/*********************************************
@brief:This function Set One Byte Data to Ring Buffer

@param:[In]u32IntStatus: Interrupt Type

@return:None
*********************************************/
int LibRingBufSetOneByteData( RINGBUF_INFO* pInfo, uint8_t Data )
{
    int work;

    if( pInfo->count >= pInfo->bufSize )
    {
        /* Data has already full */
        return FALSE;
    }

    work = pInfo->pWr + 1;

    if( work >= pInfo->bufSize )
    {
        work = 0;
    }

    pInfo->pBuf[pInfo->pWr] = Data;
    pInfo->count++;
    pInfo->pWr = work;

    return TRUE;
}

/*********************************************
@brief:This function Get One Byte Data from Ring Buffer

@param:[In]u32IntStatus: Interrupt Type

@return:None
*********************************************/
int LibRingBufGetOneByteData( RINGBUF_INFO* pInfo, uint8_t* pData )
{
    int work;

    if(pInfo->count == 0)
    {
        /* Data is not exist */
        return FALSE;
    }

    *pData = pInfo->pBuf[pInfo->pRd];

    work = pInfo->pRd + 1;

    pInfo->count--;

    if(work >= pInfo->bufSize)
    {
        work = 0;
    }

    pInfo->pRd = work;

    return TRUE;
}

/*********************************************
@brief:This function Get Specified Bytes of Data from Ring Buffer

@param:[In]u32IntStatus: Interrupt Type

@return:None
*********************************************/
int LibRingBufGetBlkData( RINGBUF_INFO *pInfo, uint8_t *pData, int len )
{
    int size;
    int rdsize;
    int nextRd;

    if( pInfo->count < len)
    {
        return FALSE;
    }

    size = len;

    while(size)
    {
        if( (pInfo->pRd + size) >= pInfo->bufSize )
        {
            rdsize = (pInfo->bufSize - pInfo->pRd); 
            nextRd = 0;
        }
        else
        {
            rdsize = size;
            nextRd = pInfo->pRd + size;
        }

        memcpy(pData, &(pInfo->pBuf[pInfo->pRd]), rdsize);

        size -= rdsize;
        pInfo->count -= rdsize;
        pInfo->pRd = nextRd;
        pData += rdsize;
    }

    return TRUE;
}

/*********************************************
@brief:This function Get the Data Number of Ring Buffer

@param:[In]u32IntStatus: Interrupt Type

@return:None
*********************************************/
int LibRingBufGetDataCnt( const RINGBUF_INFO* pInfo )
{
    return ( pInfo->count );
}

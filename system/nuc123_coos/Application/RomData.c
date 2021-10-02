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
#include "SYS.h"
#include "DrvUSB.h"
#include "VCOMSys.h"
#include "RomData.h"
#include "Host_Build_Info.h"

/*-------------------------------------------*/
/*        Local Macro Definition             */
/*-------------------------------------------*/
#if defined(BILL_CLASS)
#define USB_VID         (0x1416)/*(0x1A4A)*/  /* Vendor ID */
#ifdef CHISEL_HOST_VER_2
#define USB_PID         (0x5111)/*(0x6410)*/  /* Product ID */
#endif
#else
#define USB_VID         (0x0416)/*(0x1A4A)*/  /* Vendor ID */
#ifdef CHISEL_HOST_VER_2
#define USB_PID         (0x5011)/*(0x6410)*/  /* Product ID */
#endif
#endif

#define USB_VID_LO      (USB_VID & 0xFF)
#define USB_VID_HI      ((USB_VID >> 8) & 0xFF)

#define USB_PID_LO      (USB_PID & 0xFF)
#define USB_PID_HI      ((USB_PID >> 8) & 0xFF)

/*-------------------------------------------*/
/*        ROM Data Definition                */
/*-------------------------------------------*/
const uint8_t gau8DeviceDescriptor[] =
{
    LEN_DEVICE,     /* bLength              */
    DESC_DEVICE,    /* bDescriptorType      */
#if defined(BILL_CLASS)
    0x01, 0x02,     /* bcdUSB               */
    0x11,           /* bDeviceClass         */
#else
    0x00, 0x02,     /* bcdUSB               */
    0x02,           /* bDeviceClass         */
#endif
    0x00,           /* bDeviceSubClass      */
    0x00,           /* bDeviceProtocol      */
#if defined(BILL_CLASS)
    VCOM_CTRL_MXPLD, /* bMaxPacketSize0      */
    USB_VID_LO,
    USB_VID_HI,     /* Veondor ID           */
    USB_PID_LO,     
    USB_PID_HI,     /* Product ID           */
    /* Setting 1.00 Revision */
    0x00, 0x01,     /* bcdDevice            */
#else
    BULK_EP_MXPLD,  /* bMaxPacketSize0      */
    USB_VID_LO,
    USB_VID_HI,     /* Veondor ID           */
    USB_PID_LO,
    USB_PID_HI,     /* Product ID           */
    0x00, 0x03,     /* bcdDevice            */
#endif
#if defined(BILL_CLASS)
    0x01,           /* iManufacture         */
    0x02,           /* iProduct             */
#else
    0x01,           /* iManufacture         */
    0x02,           /* iProduct             */
#endif
    0x03,           /* iSerialNumber        */
    0x01            /* bNumConfigurations   */
};

const uint8_t gau8ConfigDescriptor[] =
{
    LEN_CONFIG,     /* bLength              */
    DESC_CONFIG,    /* bDescriptorType      */
#if defined(BILL_CLASS)
    //TODO update this with complete descriptor length
    0x12, 0x00,     /* wTotalLength         */
#else
    0x43, 0x00,     /* wTotalLength         */
#endif
#if defined(BILL_CLASS)
    0x01,           /* bNumInterfaces       */
#else
    0x02,           /* bNumInterfaces       */
#endif
    0x01,           /* bConfigurationValue  */
    0x04,           /* iConfiguration       */
#if defined(BILL_CLASS)
    0x00,           /* bmAttributes         */
    0x00,           /* MaxPower             */
#else
    0xC0,           /* bmAttributes         */
    0x32,           /* MaxPower             */
#endif
#if defined(BILL_CLASS)
    /* INTERFACE descriptor */
    LEN_INTERFACE,  /* bLength              */
    DESC_INTERFACE, /* bDescriptorType      */
    0x00,           /* bInterfaceNumber     */
    0x00,           /* bAlternateSetting    */
    0x00,           /* bNumEndpoints        */
    0x11,           /* bInterfaceClass      */
    0x00,           /* bInterfaceSubClass   */
    0x00,           /* bInterfaceProtocol   */
    0x05            /* iInterface           */
#else
    /* INTERFACE descriptor */
    LEN_INTERFACE,  /* bLength              */
    DESC_INTERFACE, /* bDescriptorType      */
    0x00,           /* bInterfaceNumber     */
    0x00,           /* bAlternateSetting    */
    0x01,           /* bNumEndpoints        */
    0x02,           /* bInterfaceClass      */
    0x02,           /* bInterfaceSubClass   */
    0x01,           /* bInterfaceProtocol   */
    0x00,            /* iInterface           */
#endif
#if !defined(BILL_CLASS)
    /* Communication Class Specified INTERFACE descriptor */
    0x05,           /* Size of the descriptor, in bytes */
    0x24,           /* CS_INTERFACE descriptor type */
    0x00,           /* Header functional descriptor subtype */
    0x10, 0x01,     /* Communication device compliant to the communication spec. ver. 1.10 */
    
    /* Communication Class Specified INTERFACE descriptor */
    0x05,           /* Size of the descriptor, in bytes */
    0x24,           /* CS_INTERFACE descriptor type */
    0x01,           /* Call management functional descriptor */
    0x00,           /* BIT0: Whether device handle call management itself. */
                    /* BIT1: Whether device can send/receive call management information over a Data Class Interface 0 */
    0x01,           /* Interface number of data class interface optionally used for call management */

    /* Communication Class Specified INTERFACE descriptor */
    0x04,           /* Size of the descriptor, in bytes */
    0x24,           /* CS_INTERFACE descriptor type */
    0x02,           /* Abstract control management funcational descriptor subtype */
    0x00,           /* bmCapabilities       */
    
    /* Communication Class Specified INTERFACE descriptor */
    0x05,           /* bLength              */
    0x24,           /* bDescriptorType: CS_INTERFACE descriptor type */
    0x06,           /* bDescriptorSubType   */
    0x00,           /* bMasterInterface     */
    0x01,           /* bSlaveInterface0     */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_INPUT | INT_IN_EP_NUM),     /* bEndpointAddress */
    EP_INT,                         /* bmAttributes     */
    INT_EP_MXPLD, 0x00,             /* wMaxPacketSize   */
    0x01,                           /* bInterval        */
            
    /* INTERFACE descriptor */
    LEN_INTERFACE,  /* bLength              */
    DESC_INTERFACE, /* bDescriptorType      */
    0x01,           /* bInterfaceNumber     */
    0x00,           /* bAlternateSetting    */
    0x02,           /* bNumEndpoints        */
    0x0A,           /* bInterfaceClass      */
    0x00,           /* bInterfaceSubClass   */
    0x00,           /* bInterfaceProtocol   */
    0x00,           /* iInterface           */
            
    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_INPUT | BULK_IN_EP_NUM),    /* bEndpointAddress */
    EP_BULK,                        /* bmAttributes     */
    BULK_EP_MXPLD, 0x00,            /* wMaxPacketSize   */
    0x00,                           /* bInterval        */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_OUTPUT | BULK_OUT_EP_NUM),  /* bEndpointAddress */
    EP_BULK,                        /* bmAttributes     */
    BULK_EP_MXPLD, 0x00,            /* wMaxPacketSize   */
    0x00,                           /* bInterval        */
#endif
};

const uint8_t gau8StringLang[] = {
    4,              /* bLength                  */
    DESC_STRING,    /* bDescriptorType          */
    0x09, 0x04      /* Language ID: USA(0x0409) */
};
#if defined(BILL_CLASS)
const uint8_t gau8VendorStringDescriptor[] = {
    16,             /* bLength          */
    DESC_STRING,    /* bDescriptorType  */
    'L', 0,
    'a', 0,
    't', 0,
    't', 0, 
    'i', 0,
    'c', 0,
    'e', 0
};

const uint8_t gau8ProductStringDescriptor[] = {
    40,             /* bLength          */
    DESC_STRING,    /* bDescriptorType  */
    '7', 0,
    '0', 0,
    '2', 0,
    '4', 0,
    ' ', 0,
    'U', 0, 
    'S', 0,
    'B', 0,
    ' ', 0,
    'B', 0,
    'i', 0,
    'l', 0,
    'l', 0,
    ' ', 0,
    'B', 0,
    'o', 0,
    'a', 0,
    'r', 0,
    'd', 0
};

const uint8_t gau8StringSerial[] =
{
    24,             /* bLength          */
    DESC_STRING,    /* bDescriptorType  */
    '7', 0,
    '0', 0,
    '2', 0,
    '4', 0,
    ' ', 0,
    'N', 0, 
    'U', 0,
    'C', 0,
    '1', 0,
    '2', 0, 
    '3', 0
};

const uint8_t gau8StringConf[] =
{
    40,             /* bLength          */
    DESC_STRING,    /* bDescriptorType  */
    '7', 0,
    '0', 0,
    '2', 0,
    '4', 0,
    ' ', 0,
    'B', 0,
    'i', 0,
    'l', 0,
    'l', 0,
    'b', 0,
    'o', 0,
    'a', 0,
    'r', 0,
    'd', 0,
    ' ', 0,
    'C', 0,
    'o', 0,
    'n', 0,
    'f', 0
};

const uint8_t gau8StringIntf[] =
{
    44,             /* bLength          */
    DESC_STRING,    /* bDescriptorType  */
    '7', 0,
    '0', 0,
    '2', 0,
    '4', 0,
    ' ', 0,
    'B', 0,
    'i', 0,
    'l', 0,
    'l', 0,
    'b', 0,
    'o', 0,
    'a', 0,
    'r', 0,
    'd', 0,
    ' ', 0,
    'D', 0,
    'e', 0,
    'v', 0,
    'i', 0,
    'c', 0,
    'e', 0
};

const uint8_t gau8StringURL[] =
{
    40,             /* bLength          */
    DESC_STRING,    /* bDescriptorType  */
    'w', 0,
    'w', 0,
    'w', 0,
    '.', 0,
    'l', 0,
    'a', 0,
    't', 0,
    't', 0,
    'i', 0,
    'c', 0,
    'e', 0,
    's', 0,
    'e', 0,
    'm', 0,
    'i', 0,
    '.', 0,
    'c', 0,
    'o', 0,
    'm', 0
};

const uint8_t gau8StringAlt1[] =
{
    8,             /* bLength          */
    DESC_STRING,    /* bDescriptorType  */
    'M', 0,
    'H', 0,
    'L', 0
};

const uint8_t gau8StringAlt2[] =
{
    26,             /* bLength          */
    DESC_STRING,    /* bDescriptorType  */
    'D', 0,
    'i', 0,
    's', 0,
    'p', 0,
    'l', 0,
    'a', 0,
    'y', 0,
    ' ', 0,
    'P', 0,
    'o', 0,
    'r', 0,
    't', 0
};
#else
const uint8_t gau8VendorStringDescriptor[] = {
    16,             /* bLength          */
    DESC_STRING,    /* bDescriptorType  */
    'N', 0, 
    'u', 0, 
    'v', 0, 
    'o', 0, 
    't', 0, 
    'o', 0, 
    'n', 0
};

const uint8_t gau8ProductStringDescriptor[] = {
    32,             /* bLength          */
    DESC_STRING,    /* bDescriptorType  */
    'U', 0, 
    'S', 0,
    'B', 0,
    ' ', 0,
    'V', 0,
    'i', 0,
    'r', 0,
    't', 0,
    'u', 0,
    'a', 0,
    'l', 0,
    ' ', 0,
    'C', 0,
    'O', 0,
    'M', 0
};

const uint8_t gau8StringSerial[26] =
{
    26,             /* bLength          */
    DESC_STRING,    /* bDescriptorType  */
    'N', 0, 
    'T', 0, 
    '2', 0, 
    '0', 0, 
    '0', 0, 
    '9', 0, 
    '1', 0, 
    '0', 0, 
    '1', 0, 
    '4', 0, 
    '0', 0, 
    '0', 0
};
#endif
#if defined(BILL_CLASS)
const uint8_t gau8BosHdrDescriptor[] =
{
		LEN_BOS, 	  /* bLength */
		DESC_BOS,     /* bDescriptorType */
        (LEN_BOS +
         LEN_CONTID +
         LEN_BBCAPS_BASE +
         LEN_BBCAPS_SVID *1), /* wTotalLength - Total length of of hdr + all dev caps */
        0x00,
        0x02,           /*bNumDeviceCaps  - Container ID Descriptor + others */

        /* Contid Caps */
        LEN_CONTID,		/* bLength */
        DESC_DEVCAP,	/* bDescriptorType */
        DESC_CAPTYPE,	/* bDevCapabilityType */
        0x00,           /* Reserved */
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, /* Container Id */

        /* bb caps */
        LEN_BBCAPS_BASE + LEN_BBCAPS_SVID *1,		/* bLength */
        DESC_DEVCAP,	/* bDescriptorType */
        DESC_CAPBILL,	/* bDevCapabilityType */
        0x06,				/* iAdditionalInfoURL */
        2,				/* bNumberOfAlternateModes */
        1,				/* bPreferredAlternateMode */
        0x00, 0x00,				/* VconnPower */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* bmConfigured */
        0x10, 0x01,      /* bcdVersion */
        0x00,			/* bAdditionalFailureInfo */
        0x00,           /* bReserved */

        /* bb caps svids */
        0x01, 0xFF,        /* wSVID MHL */
        0x01,              /* bAlternateMode */
        0x07,               /* iAlternateModeString */

        /* bb caps svids */
        0x02, 0xFF,        /* wSVID DP */
        0x02,              /* bAlternateMode */
        0x08               /* iAlternateModeString */
 };
#endif

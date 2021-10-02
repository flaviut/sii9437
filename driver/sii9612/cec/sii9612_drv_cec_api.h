/******************************************************************************
 *
 * 2017 (c) Lattice Semiconductor Corporation
 * 
 * This program is free software; you can redistribute it and/or
 * modify it only under the terms of version 2 of the GNU General Public License
 * as published by the Free Software Foundation, and not any later version.
 * 
 * This program is distributed "AS IS" WITHOUT ANY WARRANTY of any kind,
 * whether express or implied, including without limitation the implied warranty
 * of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE or NON-INFRINGEMENT.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 
 * You may contact Lattice Semiconductor Corporation at
 * 111 SW 5th Ave, Suite 700, Portland, OR 97204.
 *
 *****************************************************************************/
/*************************************************************************
* sii9612_drv_api.h
*************************************************************************/
#ifndef SI_DRV_CEC_API_H
#define SI_DRV_CEC_API_H

#include "sii_datatypes.h"
#include "sii_drv_cra_api.h"
#include "sii9612_reg_cec.h"
#include "sii943x_func.h"

#define SII_MAX_CMD_SIZE 16 //!< Maximum length of CEC command (in bytes)

#define MAKE_SRCDEST( src, dest )   ((( (src) << 4) & 0xF0) | ((dest) & 0x0F))
#define GET_CEC_SRC( srcDest )      (( srcDest >> 4) & 0x0F)
#define GET_CEC_DEST( srcDest )     (( srcDest >> 0) & 0x0F)

#define MAX_CEC_PARSERS             8   // Number of CEC message handlers that can
                                        // be registered with this instance.
#define MAX_CEC_TASKS               3   // Number of CEC Task handlers that can
                                        // be registered with this instance.
// CDC Tasks related time constants
#define CEC_MAX_RESPONSE_TIME_MS         1500   //!< milliseconds to wait before ending of each response gathering (required maximum = 1s, exception may apply)
#define CEC_MIN_RECALL_TIME_MS           100    //!< shortest CEC task handler recall time in milliseconds
#define CEC_MAX_RECALL_TIME_MS           20000  //!< longest CEC task handler recall time in milliseconds
#define CEC_UC_REPETITION_TIME_MS        350    //!< user control repetition time (200-500ms)
#define CEC_PING_ACK_WAITING_TIME_MS     2000   //!< maximum waiting time for acknowledge of the CEC ping

typedef enum _SiiDrvCecError_t
{
    RESULT_CEC_SUCCESS,             // Success result code
    RESULT_CEC_FAIL,                // General Failure result code

    RESULT_CEC_INVALID_PARAMETER,

    RESULT_CEC_INVALID_LOGICAL_ADDRESS,
    RESULT_CEC_INVALID_PHYSICAL_ADDRESS,
    RESULT_CEC_INVALID_PORT_INDEX,
    RESULT_CEC_NOT_ADJACENT,
    RESULT_CEC_NO_PA_FOUND,
    RESULT_CEC_TASK_QUEUE_FULL,
    RESULT_CEC_NO_LA_FOUND,

} SiiDrvCecError_t;


typedef enum
{
    SiiCEC_PORT_CHANGE          = 0x0001,
    SiiCEC_POWERSTATE_CHANGE    = 0x0002,
    SiiCEC_SOURCE_LOST          = 0x0004,
} SiiCecStatus_t;


typedef enum _SiiCecAbortReason_t   // Operands for <Feature Abort> Opcode
{
    CECAR_UNRECOG_OPCODE            = 0x00,
    CECAR_NOT_CORRECT_MODE,
    CECAR_CANT_PROVIDE_SOURCE,
    CECAR_INVALID_OPERAND,
    CECAR_REFUSED,
    CECAR_UNABLE_TO_DETERMINE
} SiiCecAbortReason_t;


typedef enum _SiiCecLogicalAddresses_t
{
    CEC_LOGADDR_TV          = 0x00,
    CEC_LOGADDR_RECDEV1     = 0x01,
    CEC_LOGADDR_RECDEV2     = 0x02,
    CEC_LOGADDR_TUNER1      = 0x03,     // STB1 in Spev v1.3
    CEC_LOGADDR_PLAYBACK1   = 0x04,     // DVD1 in Spev v1.3
    CEC_LOGADDR_AUDSYS      = 0x05,
    CEC_LOGADDR_PURE_SWITCH      = 0x06,     // STB2 in Spec v1.3
    CEC_LOGADDR_VIDEO_PROCESSOR      = 0x07,     // STB3 in Spec v1.3
    CEC_LOGADDR_PLAYBACK2   = 0x08,     // DVD2 in Spec v1.3
    CEC_LOGADDR_RECDEV3     = 0x09,
    CEC_LOGADDR_TUNER4      = 0x0A,     // RES1 in Spec v1.3
    CEC_LOGADDR_PLAYBACK3   = 0x0B,     // RES2 in Spec v1.3
    CEC_LOGADDR_RES3        = 0x0C,
    CEC_LOGADDR_RES4        = 0x0D,
    CEC_LOGADDR_FREEUSE     = 0x0E,
    CEC_LOGADDR_UNREGORBC   = 0x0F

} SiiCecLogicalAddresses_t;

typedef enum _SiiCecDeviceTypes_t   // CEC device types
{
    CEC_DT_TV,
    CEC_DT_RECORDING_DEVICE,
    CEC_DT_RSVD,
    CEC_DT_TUNER,
    CEC_DT_PLAYBACK,
    CEC_DT_AUDIO_SYSTEM,
    CEC_DT_PURE_CEC_SWITCH,
    CEC_DT_VIDEO_PROCESSOR,

    CEC_DT_COUNT
} SiiCecDeviceTypes_t;

typedef enum _SiiCecOpcodes_t       // CEC Messages
    {
    CECOP_SII_SENDPING              = 0x100,    // Special opcode for SiiDrvCpiWrite()
    CECOP_FEATURE_ABORT             = 0x00,
    CECOP_IMAGE_VIEW_ON             = 0x04,
    CECOP_TUNER_STEP_INCREMENT      = 0x05,     // N/A
    CECOP_TUNER_STEP_DECREMENT      = 0x06,     // N/A
    CECOP_TUNER_DEVICE_STATUS       = 0x07,     // N/A
    CECOP_GIVE_TUNER_DEVICE_STATUS  = 0x08,     // N/A
    CECOP_RECORD_ON                 = 0x09,     // N/A
    CECOP_RECORD_STATUS             = 0x0A,     // N/A
    CECOP_RECORD_OFF                = 0x0B,     // N/A
    CECOP_TEXT_VIEW_ON              = 0x0D,
    CECOP_RECORD_TV_SCREEN          = 0x0F,     // N/A
    CECOP_GIVE_DECK_STATUS          = 0x1A,
    CECOP_DECK_STATUS               = 0x1B,
    CECOP_SET_MENU_LANGUAGE         = 0x32,
    CECOP_CLEAR_ANALOGUE_TIMER      = 0x33,     // Spec 1.3A
    CECOP_SET_ANALOGUE_TIMER        = 0x34,     // Spec 1.3A
    CECOP_TIMER_STATUS              = 0x35,     // Spec 1.3A
    CECOP_STANDBY                   = 0x36,
    CECOP_PLAY                      = 0x41,
    CECOP_DECK_CONTROL              = 0x42,
    CECOP_TIMER_CLEARED_STATUS      = 0x43,     // Spec 1.3A
    CECOP_USER_CONTROL_PRESSED      = 0x44,
    CECOP_USER_CONTROL_RELEASED     = 0x45,
    CECOP_GIVE_OSD_NAME             = 0x46,
    CECOP_SET_OSD_NAME              = 0x47,
    CECOP_SET_OSD_STRING            = 0x64,
    CECOP_SET_TIMER_PROGRAM_TITLE   = 0x67,     // Spec 1.3A
    CECOP_SYSTEM_AUDIO_MODE_REQUEST = 0x70,     // Spec 1.3A
    CECOP_GIVE_AUDIO_STATUS         = 0x71,     // Spec 1.3A
    CECOP_SET_SYSTEM_AUDIO_MODE     = 0x72,     // Spec 1.3A
    CECOP_REPORT_AUDIO_STATUS       = 0x7A,     // Spec 1.3A
    CECOP_GIVE_SYSTEM_AUDIO_MODE_STATUS = 0x7D, // Spec 1.3A
    CECOP_SYSTEM_AUDIO_MODE_STATUS  = 0x7E,     // Spec 1.3A
    CECOP_ROUTING_CHANGE            = 0x80,
    CECOP_ROUTING_INFORMATION       = 0x81,
    CECOP_ACTIVE_SOURCE             = 0x82,
    CECOP_GIVE_PHYSICAL_ADDRESS     = 0x83,
    CECOP_REPORT_PHYSICAL_ADDRESS   = 0x84,
    CECOP_REQUEST_ACTIVE_SOURCE     = 0x85,
    CECOP_SET_STREAM_PATH           = 0x86,
    CECOP_DEVICE_VENDOR_ID          = 0x87,
    CECOP_VENDOR_COMMAND            = 0x89,
    CECOP_VENDOR_REMOTE_BUTTON_DOWN = 0x8A,
    CECOP_VENDOR_REMOTE_BUTTON_UP   = 0x8B,
    CECOP_GIVE_DEVICE_VENDOR_ID     = 0x8C,
    CECOP_MENU_REQUEST              = 0x8D,
    CECOP_MENU_STATUS               = 0x8E,
    CECOP_GIVE_DEVICE_POWER_STATUS  = 0x8F,
    CECOP_REPORT_POWER_STATUS       = 0x90,
    CECOP_GET_MENU_LANGUAGE         = 0x91,
    CECOP_SELECT_ANALOGUE_SERVICE   = 0x92,     // Spec 1.3A    N/A
    CECOP_SELECT_DIGITAL_SERVICE    = 0x93,     //              N/A
    CECOP_SET_DIGITAL_TIMER         = 0x97,     // Spec 1.3A
    CECOP_CLEAR_DIGITAL_TIMER       = 0x99,     // Spec 1.3A
    CECOP_SET_AUDIO_RATE            = 0x9A,     // Spec 1.3A
    CECOP_INACTIVE_SOURCE           = 0x9D,     // Spec 1.3A
    CECOP_CEC_VERSION               = 0x9E,     // Spec 1.3A
    CECOP_GET_CEC_VERSION           = 0x9F,     // Spec 1.3A
    CECOP_VENDOR_COMMAND_WITH_ID    = 0xA0,     // Spec 1.3A
    CECOP_CLEAR_EXTERNAL_TIMER      = 0xA1,     // Spec 1.3A
    CECOP_SET_EXTERNAL_TIMER        = 0xA2,     // Spec 1.3A

    CECOP_REPORT_SHORT_AUDIO    	= 0xA3,     // Spec 1.4
    CECOP_REQUEST_SHORT_AUDIO    	= 0xA4,     // Spec 1.4

    CECOP_INITIATE_ARC              = 0xC0,
    CECOP_REPORT_ARC_INITIATED      = 0xC1,
    CECOP_REPORT_ARC_TERMINATED     = 0xC2,

    CECOP_REQUEST_ARC_INITIATION    = 0xC3,
    CECOP_REQUEST_ARC_TERMINATION   = 0xC4,
    CECOP_TERMINATE_ARC             = 0xC5,

    CDCOP_HEADER                    = 0xF8,
    CECOP_ABORT                     = 0xFF,

} SiiCecOpcodes_t;


typedef enum _SiiCecPowerstatus_t   // Operands for <Power Status> Opcode
    {
    CEC_POWERSTATUS_ON              = 0x00,
    CEC_POWERSTATUS_STANDBY         = 0x01,
    CEC_POWERSTATUS_STANDBY_TO_ON   = 0x02,
    CEC_POWERSTATUS_ON_TO_STANDBY   = 0x03,
    } SiiCecPowerstatus_t;

typedef struct _SiiCecData_t
{
    uint8_t         srcDestAddr;            // Source in upper nibble, dest in lower nibble
    SiiCecOpcodes_t opcode;
    uint8_t         args[ SII_MAX_CMD_SIZE ];
    uint8_t         argCount;
    uint16_t        msgId;
} SiiCecData_t;

typedef struct
{
    uint8_t srcDestAddr;                //!< Source is in upper nibble, destination is in lower nibble
    uint8_t opcode;                     //!< CEC operation code
    uint8_t args[SII_MAX_CMD_SIZE];     //!< Arguments of the CEC command
    uint8_t argCount;                   //!< Number of arguments
   	uint8_t nextFrameArgCount;          //!< Number of arguments in the next CEC frame

} SI_CpiData_t;

                            //! CPI Status structure, reflecting statuses of CEC transmitter and receiver
typedef struct 
{
    uint8_t rxState;        //!< CEC receiver's status
    uint8_t txState;        //!< CEC transmitter's status
    uint8_t cecError;       //!< Error code

} SI_CpiStatus_t;

typedef enum _SiiCpiTxState_t
{
    SiiTX_IDLE          = 0,
    SiiTX_WAITCMD,
    SiiTX_SENDING,
    SiiTX_SENDACKED,
    SiiTX_SENDFAILED,
    SiiTX_TIMEOUT
} SiiCpiTxState_t;

typedef enum _SiiDrvCpiStatus_t
{
    SiiCPI_CEC_INT          = 0x0001,
    SiiCPI_CEC_STATUS_VALID = 0x0002,
    SiiCPI_CEC_DISABLED     = 0x0004,
} SiiDrvCpiStatus_t;

//-------------------------------------------------------------------------------
// CPI data structures
//-------------------------------------------------------------------------------

typedef struct _SiiCpiData_t
{
    uint8_t         srcDestAddr;            // Source in upper nibble, dest in lower nibble
    SiiCecOpcodes_t opcode;
    uint8_t         args[ SII_MAX_CMD_SIZE ];
    uint8_t         argCount;
    SiiCpiTxState_t txState;
    uint16_t        msgId;
} SiiCpiData_t;

//-------------------------------------------------------------------------------
// CPI data structures
//-------------------------------------------------------------------------------

typedef struct _SiiCpiStatus_t
{
    uint8_t         rxState;
    SiiCpiTxState_t txState;
    uint8_t         cecError;
    uint16_t        msgId;

} SiiCpiStatus_t;

/************************************************************************/
/*  EVENT Notifications                                                 */
/************************************************************************/
#define SII_9612_CEC_EVENT_FLAG__ARC_INIT		0x000000000000001
#define SII_9612_CEC_EVENT_FLAG__ARC_TERM		0x000000000000002

/************************************************************************/
/*  Type Definitions                                                    */
/************************************************************************/
void SiiDrvCecCreate(SiiCecDeviceTypes_t dev_type,
		Sii943xEventCallbackFunc_t pCallBack);
void SiiDrvCecHandle( void );
bool_t SiiDrvCecArcRxStart( void );
bool_t SiiDrvCecArcTxStart( void );
bool_t SiiDrvCecArcRxStop( void );
bool_t SiiDrvCecArcTxStop( void );
bool_t SiiDrvCecPASet( uint16_t cec_pa );

#endif /* SI_DRV_CEC_API_H*/

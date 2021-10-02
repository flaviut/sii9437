

#include "string.h"
#include "NUC123Series.h"
#include "LibRingBuf.h"
#include "Vcom_Data_Proc.h"
#include "FMC.h"
#include "Host_Common.h"
#include "Host_Core.h"
#include "I2C_MASTER.h"
#define DATA_RINGBUF_SIZE           (600)
#define CMD_OPERATION_END           (0x0d)   /* Ctrl-M or Enter */
/*-------------------------------------------*/
/*              Enum Definition              */
/*-------------------------------------------*/
typedef enum
{
    eTRANSFER_TYPE_INITIAL = 0,
    eTRANSFER_TYPE_MANAGE_SIIMON_CMD = 0xFF,                /* Ctrl-J */
    eTRANSFER_TYPE_MANAGE_VCOM_CMD = 0x0a,                /* Ctrl-J */
    eTRANSFER_TYPE_PROGRAM_SPI_FLASH_PREPARE = 0x07,      /* Ctrl-G */
    eTRANSFER_TYPE_PROGRAM_SPI_FLASH_MAIN_PROCESS = 0x03, /* Ctrl-C */
    eTRANSFER_TYPE_READ_SPI_FLASH_PREPARE = 0x0C,         /* Ctrl-L */
    eTRANSFER_TYPE_READ_SPI_FLASH_MAIN_PROCESS = 0x08,    /* Ctrl-H */
    eTRANSFER_TYPE_OTA_PROGRAM_PREPARE = 0x04,            /* Ctrl-G */
    eTRANSFER_TYPE_OTA_PROGRAM_MAIN_PROCESS = 0x05,       /* Ctrl-C */
    eTRANSFER_TYPE_GET_HOST_VERSION = 0x76,
}eTRANSFER_TYPE;

typedef enum
{
    eTRANSFER_STATUS_NOT_START = 0,
    eTRANSFER_STATUS_IN_PROGRESS,
    eTRANSFER_STATUS_FINISH,
    eTRANSFER_STATUS_NUM_OF_STATUS
}eTRANSFER_STATUS;

typedef enum
{
    eBB_STATUS_SLEEP = 0,
    eBB_STATUS_NORMAL,
    eBB_STATUS_RESET,
    eBB_STATUS_UNKNOWN,
    NUMBER_OF_BB_STATUS
}eBB_STATUS;

/*-------------------------------------------*/
/*        Internal Variable Definition       */
/*-------------------------------------------*/
static uint8_t  u8ModuleIdLen = 0;
static uint32_t u32ModuleIdNum[3] = {0x00};
static uint32_t u32ModuleIdDataFlashAddr;

static uint32_t u32TotalCount = 0;
static uint8_t u8DataRingBuf[DATA_RINGBUF_SIZE];
static eTRANSFER_TYPE eTransferType = eTRANSFER_TYPE_INITIAL;
static eTRANSFER_STATUS eTransferStatus = eTRANSFER_STATUS_NOT_START;
static RINGBUF_INFO sDataRingBufInfo;
static void s_Host_Get_ID_Num_Length( uint32_t* u32IDNum, uint8_t* u8IDLength );
static void s_Host_Data_Recv_Proc(uint8_t *pu8EpBuf, uint32_t u32Size);

/*-------------------------------------------*/
/*        External Variable Definition       */
/*-------------------------------------------*/
volatile uint8_t u8I2cProcCount = 0;
eDUT_BOARD_TYPE eDUTBoardType = eDUT_BOARD_TYPE_UNKNOWN;
eUART_STATUS_TYPE eUartStatusType = eUART_STATUS_TYPE_UNKNOWN;

uint8_t  u8DataBuf[300] = {0x00};
uint32_t u32DataCount = 0;

/*********************************************
@brief:This function initialize parameter

@param:[In]None

@return:None
*********************************************/
static void s_Host_Param_Init(void)
{
    /* Clear Total Number */
    u32TotalCount = 0;
    /* Clear Transfer Type */
    eTransferType = eTRANSFER_TYPE_INITIAL;
    /* Clear Transfer Status */
    eTransferStatus = eTRANSFER_STATUS_NOT_START;
    /* Ring Buffer Flushing */
    LibRingBufFlush( &sDataRingBufInfo );

    return;
}

/*********************************************
@brief:This function Update ID Number

@param:[In]None

@return:None
*********************************************/
static void s_Host_ID_Num_Update(void)
{
    uint32_t u32ModuleId;

    UNLOCKREG();

    /* Get Module ID Address in Data Flash Area - For NUC122, addr=0x1F000 as hard code address. */
    u32ModuleIdDataFlashAddr = FMC_READ_DATAFLASH_BASEADDRESS();

    /* Read Module ID from Data Flash */
    DrvFMC_Read( u32ModuleIdDataFlashAddr, &u32ModuleId );

    if( u32ModuleId == 0xFFFFFFFF )
    {
        /* For the First Time, the Module ID Number should be 0x00 */
        u32ModuleIdNum[0] = 0x00;
        u32ModuleIdNum[1] = 0x00;
        u32ModuleIdNum[2] = 0x00;
        /* Erase One Page ( 0x200 - 512bytes ) before Write */
        DrvFMC_Erase(u32ModuleIdDataFlashAddr);
        /* Update Module ID Number */
        DrvFMC_Write( ( u32ModuleIdDataFlashAddr + 0 ), u32ModuleIdNum[0] );
        DrvFMC_Write( ( u32ModuleIdDataFlashAddr + 4 ), u32ModuleIdNum[1] );
        DrvFMC_Write( ( u32ModuleIdDataFlashAddr + 8 ), u32ModuleIdNum[2] );
        /* Set Bow and Arrow ID Number Length Parameter */
        u8ModuleIdLen = 0;
    }
    else
    {
        /* Read Module ID Number from Data Flash */
        DrvFMC_Read( ( u32ModuleIdDataFlashAddr + 0 ), &u32ModuleIdNum[0] );
        DrvFMC_Read( ( u32ModuleIdDataFlashAddr + 4 ), &u32ModuleIdNum[1] );
        DrvFMC_Read( ( u32ModuleIdDataFlashAddr + 8 ), &u32ModuleIdNum[2] );
        /* Set Bow and Arrow ID Number Length */
        s_Host_Get_ID_Num_Length( u32ModuleIdNum, &u8ModuleIdLen );
    }

    LOCKREG();

    return;
}

/*-------------------------------------------*/
/*        Internal Function                  */
/*-------------------------------------------*/
/*********************************************
@brief:This function exexutes the Initialize Process for Function and Parameter

@param:[In]None

@return:None
*********************************************/
static void s_Host_Param_Func_Init(void)
{
    /* Data Parameter Initialization */
    s_Host_Param_Init();

    /* Ring Buffer Initialization */
    LibRingBufInit( &sDataRingBufInfo, u8DataRingBuf, sizeof(u8DataRingBuf) );

    /* Install the Data Send Out Callback Function ( PC -> Nuvoton )*/
    Vcom_Set_DataSendOutCallback( s_Host_Data_Recv_Proc );

    /* Update ID Number */
    s_Host_ID_Num_Update();

    return;
}

/*********************************************
@brief:This function Execute Send the BB command through UART

@param:[In]uint8_t *pu8CmdData
       [In]uint32_t u32CmdLen

@return:None
*********************************************/
static void s_Host_CMD_BB_UART( uint8_t *pu8CmdData, uint32_t u32CmdLen )
{
    if( ( eDUTBoardType == eDUT_BOARD_TYPE_MARBLE ) &&
        ( eUartStatusType == eUART_STATUS_TYPE_NOT_AVAILABLE ) )
    {
        Vcom_Data_Return_Proc((uint8_t *)"<CMDBEGIN>UART_IS_NOT_OPENED<EOF><CMDEND>",
                        strlen("<CMDBEGIN>UART_IS_NOT_OPENED<EOF><CMDEND>") );
    }
    else
    {
        /* UART Function */
        /*Uart_Debug_Message_Return_Proc( UART_PORT1 );*/
        /* Send the BB command through UART */
        /*Uart_Debug_Cmd_Send_Proc( UART_PORT1, pu8CmdData, u32CmdLen );*/
    }

    return;
}


/*********************************************
@brief:This function Execute Main Process of Executing BB Command

@param:[In]uint8_t *pu8CmdData
       [In]uint32_t u32CmdLen

@return:None
*********************************************/
static void s_Host_CMD_BB_Main_Proc( uint8_t *pu8CmdData, uint32_t u32CmdLen )
{
    eBB_STATUS eBBCurrentStatus = eBB_STATUS_NORMAL;
#if 0
    if( eDUTBoardType == eDUT_BOARD_TYPE_ARYAX02 )
    {
        /* For AryaX01 Board, because BB_STATUS pin is connected, Check Current BB Status */
        eBBCurrentStatus = s_Host_Check_BB_Status();
    }
#endif
    /* For other Boards, default eBBCurrentStatus to eBB_STATUS_NORMAL because no BB_STATUS is connected */
    /* Only if BB in Normal Status will Let Command Send to BB through UART or I2C */
    if( eBBCurrentStatus == eBB_STATUS_NORMAL )
    {
        /* Execute Some Preparation Work before BB Command Send to BB */
        //if( s_Host_CMD_BB_Prepare( pu8CmdData, u32CmdLen ) == TRUE )
        {
						uint8_t interfaceType = (pu8CmdData[1] >> 4);
            /* If the last 3 character(suffix) is "i2c", this Command is a i2c Command */
            if( interfaceType == 0 )
            {
								I2C_Cmd_Data_Send(pu8CmdData, strlen((const char *)pu8CmdData) );
                /* Send the BB command through I2C*/
                /*s_Host_CMD_BB_I2C(pu8CmdData, strlen(pu8CmdData));*/
            }
						else if(interfaceType == 1 )
						{
								//SPI CMD
						}
            else
            {
                /* Send the BB command through UART*/
                s_Host_CMD_BB_UART( pu8CmdData, u32CmdLen );
            }
        }
        /*else
        {
            Vcom_Data_Return_Proc( "<CMDBEGIN>Warning:This Setting is NOT allowed<EOF><CMDEND>",
                            strlen("<CMDBEGIN>Warning:This Setting is NOT allowed<EOF><CMDEND>") );
        }*/
    }
    else if( eBBCurrentStatus == eBB_STATUS_SLEEP )
    {
            Vcom_Data_Return_Proc((uint8_t *) "<CMDBEGIN>Warning:BB is in Sleep Mode<EOF><CMDEND>",
                            strlen("<CMDBEGIN>Warning:BB is in Sleep Mode<EOF><CMDEND>") );
    }
    else if( eBBCurrentStatus == eBB_STATUS_RESET )
    {
            Vcom_Data_Return_Proc((uint8_t *) "<CMDBEGIN>Warning:BB is in Reset Mode<EOF><CMDEND>",
                            strlen("<CMDBEGIN>Warning:BB is in Reset Mode<EOF><CMDEND>") );
    }
    else
    {
            Vcom_Data_Return_Proc((uint8_t *) "<CMDBEGIN>Warning:BB is in Unknown Mode<EOF><CMDEND>",
                            strlen("<CMDBEGIN>Warning:BB is in Unknown Mode<EOF><CMDEND>") );
    }

    return;
}

/*********************************************
@brief:This function executes Host Command and BB Command

@param:[In]None

@return:None
*********************************************/
static void s_Host_Core_Manage_VCOM_Command(void)
{
	uint32_t u32DataLen = 0;

	/* Disable USB Interrupt */
	NVIC_DisableIRQ(USBD_IRQn);

	if( eTransferStatus == eTRANSFER_STATUS_FINISH )
	{
		/* Get the Data Count of Ring Buffer */
		//u32DataCount = LibRingBufGetDataCnt( &sDataRingBufInfo );

		if( u32DataCount > 0 )
		{
			eTRANSFER_TYPE lTransferType = eTransferType;
			/* Get all the Data which have been stored in Ring Buffer */
			//LibRingBufGetBlkData( &sDataRingBufInfo, u8DataBuf, u32DataCount );
			/* Initialize the Parameter - Transfer End */
			s_Host_Param_Init();
			/* Enable USB Interrupt */
			NVIC_EnableIRQ(USBD_IRQn);
			/* The Data Length Should include '\n' */
			u32DataLen = ( u32DataCount + 1 );

			if (lTransferType != eTRANSFER_TYPE_MANAGE_SIIMON_CMD)
			{
				/* Execute Host Command and BB Command */
				s_Host_CMD_BB_Main_Proc( u8DataBuf, u32DataLen );
				/*s_Host_Core_Manage_VCOM_Command_Core( u8DataBuf, u32DataLen );*/
			}
			else
			{
				I2C_Cmd_Data_Send(u8DataBuf, u32DataLen );
			}
		}
		else
		{
			/* Initialize the Parameter - Transfer End */
			s_Host_Param_Init();
			/* Enable USB Interrupt */
			NVIC_EnableIRQ(USBD_IRQn);
			/* Return ERROR Message to PC */
			Vcom_Data_Return_Proc((uint8_t *) "<CMDBEGIN>ERROR_HAPPEN<EOF><CMDEND>", strlen("<CMDBEGIN>ERROR_HAPPEN<EOF><CMDEND>") );
		}
	}
	else
	{
		/* Enable USB Interrupt */
		NVIC_EnableIRQ(USBD_IRQn);
	}

    return;
}
/*********************************************
@brief:This function executes VCOM Command Analyze Process

@param:[In]None

@return:None
*********************************************/
void sii_vcom_command_process(void)
{
    switch( eTransferType )
    {
        /* USB(VCOM) Command */
	case eTRANSFER_TYPE_MANAGE_VCOM_CMD:
		break;
	case eTRANSFER_TYPE_MANAGE_SIIMON_CMD:
	{
		s_Host_Core_Manage_VCOM_Command();
		break;
	}
#if 0
        case eTRANSFER_TYPE_GET_HOST_VERSION:
        {
            s_Host_Core_Get_Host_Version();
            break;
        }
        case eTRANSFER_TYPE_PROGRAM_SPI_FLASH_PREPARE:
        {
            s_Host_Core_Program_SPI_Flash_Prepare();
            break;
        }
        case eTRANSFER_TYPE_PROGRAM_SPI_FLASH_MAIN_PROCESS:
        {
            s_Host_Core_Program_SPI_Flash_Main_Process();
            break;
        }
        case eTRANSFER_TYPE_READ_SPI_FLASH_PREPARE:
        {
            s_Host_Core_Read_SPI_Flash_Prepare();
            break;
        }
        case eTRANSFER_TYPE_READ_SPI_FLASH_MAIN_PROCESS:
        {
            s_Host_Core_Read_SPI_Flash_Main_Process();
            break;
        }

        case eTRANSFER_TYPE_OTA_PROGRAM_PREPARE:
        {
            s_Host_Core_OTA_Program_Prepare();
            break;
        }
        case eTRANSFER_TYPE_OTA_PROGRAM_MAIN_PROCESS:
        {
            s_Host_Core_OTA_Program_Main_Process();
            break;
        }
#endif
        default:
        {
            /* Do Nothing */
            break;
        }
    }

    return;
}

/*-------------------------------------------*/
/*             External Function             */
/*-------------------------------------------*/
/*********************************************
@brief:This function executes the Main Process for VCOM

@param:[In]None

@return:None
*********************************************/
void Host_Core_Proc(void)
{

    /* Initialize Process - Parameter and Function */
    s_Host_Param_Func_Init();

    /* Poll and handle USB Events */
    //while(TRUE)
    //{
        /* Command Core Process */
    /*sii_vcom_command_process();*/
    //}
}


/*********************************************
@brief:This function checks the Transfer Type for Application Processor(AP)( Execute in Interrupt )

@param:[In]uint8_t u8Inputchar

@return:None
*********************************************/
static void s_Host_Int_CheckTransType( uint8_t u8Inputchar )
{
    switch( (eTRANSFER_TYPE)u8Inputchar )
    {
        /* USB(VCOM) Command */
       case eTRANSFER_TYPE_MANAGE_VCOM_CMD:
        {
            eTransferType   = eTRANSFER_TYPE_MANAGE_VCOM_CMD;
            eTransferStatus = eTRANSFER_STATUS_IN_PROGRESS;
						u32DataCount = 0;
            break;
        }
       case eTRANSFER_TYPE_MANAGE_SIIMON_CMD:
        {
            eTransferType   = eTRANSFER_TYPE_MANAGE_SIIMON_CMD;
            eTransferStatus = eTRANSFER_STATUS_IN_PROGRESS;
						u32DataCount = 0;
            break;
        }
#if 0
        case eTRANSFER_TYPE_GET_HOST_VERSION:
        {
            eTransferType   = eTRANSFER_TYPE_GET_HOST_VERSION;
            eTransferStatus = eTRANSFER_STATUS_IN_PROGRESS;
            break;
        }
        /* Program SPI Flash */
        case eTRANSFER_TYPE_PROGRAM_SPI_FLASH_PREPARE:
        {
            eTransferType   = eTRANSFER_TYPE_PROGRAM_SPI_FLASH_PREPARE;
            eTransferStatus = eTRANSFER_STATUS_IN_PROGRESS;
            break;
        }
        case eTRANSFER_TYPE_PROGRAM_SPI_FLASH_MAIN_PROCESS:
        {
            eTransferType   = eTRANSFER_TYPE_PROGRAM_SPI_FLASH_MAIN_PROCESS;
            eTransferStatus = eTRANSFER_STATUS_IN_PROGRESS;
            break;
        }
        case eTRANSFER_TYPE_READ_SPI_FLASH_PREPARE:
        {
            eTransferType   = eTRANSFER_TYPE_READ_SPI_FLASH_PREPARE;
            eTransferStatus = eTRANSFER_STATUS_IN_PROGRESS;
            break;
        }
        case eTRANSFER_TYPE_READ_SPI_FLASH_MAIN_PROCESS:
        {
            eTransferType   = eTRANSFER_TYPE_READ_SPI_FLASH_MAIN_PROCESS;
            eTransferStatus = eTRANSFER_STATUS_IN_PROGRESS;
            break;
        }
        /* OTA Program */
        case eTRANSFER_TYPE_OTA_PROGRAM_PREPARE:
        {
            eTransferType   = eTRANSFER_TYPE_OTA_PROGRAM_PREPARE;
            eTransferStatus = eTRANSFER_STATUS_IN_PROGRESS;
            break;
        }
        case eTRANSFER_TYPE_OTA_PROGRAM_MAIN_PROCESS:
        {
            eTransferType   = eTRANSFER_TYPE_OTA_PROGRAM_MAIN_PROCESS;
            eTransferStatus = eTRANSFER_STATUS_IN_PROGRESS;
            break;
        }
#endif
        default:
        {
            /* Do Nothing */
            break;
        }
    }

    return;
}

/*********************************************
@brief:This function analyzes the Input data from USB/VCOM( Execute in Interrupt )

@param:[In]uint8_t u8Inputchar

@return:None
*********************************************/
static void s_Host_Int_ManageInputData( uint8_t u8Inputchar )
{
    switch( eTransferType )
    {
        /* USB(VCOM) Command */
        case eTRANSFER_TYPE_GET_HOST_VERSION:
        case eTRANSFER_TYPE_MANAGE_VCOM_CMD:
        {
            /* For USB(VCOM) Command, data is transferred by ASCII code, the End Token is "0x0D"("\") */
            /* If the Data is "End Token", Do NOT Store the Data into Ring Buffer */
            /* If the Data is NOT "End Token", Store the Data to Ring Buffer */
            if( u8Inputchar == CMD_OPERATION_END )    /* 0x0D - "\" */
            {
                /* Set Transfer End Flag */
                eTransferStatus = eTRANSFER_STATUS_FINISH;
                //break;
            }
        }
        case eTRANSFER_TYPE_MANAGE_SIIMON_CMD:
        /* Program SPI Flash */
        case eTRANSFER_TYPE_PROGRAM_SPI_FLASH_PREPARE:
        case eTRANSFER_TYPE_PROGRAM_SPI_FLASH_MAIN_PROCESS:
        case eTRANSFER_TYPE_READ_SPI_FLASH_PREPARE:
        case eTRANSFER_TYPE_READ_SPI_FLASH_MAIN_PROCESS:
        /* OTA Program */
        case eTRANSFER_TYPE_OTA_PROGRAM_PREPARE:
        case eTRANSFER_TYPE_OTA_PROGRAM_MAIN_PROCESS:
        {
            /* Store the Data to Ring Buffer */
            /* For SPI Flash Upgrade and OTA Program Command - Just Increment Data Counter and Store the Data into Ring Buffer */
            /* Because it transfers Binary Data, not ASCII So the Ending Index is NOT "0x0D" */
            u32TotalCount++;
            u8DataBuf[u32DataCount] = u8Inputchar;
            u32DataCount++;
            //LibRingBufSetOneByteData( &sDataRingBufInfo, u8Inputchar );
            break;
        }
        default:
        {
            /* Do Nothing */
            break;
        }
    }

    return;
}



/*********************************************
@brief:This function exexutes the Data Receive from PC Process( Execute in Interrupt )

@param:[In]uint8_t *pu8EpBuf,
       [In]uint32_t u32Size

@return:None
*********************************************/
static void s_Host_Data_Recv_Proc(uint8_t *pu8EpBuf, uint32_t u32Size)
{
    uint8_t u8InputValue;
    int i;

		if (u32Size == 0)
				return;
    for( i = 0; i < u32Size; i++ )
    {
        /* Get the Input Character from USB/VCOM */
        u8InputValue = pu8EpBuf[i];

        if( eTransferStatus == eTRANSFER_STATUS_NOT_START )
        {
            /* Transfer is NOT Started yet, Check the Start Token to Get Transfer Type */
            s_Host_Int_CheckTransType( u8InputValue );
        }
				if (eTransferStatus == eTRANSFER_STATUS_FINISH)
					eTransferStatus = eTRANSFER_STATUS_IN_PROGRESS; // data transfer is not finished yet
        if (eTransferStatus == eTRANSFER_STATUS_IN_PROGRESS)
        {
			eTransferStatus = eTRANSFER_STATUS_IN_PROGRESS;
            /* Transfer has been Started, Manage the Input Character */
            s_Host_Int_ManageInputData( u8InputValue );
        }
    }
		if (eTransferType == eTRANSFER_TYPE_MANAGE_SIIMON_CMD)
			eTransferStatus = eTRANSFER_STATUS_FINISH;

    return;
}



/*********************************************
@brief:This function Get ID Number Length

@param:[IN]uint32_t* u32IDNum
       [OUT]uint8_t* u8IDLength

@return:[IN]None
*********************************************/
static void s_Host_Get_ID_Num_Length( uint32_t* u32IDNum, uint8_t* u8IDLength )
{
    uint8_t u8DataIndex[12];
    uint8_t Len = 0;

    UINT32_TO_UINT8_ID( u32IDNum[0] , u8DataIndex );
    UINT32_TO_UINT8_ID( u32IDNum[1] , ( u8DataIndex + 4 ) );
    UINT32_TO_UINT8_ID( u32IDNum[2] , ( u8DataIndex + 8 ) );

    while( u8DataIndex[Len] != 0x00 )
    {
        Len++;
        if( Len >= sizeof(u8DataIndex) )
        {
            break;
        }
    }

    (*u8IDLength) = Len;

    return;
}

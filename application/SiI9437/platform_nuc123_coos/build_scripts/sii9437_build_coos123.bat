@ECHO off

SET SII_USERNAME=""

IF NOT DEFINED USERNAME (
	ECHO please make sure to define USERNAME environment variable
	pause 
	EXIT
) ELSE (
	SET SII_USERNAME="%USERNAME%"
)

IF NOT DEFINED P4_CHANGELIST (
	SET SII_REVISION="unknown"
) ELSE (
	SET SII_REVISION="%P4_CHANGELIST%"
)

IF NOT DEFINED SALZBURG_API_GUI_BRANCH (
	SET SII_BRANCH=""
) ELSE (
	SET SII_BRANCH="%SALZBURG_API_GUI_BRANCH%"
)

IF NOT DEFINED RELEASE_BUILD (
	SET RELEASE_VERSION_MAJOR=0
	SET RELEASE_VERSION_MINOR=80
	SET RELEASE_CUSTOMER=0
	SET RELEASE_PATCH=0
)

SET "ROOT_DIR=%cd:~0,2%"
ECHO %ROOT_DIR% 

IF EXIST "%ROOT_DIR%\Salzburg_sii9437_build_coos123" ( 
;    RMDIR /s /q %ROOT_DIR%\Salzburg_sii9437_build_coos123
)

MKDIR %ROOT_DIR%\Salzburg_sii9437_build_coos123

CD ..\..\..\..\

SET "SOURCE_DIR=%CD%"

XCOPY /E application\sii9437\platform_nuc123_coos %ROOT_DIR%\Salzburg_sii9437_build_coos123\application\sii9437\platform_nuc123_coos\
XCOPY /E driver\internal %ROOT_DIR%\Salzburg_sii9437_build_coos123\driver\internal\
XCOPY /E driver\sii9437 %ROOT_DIR%\Salzburg_sii9437_build_coos123\driver\sii9437\
XCOPY /E driver\sii9612 %ROOT_DIR%\Salzburg_sii9437_build_coos123\driver\sii9612\
XCOPY /E platform\nuc123_coos %ROOT_DIR%\Salzburg_sii9437_build_coos123\platform\nuc123_coos\
COPY platform\sii_platform_api.h %ROOT_DIR%\Salzburg_sii9437_build_coos123\platform\sii_platform_api.h
XCOPY /E system\nuc123_coos %ROOT_DIR%\Salzburg_sii9437_build_coos123\system\nuc123_coos\

CD %ROOT_DIR%\Salzburg_sii9437_build_coos123

SET "PWD=%CD%"

attrib -r %PWD%\driver\sii9437\sii9437_version.h

echo #ifndef _SII9437_VERSION_H_ >%PWD%\driver\sii9437\sii9437_version.h
echo #define _SII9437_VERSION_H_ >>%PWD%\driver\sii9437\sii9437_version.h

echo #define SII_VERSION    		"%RELEASE_VERSION_MAJOR%.%RELEASE_VERSION_MINOR%.%RELEASE_CUSTOMER%.%RELEASE_PATCH%" >>%PWD%\driver\sii9437\sii9437_version.h
echo #define SII_REVISION   		%SII_REVISION% >>%PWD%\driver\sii9437\sii9437_version.h
echo #define SII_BRANCH     		%SII_BRANCH% >>%PWD%\driver\sii9437\sii9437_version.h
echo #define SII_USERNAME     		%SII_USERNAME% >>%PWD%\driver\sii9437\sii9437_version.h
echo #endif /* _SII9437_VERSION_H_ */ >>%PWD%\driver\sii9437\sii9437_version.h

CD %PWD%\application\sii9437\platform_nuc123_coos

:: Setting Build Version 
:: SET EXE_NAME=Sii9437_%RELEASE_VERSION_MAJOR%.%RELEASE_VERSION_MINOR%.%RELEASE_CUSTOMER%.%RELEASE_PATCH%.%P4_CHANGELIST%
SET EXE_NAME=Sii9437_build
SET PATH=%PATH%;C:\Program Files (x86)\GNU Tools ARM Embedded\4.9 2014q4\bin
Setlocal EnableDelayedExpansion
SET "BUILD_PATH=%CD%"
SET CC=arm-none-eabi-gcc
SET OBJ_COPY=arm-none-eabi-objcopy
SET C_FLAGS=-DNUC123ZD4AN0 -DSII_DEBUG_USB_VCOM -DSII_WORKAROUND_I2C -mcpu=cortex-m0 -mthumb -Wall -Werror -ffunction-sections -O1 -g
SET LD_FLAGS= -mcpu=cortex-m0 -mthumb -g -O1 
SET EXE_PATH=%BUILD_PATH%\bin\CoOS_Nu123\Debug\bin
SET OBJ_DIR=%BUILD_PATH%\bin\CoOS_Nu123\Debug\obj

::Remove previous release and build folder
::Create fresh build folder
IF EXIST "%OBJ_DIR%" ( 
;    RMDIR /s /q %OBJ_DIR%
)
IF EXIST "%EXE_PATH%" ( 
;    RMDIR /s /q %EXE_PATH%
)
MKDIR %OBJ_DIR%
MKDIR %EXE_PATH%

:: Source Files
SET C_FILES=^
%PWD%\system\nuc123_coos\cmsis_lib\source\DrvI2C.c ^
%PWD%\application\sii9437\platform_nuc123_coos\sii9437_example.c ^
%PWD%\system\nuc123_coos\CMSIS_Boot\Start_UP\startup_coide.c ^
%PWD%\system\nuc123_coos\CoOS\kernel\kernelHeap.c ^
%PWD%\system\nuc123_coos\CoOS\kernel\core.c ^
%PWD%\system\nuc123_coos\CoOS\kernel\timer.c ^
%PWD%\system\nuc123_coos\CoOS\kernel\utility.c ^
%PWD%\system\nuc123_coos\cmsis_lib\source\DrvUART.c ^
%PWD%\system\nuc123_coos\cmsis_lib\source\DrvFMC.c ^
%PWD%\system\nuc123_coos\CoOS\kernel\task.c ^
%PWD%\system\nuc123_coos\Library\LibTimer.c ^
%PWD%\system\nuc123_coos\CoOS\kernel\serviceReq.c ^
%PWD%\system\nuc123_coos\NUC123Series\system_NUC123Series.c ^
%PWD%\system\nuc123_coos\stdio\printf.c ^
%PWD%\system\nuc123_coos\CoOS\kernel\mbox.c ^
%PWD%\system\nuc123_coos\Application\Host_Core.c ^
%PWD%\system\nuc123_coos\CoOS\kernel\mm.c ^
%PWD%\system\nuc123_coos\cmsis_lib\source\DrvTimer.c ^
%PWD%\system\nuc123_coos\cmsis_lib\source\DrvSYS.c ^
%PWD%\system\nuc123_coos\CoOS\kernel\time.c ^
%PWD%\system\nuc123_coos\cmsis_lib\source\DrvUSB.c ^
%PWD%\system\nuc123_coos\CoOS\kernel\event.c ^
%PWD%\system\nuc123_coos\Application\RomData.c ^
%PWD%\system\nuc123_coos\Application\VCOMSys.c ^
%PWD%\system\nuc123_coos\syscalls\syscalls.c ^
%PWD%\system\nuc123_coos\CoOS\kernel\queue.c ^
%PWD%\system\nuc123_coos\CoOS\portable\GCC\port.c ^
%PWD%\system\nuc123_coos\CoOS\kernel\mutex.c ^
%PWD%\system\nuc123_coos\CoOS\kernel\flag.c ^
%PWD%\system\nuc123_coos\CMSIS_Boot\system_NUC123.c ^
%PWD%\system\nuc123_coos\CoOS\portable\arch.c ^
%PWD%\system\nuc123_coos\Application\UART.c ^
%PWD%\system\nuc123_coos\Application\Vcom_Data_Proc.c ^
%PWD%\system\nuc123_coos\CoOS\kernel\sem.c ^
%PWD%\system\nuc123_coos\Library\LibRingBuf.c ^
%PWD%\system\nuc123_coos\CoOS\kernel\hook.c ^
%PWD%\system\nuc123_coos\Application\I2C_MASTER.c ^
%PWD%\driver\internal\cra\sii_drv_cra.c ^
%PWD%\driver\internal\malloc\sii_sys_malloc.c ^
%PWD%\driver\internal\obj\sii_sys_obj.c ^
%PWD%\driver\internal\log\sii_sys_log.c ^
%PWD%\driver\internal\time\sii_sys_time.c ^
%PWD%\driver\internal\assert\sii_sys_assert.c ^
%PWD%\driver\internal\pipe\sii_lib_pipe.c ^
%PWD%\driver\internal\seq\sii_sys_seq.c ^
%PWD%\driver\sii9612\edid\si_lib_edid.c ^
%PWD%\driver\sii9612\sii9612_drv_tpg.c ^
%PWD%\driver\sii9612\sii9612_drv_tx.c ^
%PWD%\driver\sii9612\cec\sii9612_drv_cec.c ^
%PWD%\driver\sii9437\sii9437_drv.c ^
%PWD%\driver\sii9437\sii9437_hal.c ^
%PWD%\system\nuc123_coos\com_interface.c ^
%PWD%\platform\nuc123_coos\sii_platform_api.c

:: Includes
SET INCLUDE_PATH=^
%PWD%\system\nuc123_coos\NUC123Series ^
%PWD%\system\nuc123_coos\Include\Host ^
%PWD%\system\nuc123_coos\Include\Library ^
%PWD%\system\nuc123_coos\cmsis_lib\include ^
%PWD%\system\nuc123_coos\CoOS\portable ^
%PWD%\system\nuc123_coos\Include\System ^
%PWD%\system\nuc123_coos\CoOS ^
%PWD%\system\nuc123_coos\Include\USB ^
%PWD%\application\sii9437\platform_nuc123_coos ^
%PWD%\system\nuc123_coos\CMSIS_Boot ^
%PWD%\system\nuc123_coos\Include ^
%PWD%\system\nuc123_coos\stdio ^
%PWD%\system\nuc123_coos\CoOS\kernel ^
%PWD%\system\nuc123_coos\Include\Application ^
%PWD%\system\nuc123_coos\cmsis_lib ^
%PWD%\system\nuc123_coos\cmsis_core ^
%PWD%\system\nuc123_coos ^
%PWD%\system ^
%PWD%\platform\nuc123_coos ^
%PWD%\platform ^
%PWD%\driver ^
%PWD%\driver\sii9437 ^
%PWD%\driver\sii9612 ^
%PWD%\driver\internal ^
%PWD%\driver\internal\cra ^
%PWD%\driver\internal\malloc ^
%PWD%\driver\internal\obj ^
%PWD%\driver\sii9612\aud ^
%PWD%\driver\sii9612\edid ^
%PWD%\driver\sii9612\cec ^
%PWD%\driver\sii9612\api ^
%PWD%\driver\internal\log ^
%PWD%\driver\internal\time ^
%PWD%\driver\internal\assert ^
%PWD%\driver\internal\pipe ^
%PWD%\driver\internal\seq

:: INITIALISATIONS 
SET INCLUDE_DIRECTORIES=
SET OBJS=

:: Add absolute path to include paths
for %%a in (%INCLUDE_PATH%) do SET "INCLUDE_DIRECTORIES=!INCLUDE_DIRECTORIES!-I%%a "

:: Compile Source Files
for %%a in (%C_FILES%) do ( 
    %CC% %C_FLAGS% %INCLUDE_DIRECTORIES% -c %%a -o %OBJ_DIR%\%%~na.o 
)

:: Create OBJS String 
for %%a in (%C_FILES%) do SET "OBJS=!OBJS!%OBJ_DIR%\%%~na.o "

:: Linking
%CC% %LD_FLAGS% -Wl,-Map=%EXE_PATH%\%EXE_NAME%.map -Wl,--gc-sections -L%PWD%/system/nuc123_coos -Wl,-T%PWD%/system/nuc123_coos/arm-gcc-link.ld -o %EXE_PATH%\%EXE_NAME%.elf %OBJS%

:: Generating Binary 
%OBJ_COPY% -O binary %EXE_PATH%\%EXE_NAME%.elf %EXE_PATH%\%EXE_NAME%.bin

IF NOT %ERRORLEVEL%==0 ( 
	echo ABORT: %ERRORLEVEL%
	exit /b %ERRORLEVEL%
)

attrib -r %SOURCE_DIR%\driver\sii9437\sii9437_version.h
COPY /Y %PWD%\driver\sii9437\sii9437_version.h %SOURCE_DIR%\driver\sii9437\sii9437_version.h

IF EXIST "%SOURCE_DIR%\application\sii9437\platform_nuc123_coos\bin" ( 
;    RMDIR /s /q %SOURCE_DIR%\application\sii9437\platform_nuc123_coos\bin
)
XCOPY /E %PWD%\application\sii9437\platform_nuc123_coos\bin %SOURCE_DIR%\application\sii9437\platform_nuc123_coos\bin\

CD %SOURCE_DIR%

IF EXIST "%ROOT_DIR%\Salzburg_sii9437_build_coos123" ( 
;    RMDIR /s /q %ROOT_DIR%\Salzburg_sii9437_build_coos123
)

pause

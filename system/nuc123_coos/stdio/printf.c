/**************************************************************************//*****
 * @file     printf.c
 * @brief    Implementation of several stdio.h methods, such as printf(), 
 *           sprintf() and so on. This reduces the memory footprint of the
 *           binary when using those methods, compared to the libc implementation.
 ********************************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "LibTimer.h"
#if defined(SII_DEBUG_UART1)
#include <UART.h>
#include "Vcom_Data_Proc.h"
#elif defined(SII_DEBUG_USB_VCOM)
#include "Vcom_Data_Proc.h"
#endif

/**
 * @brief  Transmit a char, if you want to use printf(), 
 *         you need implement this function
 *
 * @param  pStr	Storage string.
 * @param  c    Character to write.
 */
void PrintChar(char c)
{
#if defined(SII_DEBUG_UART1)
	UART_WRITE(UART1, c);
#elif defined(SII_DEBUG_USB_VCOM)
	Vcom_Data_Return_Proc((uint8_t *) &c, 1);
#endif
}

/** Maximum string size allowed (in bytes). */
#define MAX_STRING_SIZE         100


/** Required for proper compilation. */
struct _reent r = {0, (FILE *) 0, (FILE *) 1, (FILE *) 0};
/*struct _reent *_impure_ptr = &r;*/

/**
 * @brief  Writes a character inside the given string. Returns 1.
 *
 * @param  pStr	Storage string.
 * @param  c    Character to write.
 */
signed int PutChar(char *pStr, char c)
{
    *pStr = c;
    return 1;
}


/**
 * @brief  Writes a string inside the given string.
 *
 * @param  pStr     Storage string.
 * @param  pSource  Source string.
 * @return  The size of the written
 */
signed int PutString(char *pStr, const char *pSource)
{
    signed int num = 0;

    while (*pSource != 0) {

        *pStr++ = *pSource++;
        num++;
    }

    return num;
}


/**
 * @brief  Writes an unsigned int inside the given string, using the provided fill &
 *         width parameters.
 *
 * @param  pStr  Storage string.
 * @param  fill  Fill character.
 * @param  width  Minimum integer width.
 * @param  value  Integer value.   
 */
signed int PutUnsignedInt(
    char *pStr,
    char fill,
    signed int width,
    unsigned int value)
{
    signed int num = 0;

    /* Take current digit into account when calculating width */
    width--;

    /* Recursively write upper digits */
    if ((value / 10) > 0) {

        num = PutUnsignedInt(pStr, fill, width, value / 10);
        pStr += num;
    }
    
    /* Write filler characters */
    else {

        while (width > 0) {

            PutChar(pStr, fill);
            pStr++;
            num++;
            width--;
        }
    }

    /* Write lower digit */
    num += PutChar(pStr, (value % 10) + '0');

    return num;
}


/**
 * @brief  Writes a signed int inside the given string, using the provided fill & width
 *         parameters.
 *
 * @param pStr   Storage string.
 * @param fill   Fill character.
 * @param width  Minimum integer width.
 * @param value  Signed integer value.
 */
signed int PutSignedInt(
    char *pStr,
    char fill,
    signed int width,
    signed int value)
{
    signed int num = 0;
    unsigned int absolute;

    /* Compute absolute value */
    if (value < 0) {

        absolute = -value;
    }
    else {

        absolute = value;
    }

    /* Take current digit into account when calculating width */
    width--;

    /* Recursively write upper digits */
    if ((absolute / 10) > 0) {

        if (value < 0) {
        
            num = PutSignedInt(pStr, fill, width, -(absolute / 10));
        }
        else {

            num = PutSignedInt(pStr, fill, width, absolute / 10);
        }
        pStr += num;
    }
    else {

        /* Reserve space for sign */
        if (value < 0) {

            width--;
        }

        /* Write filler characters */
        while (width > 0) {

            PutChar(pStr, fill);
            pStr++;
            num++;
            width--;
        }

        /* Write sign */
        if (value < 0) {

            num += PutChar(pStr, '-');
            pStr++;
        }
    }

    /* Write lower digit */
    num += PutChar(pStr, (absolute % 10) + '0');

    return num;
}


/**
 * @brief  Writes an hexadecimal value into a string, using the given fill, width &
 *         capital parameters.
 *
 * @param pStr   Storage string.
 * @param fill   Fill character.
 * @param width  Minimum integer width.
 * @param maj    Indicates if the letters must be printed in lower- or upper-case.
 * @param value  Hexadecimal value.
 *
 * @return  The number of char written
 */
signed int PutHexa(
    char *pStr,
    char fill,
    signed int width,
    unsigned char maj,
    unsigned int value)
{
    signed int num = 0;

    /* Decrement width */
    width--;

    /* Recursively output upper digits */
    if ((value >> 4) > 0) {

        num += PutHexa(pStr, fill, width, maj, value >> 4);
        pStr += num;
    }
    /* Write filler chars */
    else {

        while (width > 0) {

            PutChar(pStr, fill);
            pStr++;
            num++;
            width--;
        }
    }

    /* Write current digit */
    if ((value & 0xF) < 10) {

        PutChar(pStr, (value & 0xF) + '0');
    }
    else if (maj) {

        PutChar(pStr, (value & 0xF) - 10 + 'A');
    }
    else {

        PutChar(pStr, (value & 0xF) - 10 + 'a');
    }
    num++;

    return num;
}

/**
 * @brief  Writes an unsigned long int inside the given string,
 *  using the provided fill & width parameters.
 *
 * @param  pStr  Storage string.
 * @param  fill  Fill character.
 * @param  width  Minimum long integer width.
 * @param  value  long Integer value.
 */
signed int PutUnsignedLongInt(
    char *pStr,
    char fill,
    signed int width,
    unsigned long int value)
{
    signed int num = 0;

    /* Take current digit into account when calculating width */
    width--;

    /* Recursively write upper digits */
    if ((value / 10) > 0) {

        num = PutUnsignedInt(pStr, fill, width, value / 10);
        pStr += num;
    }

    /* Write filler characters */
    else {

        while (width > 0) {

            PutChar(pStr, fill);
            pStr++;
            num++;
            width--;
        }
    }

    /* Write lower digit */
    num += PutChar(pStr, (value % 10) + '0');

    return num;
}

/* Global Functions ----------------------------------------------------------- */


/**
 * @brief  Stores the result of a formatted string into another string. Format
 *         arguments are given in a va_list instance.
 *
 * @param pStr    Destination string.
 * @param length  Length of Destination string.
 * @param pFormat Format string.
 * @param ap      Argument list.
 *
 * @return  The number of characters written.
 */
unsigned int printBuffHead = 0;
signed int vsnprintf(char *pStr, size_t length, const char *pFormat, va_list ap)
{
	char          fill;
	unsigned char width;
	signed int    num = 0;
	signed int    size = 0;
	char format;

	/* Clear the string */
	if (pStr) {
		*pStr = 0;
	}
	printBuffHead += strlen(pFormat);
	/* Phase string */
	while (*pFormat != 0 && size < length) {
		/* Normal character */
		if (*pFormat != '%') {
			*pStr++ = *pFormat++;
			size++;
		}
		/* Escaped '%' */
		else if (*(pFormat+1) == '%') {

			*pStr++ = '%';
			pFormat += 2;
			size++;
		}
		/* Token delimiter */
		else {
			fill = ' ';
			width = 0;
			pFormat++;

			/* Parse filler */
			if (*pFormat == '0') {
				fill = '0';
				pFormat++;
			}

			/* Parse width */
			while ((*pFormat >= '0') && (*pFormat <= '9')) {
				width = (width*10) + *pFormat-'0';
				pFormat++;
			}

			/* Check if there is enough space */
			if (size + width > length) {
				width = length - size;
			}

			if ( (*pFormat == 'l') && (*pFormat == 'l') ) {
				format = 'U';
				pFormat += 2;
			} else {
				format = *pFormat;
				pFormat++;
			}

			/* Parse type */
			switch (format) {
				case 'd':
				case 'i': num = PutSignedInt(pStr, fill, width,
						va_arg(ap, signed int)); break;
				case 'u':
					num = PutUnsignedInt(pStr, fill, width,
						va_arg(ap, unsigned int));
					break;
				case 'U':
					num = PutUnsignedLongInt(pStr,
						fill, width,
						va_arg(ap, unsigned long int));
				break;
				case 'x':
					num = PutHexa(pStr, fill, width, 0,
						va_arg(ap, unsigned int));
					break;
				case 'X':
					num = PutHexa(pStr, fill, width, 1,
						va_arg(ap, unsigned int));
					break;
				case 's':
					num = PutString(pStr,
						va_arg(ap, char *));
					break;
				case 'c':
					num = PutChar(pStr,
						va_arg(ap, unsigned int));
					break;
				default:
				return EOF;
			}

			pStr += num;
			size += num;
		}
	}

    /* NULL-terminated (final \0 is not counted) */
    if (size < length) {

        *pStr = 0;
    }
    else {

        *(--pStr) = 0;
        size--;
    }

    return size;
}


/**
 * @brief  Stores the result of a formatted string into another string. Format
 *         arguments are given in a va_list instance.
 *
 * @param pStr    Destination string.
 * @param length  Length of Destination string.
 * @param pFormat Format string.
 * @param ...     Other arguments
 *
 * @return  The number of characters written.
 */
signed int snprintf(char *pString, size_t length, const char *pFormat, ...)
{
    va_list    ap;
    signed int rc;

    va_start(ap, pFormat);
    rc = vsnprintf(pString, length, pFormat, ap);
    va_end(ap);

    return rc;
}


/**
 * @brief  Stores the result of a formatted string into another string. Format
 *         arguments are given in a va_list instance.
 *
 * @param pString  Destination string.
 * @param length   Length of Destination string.
 * @param pFormat  Format string.
 * @param ap       Argument list.
 *
 * @return  The number of characters written.
 */
signed int vsprintf(char *pString, const char *pFormat, va_list ap)
{
   return vsnprintf(pString, MAX_STRING_SIZE, pFormat, ap);
}

/**
 * @brief  Outputs a formatted string on the given stream. Format arguments are given
 *         in a va_list instance.
 *
 * @param pStream  Output stream.
 * @param pFormat  Format string
 * @param ap       Argument list. 
 */
signed int vfprintf(FILE *pStream, const char *pFormat, va_list ap)
{
    char pStr[MAX_STRING_SIZE];
    unsigned char pError[] = "stdio.c: increase MAX_STRING_SIZE\n\r";

    /* Write formatted string in buffer */
    if (vsprintf(pStr, pFormat, ap) >= MAX_STRING_SIZE) {

        //fputs(pError, stderr);
    	Vcom_Data_Return_Proc((uint8_t *)pError, sizeof(pError));
        while (1); /* Increase MAX_STRING_SIZE */
    }

    /* Display string */
    //return fputs(pStr, pStream);
    Vcom_Data_Return_Proc((uint8_t *)pStr, sizeof(pStr));

    return sizeof(pStr);
}


/**
 * @brief  Outputs a formatted string on the DBGU stream. Format arguments are given
 *         in a va_list instance.
 *
 * @param pFormat  Format string.
 * @param ap  Argument list.
 */
signed int vprintf(const char *pFormat, va_list ap)
{
    return vfprintf(stdout, pFormat, ap);
}


/**
 * @brief  Outputs a formatted string on the given stream, using a variable 
 *         number of arguments.
 *
 * @param pStream  Output stream.
 * @param pFormat  Format string.
 */
signed int fprintf(FILE *pStream, const char *pFormat, ...)
{
    va_list ap;
    signed int result;

    /* Forward call to vfprintf */
    va_start(ap, pFormat);
    result = vfprintf(pStream, pFormat, ap);
    va_end(ap);

    return result;
}


/**
 * @brief  Outputs a formatted string on the DBGU stream, using a variable number of
 *         arguments.
 *
 * @param  pFormat  Format string.
 */
signed int printf(const char *pFormat, ...)
{
    va_list ap;
    signed int result;
#if defined(SII_DEBUG_UART1)
    unsigned int i;
    char tmp = '\r';
#endif
    char pStr[MAX_STRING_SIZE] = {0};
#if defined(SII_DEBUG_USB_VCOM)
    uint32_t strlength = 0;
#endif
    /* Forward call to vprintf */
    va_start(ap, pFormat);
    result = vsprintf(pStr, pFormat, ap);
    va_end(ap);

    /* Adding Time Stamp */

#if defined(SII_DEBUG_UART1)
	for(i = 0;i < strlen(pStr); i++)
    {
    	UART_WRITE(UART1, pStr[i]);
    	if(pStr[i] == '\n')
    		UART_WRITE(UART1, tmp);
    }
#elif defined(SII_DEBUG_USB_VCOM)
	strlength = strlen(pStr);
	/*if(pStr[strlength-1] == '\n') {
		pStr[strlength] = '\r';
		strlength++;
	}*/
    Vcom_Data_Return_Proc((uint8_t *)pStr, strlength);
#endif
    return result;
}

/**
 * @brief  Outputs a formatted string on the DBGU stream, using a variable number of
 *         arguments.
 *
 * @param  pFormat  Format string.
 */
signed int get_time(char *pTsOut, const char *pFormat, ...)
{
    va_list ap;
    signed int result;

    /* Forward call to vprintf */
    va_start(ap, pFormat);
    result = vsprintf(pTsOut, pFormat, ap);
    va_end(ap);

    return result;
}
/**
 * @brief  Outputs a formatted string on the DBGU stream, using a variable number of
 *         arguments with time stamp.
 *
 * @param  pFormat  Format string.
 */
signed int tsprintf(const char *pFormat, ...)
{
    va_list ap;
    signed int result, tResult;
    /* U64 get_CoGetOSTime; */
    char pTime[30] = {0};
    uint32_t volatile btime;

    char pStr[MAX_STRING_SIZE] = {0};
#if defined(SII_DEBUG_UART1)
    signed int i;
    char tmp = '\r';
#endif
    /*get_CoGetOSTime = CoGetOSTime();
      tResult = get_time(pTime, "[%d.%d-    ]  ",
        		get_CoGetOSTime/1000,
        		get_CoGetOSTime%1000);
    */

    btime = LibTimerGetTick(  );
    tResult = get_time(pTime, "[ %d.%03d sec ] ",
    		btime/1000,
    		btime%1000);

    memcpy(pStr, pTime, tResult);

    /* Forward call to vprintf */
    va_start(ap, pFormat);
    result = vsprintf(&pStr[tResult], pFormat, ap);
    va_end(ap);

    pStr[tResult+result] = '\0';

    /* Adding Time Stamp */

#if defined(SII_DEBUG_UART1)
	for(i = 0;i < strlen(pStr); i++)
    {
    	UART_WRITE(UART1, pStr[i]);
    	if(pStr[i] == '\n')
    		UART_WRITE(UART1, tmp);
    }
#elif defined(SII_DEBUG_USB_VCOM)
    Vcom_Data_Return_Proc((uint8_t *) pStr, strlen(pStr));
#endif
    return result;
}

#if 0
static void sVcom_print(uint8_t *pu8Message, uint32_t u32MessageLen)
{
    for( i = 0; i < u8Count; i++ )
     {
         s_Vcom_DataIn_Func( &pStr[pr_index], MESSAGE_UNIT_LEN );
         pr_index += MESSAGE_UNIT_LEN;
     }

     if( u8Rest != 0 )
     {
         s_Vcom_DataIn_Func( &pStr[pr_index], u8Rest );
     }

     /* Need to trigger for the Last return message */
     //s_Vcom_DataIn_Func( pu8Message, 0 );

     return;
}
#endif

/**
 * @brief  Writes a formatted string inside another string.
 *
 * @param pStr     torage string.
 * @param pFormat  Format string.
 */
signed int sprintf(char *pStr, const char *pFormat, ...)
{
    va_list ap;
    signed int result;

    // Forward call to vsprintf
    va_start(ap, pFormat);
    result = vsprintf(pStr, pFormat, ap);
    va_end(ap);

    return result;
}


/**
 * @brief  Outputs a string on stdout.
 *
 * @param pStr  String to output. 
 */
signed int puts(const char *pStr)
{
    return (fputs(pStr, stdout) && fputs("\n\r", stdout));
}


/**
 * @brief  Implementation of fputc using the DBGU as the standard output. Required
 *         for printf().
 *
 * @param c        Character to write.
 * @param pStream  Output stream.
 * @param The character written if successful, or -1 if the output stream is
 *        not stdout or stderr.
 */
signed int fputc(signed int c, FILE *pStream)
{
    if ((pStream == stdout) || (pStream == stderr)) {

    	PrintChar(c);

        return c;
    }
    else {

        return EOF;
    }
}


/**
 * @brief  Implementation of fputs using the DBGU as the standard output. Required
 *         for printf().
 *
 * @param pStr     String to write.
 * @param pStream  Output stream.
 *
 * @return  Number of characters written if successful, or -1 if the output
 *          stream is not stdout or stderr.
 */
signed int fputs(const char *pStr, FILE *pStream)
{
    signed int num = 0;

    while (*pStr != 0) {

        if (fputc(*pStr, pStream) == -1) {

            return -1;
        }
        num++;
        pStr++;
    }

    //Vcom_Data_Return_Proc(pStr, sizeof(pStr));

    return num;
}

/* --------------------------------- End Of File ------------------------------ */

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
/**
 * @file si_sys_obj.c
 *
 * @brief Object creater
 *
 *****************************************************************************/

/***** #include statements ***************************************************/
#define SII_DEBUG 3

#include "sii_system_api.h"
#include "sii_sys_log_api.h"
#include "sii_sys_obj_api.h"
#include "sii_sys_time_api.h"
#include "sii_platform_api.h"
#include "sii_lib_pipe_api.h"

/***** Register Module name **************************************************/

/*SII_MODULE_NAME_SET(LibLog);*/

/***** local macro definitions ***********************************************/

#define LOG_LINE_WRAP		(160)
#define LOG_LENGTH_LHS_MAX	(12)
#define LOG_LENGTH_CLASS_NAME	(LOG_LENGTH_LHS_MAX)
#define LOG_LENGTH_OBJECT_NAME	(LOG_LENGTH_LHS_MAX)

#define STR_LEN		160
/***** local type definitions ************************************************/

#ifdef SII_LOG_FIFO
static sii_inst_t s_log_queue;
#endif

/***** local prototypes ******************************************************/

/*static void s_put_char(char character);*/
static uint8_t s_put_string(const char *log_str);
/*static uint8_t s_log_limited(uint8_t size, const char *log_str);*/
static void s_log_put_string(char *log_str);

/***** local data objects ****************************************************/

static uint16_t s_line_pos;

/***** public functions ******************************************************/

#ifdef SII_LOG_FIFO
void sii_sys_log_set_pipe(sii_inst_t log_queue)
{
	s_log_queue = log_queue;
}
#endif

void sii_sys_log_timestamp(const char *class_str, const char *func_str,
	void *p_obj)
{
	uint32_t sec, usec;
	uint16_t tot = 0;
	uint16_t ts_len = 0;
	char str[50];
	char t_len = 0;

	if (!func_str)
		return;

	sii_platform_log_time_query(&sec, &usec);

	/* Print time stamp */
	{
		/*s_put_char('\n');
		s_put_char('[');
		SII_SPRINTF(str, "%ld", milli_sec/1000);
		ts_len += s_put_string(str);
		s_put_char('.');
		SII_SPRINTF(str, "%03ld", milli_sec%1000);
		ts_len += s_put_string(str);
		s_put_char(']');
		s_put_char(' ');
		s_put_char('-');
		s_put_char(' ');
		ts_len += 7;*/

		//~ str[ts_len++] = '\r';
		str[ts_len++] = '\n';
		str[ts_len++] = '[';
		t_len = SII_SPRINTF(&str[ts_len], sizeof(str) - ts_len, "%5u",
				sec);
		ts_len += t_len;
		str[ts_len++] = '.';
		t_len = SII_SPRINTF(&str[ts_len], sizeof(str) - ts_len, "%06u",
				usec);
		ts_len += t_len;
		str[ts_len++] = ']';
		str[ts_len++] = ' ';
		//~ str[ts_len++] = '-';
		//~ str[ts_len++] = ' ';
	}

	/* Print module name */
	/*tot += s_log_limited(LOG_LENGTH_CLASS_NAME, class_str);*/
	{
		uint8_t i   = 0;
		const char *log_str = class_str;
		uint8_t len = (uint8_t)SII_STRLEN(log_str);

		/* does string length exceed size limitation? */
		if (len > LOG_LENGTH_CLASS_NAME) {
			/* truncate left of string */
			str[ts_len++] = '*';
			tot++;
			log_str += (len - LOG_LENGTH_CLASS_NAME + 1);
			len = LOG_LENGTH_CLASS_NAME - 1;
		}

		for (i = 0; i < len; i++)
			str[ts_len++] = log_str[i];
		tot += len;
	}

	/* If instance print instance name */
	if (p_obj) {
		/* Separation character */
		str[ts_len++] = '.';
		tot++;

		/*tot += s_log_limited(LOG_LENGTH_OBJECT_NAME,
			sii_sys_obj_name_get(p_obj));*/
		{
			uint8_t i   = 0;
			const char *log_str = sii_sys_obj_name_get(p_obj);
			uint8_t len = (uint8_t)SII_STRLEN(log_str);

			/* does string length exceed size limitation? */
			if (len > LOG_LENGTH_OBJECT_NAME) {
				/* truncate left of string */
				str[ts_len++] = '*';
				tot++;
				log_str += (len - LOG_LENGTH_OBJECT_NAME + 1);
				len = LOG_LENGTH_OBJECT_NAME - 1;
			}

			for (i = 0; i < len; i++)
				str[ts_len++] = log_str[i];
			tot += len;
		}
	}

	if (*func_str) {
		/* Separation character */
		str[ts_len++] = '.';
		tot++;

		/*tot += s_log_limited((uint8_t)(LOG_LENGTH_LHS_MAX - tot),
			func_str);*/
		{
			uint8_t i   = 0;
			const char *log_str = sii_sys_obj_name_get(p_obj);
			uint8_t len = (uint8_t)SII_STRLEN(log_str);

			/* does string length exceed size limitation? */
			if (len > (LOG_LENGTH_LHS_MAX - tot)) {
				/* truncate left of string */
				str[ts_len++] = '*';
				tot++;
				log_str += (len -
						(LOG_LENGTH_LHS_MAX - tot) + 1);
				len = (LOG_LENGTH_LHS_MAX - tot) - 1;
			}

			for (i = 0; i < len; i++)
				str[ts_len++] = log_str[i];
			tot += len;
		}
	}

	/* Print alignment space characters */
	tot = (LOG_LENGTH_LHS_MAX < tot) ? (0) : (LOG_LENGTH_LHS_MAX - tot);
	while (tot--)
		str[ts_len++] = ' ';

	/* Print end of preamble */
	/* - 1 to compensate for the newline counted with the timestamp */
	/*s_line_pos  = ts_len + LOG_LENGTH_LHS_MAX - 1;
	s_line_pos += s_put_string(": ");*/

	str[ts_len++] = ':';
	str[ts_len++] = ' ';
	str[ts_len] = '\0';
	s_line_pos = s_put_string(str);

}

void sii_sys_log_printf(const char *fmt_str, ...)
{
	va_list arg;
	char str[STR_LEN]; /* CEC_LOGGER requires 160 */

	va_start(arg, fmt_str);
	SII_VSPRINTF(str, STR_LEN, fmt_str, arg);
	va_end(arg);
	/* SII_ASSERT(STR_LEN > ((int)sizeof(str))); */

	s_log_put_string(str);
}

/***** local functions *******************************************************/

static uint8_t s_put_string(const char *log_str)
{
	uint8_t len = (uint8_t)SII_STRLEN(log_str);
#ifdef SII_LOG_QUEUE
	uint8_t null_value = 0;
	uint8_t i = 0;

	if (s_log_queue) {
		for (i = 0; i < len; i++) {
			char c = log_str[i];
			sii_lib_pipe_write(s_log_queue, &c);
		}
		sii_lib_pipe_write(s_log_queue, &null_value);
	}
#else
	sii_platform_log_print(log_str);
#endif

	return len;
}

static void s_log_put_string(char *log_str)
{
	uint8_t last_space = 0;
	uint8_t index = 0;

	while (log_str[index]) {
		if (s_line_pos == 0)
			s_line_pos = s_put_string("\n    ");

		if (log_str[index] == ' ')
			last_space = index;

		if (log_str[index] == '\n') {
			log_str[index] = 0x00;
			s_put_string(log_str);
			s_line_pos = 0;

			log_str += (index + 1);
			last_space = index = 0;
		} else  {
			index++;
		}

		if ((s_line_pos + index) >= LOG_LINE_WRAP) {
			if (last_space != 0) {
				log_str[last_space] = 0x00;
				s_put_string(log_str);
				log_str += (last_space + 1);
			}

			s_line_pos = s_put_string("\n    ");
			last_space = index = 0;
		}
	}

	if (index != 0)
		s_line_pos += s_put_string(log_str);
}

/***** end of file ***********************************************************/

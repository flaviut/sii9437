/******************************************************************************
 *
 * 2016 (c) Lattice Semiconductor Corporation
 * 2018 (c) Sonos Inc
 *
 * This program is free software; you can redistribute it and/or
 * modify it only under the terms of version 2 of the GNU General Public License
 * as published by the Free Software Foundation, and not any later version.
 *
 * This program is distributed *AS IS* WITHOUT ANY WARRANTY of any kind,
 * whether express or implied, including without limitation the implied warranty
 * of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE or NON-INFRINGEMENT.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 *****************************************************************************/


#include "sii_platform_linux.h"
#include "sii_platform_api.h"

#include <linux/printk.h>
#include <linux/jiffies.h>
#include <linux/i2c.h>
#include <linux/mutex.h>
#include <linux/string.h>
#include <linux/sched.h>


#define LOG_SIZE	(32*PAGE_SIZE)
static char platform_log[LOG_SIZE];
static unsigned int platform_log_end = 0;
static unsigned int platform_log_size = 0;
static DEFINE_MUTEX(platform_log_lock);

void sii_seq_show_platform_log(struct seq_file *m)
{
	int show1_start, show1_len, show2_len, ignore_len;

	mutex_lock(&platform_log_lock);

	if (platform_log_size == LOG_SIZE) {
		show1_start = platform_log_end;
		show1_len = LOG_SIZE - platform_log_end;
		show2_len = platform_log_end;
	}
	else {
		show1_start = 0;
		show1_len = platform_log_size;
		show2_len = 0;
	}

	for (ignore_len = 0; platform_log[show1_start + ignore_len] != '\n'; ignore_len++) {
		if (ignore_len > show1_len) {
			ignore_len = -1;
			break;
		}
	}
	ignore_len++;
	show1_start += ignore_len;
	show1_len -= ignore_len;

	seq_write(m, &(platform_log[show1_start]), show1_len);
	seq_write(m, &(platform_log[0]), show2_len);
	seq_printf(m, "\n");

	mutex_unlock(&platform_log_lock);
}

void sii_platform_log_print(const char *msg_str)
{
	int msg_len, copy1, copy2;

	msg_len = strlen(msg_str);

	mutex_lock(&platform_log_lock);

	if ((platform_log_end + msg_len) > LOG_SIZE) {
		copy1 = LOG_SIZE - platform_log_end;
		copy2 = msg_len - copy1;
	}
	else {
		copy1 = msg_len;
		copy2 = 0;
	}

	memcpy(&(platform_log[platform_log_end]), msg_str, copy1);
	memcpy(&(platform_log[0]), &(msg_str[copy1]), copy2);

	platform_log_size += msg_len;
	if (platform_log_size > LOG_SIZE) platform_log_size = LOG_SIZE;
	platform_log_end = (platform_log_end + msg_len) % LOG_SIZE;

	mutex_unlock(&platform_log_lock);
}


static struct regmap *sii9437_inst1_i2c_page0 = NULL;
static struct regmap *sii9437_inst1_i2c_page1 = NULL;
static struct regmap *sii9437_inst1_i2c_page2 = NULL;

void sii_register_regmap_pages(
	struct regmap *page0,
	struct regmap *page1,
	struct regmap *page2)
{
	sii9437_inst1_i2c_page0 = page0;
	sii9437_inst1_i2c_page1 = page1;
	sii9437_inst1_i2c_page2 = page2;
}

void sii_deregister_regmap_pages(void)
{
	sii9437_inst1_i2c_page0 = NULL;
	sii9437_inst1_i2c_page1 = NULL;
	sii9437_inst1_i2c_page2 = NULL;
}

static int _get_i2c_page_cmd(
	uint32_t dev_id,
	uint16_t value,
	struct regmap **page,
	uint8_t *cmd)
{
	if (dev_id != SII9437_INST_1) {
		return FAIL;
	}

	switch ((value >> 8) & 0xff) {
		case PAGE_0:
			*page = sii9437_inst1_i2c_page0;
			break;
		case PAGE_1:
			*page = sii9437_inst1_i2c_page1;
			break;
		case PAGE_2:
			*page = sii9437_inst1_i2c_page2;
			break;
		default:
			return FAIL;
	}

	*cmd = value & 0xff;

	return SUCCESS;
}

uint32_t sii_platform_host_block_read(
	uint32_t dev_id,
	uint16_t address,
	uint8_t *p_data_in,
	uint16_t length)
{
	struct regmap *i2c_page = 0;
	uint8_t i2c_cmd = 0;
	s32 ret;

	if (_get_i2c_page_cmd(dev_id, address, &i2c_page, &i2c_cmd)) {
		return FAIL;
	}

	ret = regmap_bulk_read(i2c_page, i2c_cmd, p_data_in, length);
	if (ret < 0) {
		return FAIL;
	}
	return SUCCESS;
}

uint32_t sii_platform_host_block_write(
	uint32_t dev_id,
	uint16_t address,
	const uint8_t *p_data_out,
	uint16_t length)
{
	struct regmap *i2c_page = 0;
	uint8_t i2c_cmd = 0;
	s32 ret;

	if (_get_i2c_page_cmd(dev_id, address, &i2c_page, &i2c_cmd)) {
		return FAIL;
	}

	ret = regmap_bulk_write(i2c_page, i2c_cmd, p_data_out, length);
	if (ret < 0) {
		return FAIL;
	}
	return SUCCESS;
}


uint32_t sii_platform_time_msec_query(void)
{
	return jiffies_to_msecs(jiffies - INITIAL_JIFFIES);
}

void sii_platform_log_time_query(uint32_t *sec, uint32_t *usec)
{
	u64 nsec = local_clock();
	*usec = do_div(nsec, 1000000000) / 1000;
	*sec = nsec;
}

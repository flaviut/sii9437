/******************************************************************************
 *
 * 2018 (c) Sonos, Inc
 *
 * This program is free software; you can redistribute it and/or
 * modify it only under the terms of version 2 of the GNU General Public License
 * as published by the Free Software Foundation, and not any later version.
 *
 * This program is distributed AS IS WITHOUT ANY WARRANTY of any kind,
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

#ifndef SII_PLATFORM_LINUX_H
#define SII_PLATFORM_LINUX_H

#include <linux/regmap.h>
#include <linux/seq_file.h>

extern void sii_register_regmap_pages(
	struct regmap *page0,
	struct regmap *page1,
	struct regmap *page2);

extern void sii_deregister_regmap_pages(void);

extern void sii_seq_show_platform_log(struct seq_file *m);

#endif

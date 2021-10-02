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
* si_drv_audmux.c
*************************************************************************/
#include "sii_datatypes.h"
#include "sii_system_api.h"
#include "sii9612_drv_tpg_api.h"
#include "sii9612_drv_api.h"
#include "sii_api_wrap.h"
//#include "siiplatform_api.h"
//#include "si_sii9436_api.h"

/***** local macro definitions ***********************************************/
/***** Register Module name **************************************************/
/***** local static variable ****************************************************/
/***** public variables definitions **********************************************/
#define TPG_COLOR_WHITE				0x01cbb8
#define TPG_COLOR_YELLOW			0x025dc1
#define TPG_COLOR_CYAN				0x02ee0a
#define TPG_COLOR_GREEN				0x037053
#define TPG_COLOR_MAGENTA			0x03829c
#define TPG_COLOR_RED				0x0014e5
#define TPG_COLOR_BLUE				0x00a72e
#define TPG_COLOR_BLACK				0x013977


/***** local functions definitions ************************************************/
void sTpgWriteColor( uint16_t addr, uint32_t color )
{
	uint16_t i;
	for (i=0; i < 160; i ++)
	{
		SiiDrvCraWrite24(SII_9612_I2C_ADDRESS_TX, addr, color);
	}
}

void sProgramLUT (void)
{
		uint16_t i;
	//set up tpg (color bar)
	//===== set up y clut =====
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe88,0x00);		//reset address
	//y clut group 0
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe92,0xeb0);		//y value for white
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe92,0xd20);		//y value for yellow
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe92,0xa98);		//y value for cyan
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe92,0x908);		//y value for green
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe92,0x6a7);		//y value for magenta
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe92,0x517);		//y value for red
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe92,0x28f);		//y value for blue
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe92,0x100);		//y value for black

	//y clut group 1
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe92,0x100);		//y value for black
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe92,0x28f);		//y value for blue
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe92,0x517);		//y value for red
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe92,0x6a7);		//y value for magenta
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe92,0x908);		//y value for green
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe92,0xa98);		//y value for cyan
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe92,0xd20);		//y value for yellow
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe92,0xeb0);		//y value for white

	//===== set up cb clut =====
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe88,0x00);		//reset address
	//cb clut group 0
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe94,0x800);		//cb value for white
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe94,0x100);		//cb value for yellow
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe94,0xa5d);		//cb value for cyan
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe94,0x35e);		//cb value for green
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe94,0xca2);		//cb value for magenta
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe94,0x5a3);		//cb value for red
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe94,0xf00);		//cb value for blue
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe94,0x800);		//cb value for black

	//y clut group 1
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe94,0x800);		//cb value for black
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe94,0xf00);		//cb value for blue
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe94,0x5a3);		//cb value for red
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe94,0xca2);		//cb value for magenta
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe94,0x35e);		//cb value for green
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe94,0xa5d);		//cb value for cyan
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe94,0x100);		//cb value for yellow
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe94,0x800);		//cb value for white

	//===== set up cr clut =====
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe88,0x00);		//reset address		
	//cr clut group 0
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe96,0x800);		//cr value for white
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe96,0x922);		//cr value for yellow
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe96,0x100);		//cr value for cyan
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe96,0x223);		//cr value for green
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe96,0xddd);		//cr value for magenta
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe96,0xf00);		//cr value for red
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe96,0x6de);		//cr value for blue
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe96,0x800);		//cr value for black

	//cr clut group 1
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe96,0x800);		//cr value for black
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe96,0x6de);		//cr value for blue
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe96,0xf00);		//cr value for red
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe96,0xddd);		//cr value for magenta
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe96,0x223);		//cr value for green
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe96,0x100);		//cr value for cyan
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe96,0x922);		//cr value for yellow
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe96,0x800);		//cr value for white
	
	//===== set up line gen memory =====
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe88,0x00);		//reset address
	
	sTpgWriteColor(0xe8c, 0x01cbb8);		//bg:white
	sTpgWriteColor(0xe8c, 0x025dc1);		//bg:yellow
	sTpgWriteColor(0xe8c, 0x02ee0a);		//bg:cyan
	sTpgWriteColor(0xe8c, 0x037053);		//bg:green
	sTpgWriteColor(0xe8c, 0x03829c);		//bg:magenta
	sTpgWriteColor(0xe8c, 0x0014e5);		//bg:red
	sTpgWriteColor(0xe8c, 0x00a72e);		//bg:blue
	sTpgWriteColor(0xe8c, 0x013977);		//bg:black
	
	//===== set up line map memory =====
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0xe88,0x00);		//reset address
	for (i=0; i < 720; i++) SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0xe90,0x00);

	//===== enable tpg =====
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0xe80,0x01);		//enable tpg	
}
void sSetUpPllVtg (void)
{
	//pll0 settings
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x008a, 0x00);	// select sysclk as source for pll0 input clock
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x008c, 0x01);	// pll0 post divider od1-1
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x00ad, 0x00);	// pll0 ref divider nr-1	
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0x00ae, 0x002b);	// pll0 multiply factor nf-1
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x00b0, 0x07);	// pll0 post vco divider od-1	
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0x00b2, 0x002b);	// pll0 bandwidth adjust nb-1
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x00ac, 0x60);	// pll0 enable
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x00b4, 0x01);	// pll0	update request

	//vtg settings
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0x0862, 0x0027);	// vtg end of hs pulse-1	
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0x0864, 0x0103);	// vtg horizontal active line start-1
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0x0866, 0x0337);	// vtg half line-2	
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0x086c, 0x0603);	// vtg horizontal active line end-1
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0x086e, 0x0670);	// vtg end of horizontal line-2
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0x0870, 0x0009);	// vtg end of vs pulse (in half lines)-1
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0x0872, 0x0017);	// vtg sc trigger point (in half lines)-1
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0x0874, 0x0031);	// vtg vertical active start (in half lines)-1
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0x0876, 0x05d1);	// vtg vertical active end (in half lines)-1
	SiiDrvCraWrite16(SII_9612_I2C_ADDRESS_TX, 0x0878, 0x05db);	// vtg end of frame (in half lines)-1
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x089c, 0x01);	// vtg update request

	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x09fc, 0x10);	// force vtg unlock mode
}

void sInitTitan( void )
{
	//initialize tx
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x2600, 0xb1);	// initialize tx phy (roog)
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x2601, 0x38);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x2602, 0xa9);	// 8-bit color depth
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x2603, 0x00);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x260c, 0x37);
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x260d, 0x01);

	SiiDrvCraBitsMod8(SII_9612_I2C_ADDRESS_TX, 0x2608, 0x60, 0x40);

	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x20c7, 0x00);	// switch to h/w tpi mode

	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x2040, 0x01);	// b0=1: assert tx (ekki) software reset

	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x2009, 0x00);	// b7.6=2'b00: set input color depth to 8-bit				
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x200a, 0x00);	// b7.6=2'b00: set output color depth to 8-bit				

	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x201a, 0x01);	// enable hdmi mode

	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x2040, 0x00);	// b0=0: de-assert tx (ekki) software reset
					// b2=0: disable dc packet
}					

void sTpgColorBar ( void )
{
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x2606, 0x10);	// b4=1: enable osc	 

	//initialize processing core
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x040d, 0x01);	// enable bl bypass	
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x060d, 0x01);	// enable essd bypass
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x070d, 0x01);	// enable enhhd bypass
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x0a0d, 0x01);	// enable eshd bypass
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x0010, 0x00);	// b5.4=2'b00: select processed data for hdmi output
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x0014, 0x00);	// b1.0=2'b11: +ve hs & vs polarity for hdmi output
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x008e, 0x03);	// select pll0 output as clock source for processing path
	SiiDrvCraWrite8(SII_9612_I2C_ADDRESS_TX, 0x0090, 0x60);	// b4.2=3'b000: disable 2x divider for output clock
											// b7.5=3'b011: select video processing output clock as clock source for hdmi output

	sSetUpPllVtg();
	sProgramLUT();
	sInitTitan();
}
void SiiDrvPhalanxTpgCreate( void )
{ 
	sTpgColorBar();
}




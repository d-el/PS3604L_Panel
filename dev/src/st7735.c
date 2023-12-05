/*!****************************************************************************
 * @file		st7735.c
 * @author		d_el
 * @version		V1.0
 * @date		31.01.2018
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		Driver display on controller ST7735R
 */

/*!****************************************************************************
 * Include
 */
#include <stddef.h>
#include <gpio.h>
#include <spi.h>
#include "st7735.h"

/*!****************************************************************************
 * Enumeration
 */
enum lcdCommands{
	ST7735_NOP	   = 0x00,
	ST7735_SWRESET = 0x01,
	ST7735_RDDID   = 0x04,
	ST7735_RDDST   = 0x09,

	ST7735_SLPIN   = 0x10,
	ST7735_SLPOUT  = 0x11,
	ST7735_PTLON   = 0x12,
	ST7735_NORON   = 0x13,

	ST7735_INVOFF  = 0x20,
	ST7735_INVON   = 0x21,
	ST7735_DISPOFF = 0x28,
	ST7735_DISPON  = 0x29,
	ST7735_CASET   = 0x2A,
	ST7735_RASET   = 0x2B,
	ST7735_RAMWR   = 0x2C,
	ST7735_RAMRD   = 0x2E,

	ST7735_PTLAR   = 0x30,
	ST7735_COLMOD  = 0x3A,
	ST7735_MADCTL  = 0x36,

	ST7735_FRMCTR1 = 0xB1,
	ST7735_FRMCTR2 = 0xB2,
	ST7735_FRMCTR3 = 0xB3,
	ST7735_INVCTR  = 0xB4,
	ST7735_DISSET5 = 0xB6,

	ST7735_PWCTR1  = 0xC0,
	ST7735_PWCTR2  = 0xC1,
	ST7735_PWCTR3  = 0xC2,
	ST7735_PWCTR4  = 0xC3,
	ST7735_PWCTR5  = 0xC4,
	ST7735_VMCTR1  = 0xC5,

	ST7735_RDID1   = 0xDA,
	ST7735_RDID2   = 0xDB,
	ST7735_RDID3   = 0xDC,
	ST7735_RDID4   = 0xDD,

	ST7735_PWCTR6  = 0xFC,

	ST7735_GMCTRP1 = 0xE0,
	ST7735_GMCTRN1 = 0xE1
};

enum lcdDescribed{
	MADCTL_MY	= 0x80,
	MADCTL_MX	= 0x40,
	MADCTL_MV	= 0x20,
	MADCTL_ML	= 0x10,
	MADCTL_RGB	= 0x00,
	MADCTL_BGR	= 0x08,
	MADCTL_MH	= 0x04
};

/*!****************************************************************************
 * MEMORY
 */
uint16_t videoBff[ST7735_W * ST7735_H];
static uint8_t xstart, ystart, width, height;
static flushcb_type flushcb;
static setbufcb_type setbufcb;

typedef struct{
	uint8_t w;
	uint8_t h;
	uint8_t cstart;
	uint8_t rstart;
	uint8_t invert;
	uint8_t madctl;
}st7735cfg_type;

static const st7735cfg_type st7735cfg[] = {
	[JD_T18003]	= { .w = 128,	.h = 160,	.cstart = 0,	.rstart = 0, .invert = 0,	.madctl = MADCTL_RGB },
	[IPS16080]	= { .w = 80,	.h = 160,	.cstart = 26,	.rstart = 1, .invert = 1,	.madctl = MADCTL_BGR }
};

/// Initialization for 7735R
static const uint8_t rcmd[] = {
	19,	// commands in list

	ST7735_SWRESET, 0, 150,	 // 1: Software reset

	ST7735_SLPOUT, 0, 255,	// 2: Out of sleep mode

	ST7735_FRMCTR1, 3, 0,	// 3: Frame rate ctrl - normal mode
	0x01, 0x2C, 0x2D,		//	   Rate = fosc/(1x2+40) * (LINE+2C+2D)

	ST7735_FRMCTR2, 3, 0,	// 4: Frame rate control - idle mode
	0x01, 0x2C, 0x2D,		//	   Rate = fosc/(1x2+40) * (LINE+2C+2D)

	ST7735_FRMCTR3, 6, 0,	// 5: Frame rate ctrl - partial mode
	0x01, 0x2C, 0x2D,		//	   Dot inversion mode
	0x01, 0x2C, 0x2D,		//	   Line inversion mode

	ST7735_INVCTR, 1, 0,	// 6: Display inversion ctrl
	0x07,					//	   No inversion

	ST7735_PWCTR1, 3, 0,	// 7: Power control
	0xA2, 0x02,					  //	 -4.6V
	0x84,					//	   AUTO mode

	ST7735_PWCTR2, 1, 0,	// 8: Power control
	0xC5,					//	   VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD

	ST7735_PWCTR3, 2, 0,	// 9: Power control
	0x0A,					//	   Opamp current small
	0x00,					//	   Boost frequency

	ST7735_PWCTR4, 2, 0,	// 10: Power control
	0x8A,					//	   BCLK/2, Opamp current small & Medium low
	0x2A,

	ST7735_PWCTR5, 2, 0,	// 11: Power control
	0x8A, 0xEE,

	ST7735_VMCTR1, 1, 0,	// 12: Power control
	0x0E,

	ST7735_INVOFF, 0, 0,	// 13: Don't invert display

	ST7735_MADCTL, 1, 0,	// 14: Memory access control (directions)
	0xC8,					//	   row addr/col addr, bottom to top refresh

	ST7735_COLMOD, 1, 0,	// 15: set color mode
	0x05,					//	   16-bit color

	ST7735_GMCTRP1, 16, 0,	// 1: Magical unicorn dust,
	0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d, 0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10,

	ST7735_GMCTRN1, 16, 0,	// 2: Sparkles and rainbows
	0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D, 0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10,

	ST7735_NORON, 0, 10,	// 3: Normal display on

	ST7735_DISPON, 0, 100	// 4: Main screen turn on
};

/*!****************************************************************************
 * @brief Send command
 */
static void st7735_lcdCmd(uint8_t cmd){
	gppin_reset(GP_LCD_DC);
	spi_send(cmd);
}

/*!****************************************************************************
 * @brief Send data
 */
static void st7735_lcdDat(uint8_t data){
	gppin_set(GP_LCD_DC);
	spi_send(data);
}

/*!****************************************************************************
 * @brief
 */
static void lcdDelay(uint64_t delay){
	delay = delay * 10000;
	for(uint64_t i = 0; i < delay; i++){
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
	}
}

/*!****************************************************************************
 * @brief Set address window on Graphic RAM
 */
void st7735_setAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1){
	st7735_lcdCmd(ST7735_CASET);	// column addr set
	st7735_lcdDat(0x00);
	st7735_lcdDat(x0 + xstart);		// XSTART
	st7735_lcdDat(0x00);
	st7735_lcdDat(x1 + xstart);		// XEND

	st7735_lcdCmd(ST7735_RASET);	// row addr set
	st7735_lcdDat(0x00);
	st7735_lcdDat(y0 + ystart);		// YSTART
	st7735_lcdDat(0x00);
	st7735_lcdDat(y1 + ystart);		// YEND

	st7735_lcdCmd(ST7735_RAMWR);	// write to RAM
}

/*!****************************************************************************
 * @brief Companion code to the above tables.  Reads and issues
 * a series of LCD commands stored in PROGMEM byte array.
 */
static void commandList(const uint8_t *addr){
	uint8_t	 numCommands, numArgs;
	uint16_t ms;

	if(addr == NULL){
		return;
	}

	numCommands = *addr++;					// Number of commands to follow
	while(numCommands--){					// For each command...
		st7735_lcdCmd(*addr++);				// Read, issue command
		numArgs		= *addr++;				// Number of args to follow
		ms			= *addr++;				// Read post-command delay time (ms)
		while(numArgs--){					// For each argument...
			st7735_lcdDat(*addr++);			// Read, issue argument
		}

		if(ms){
			if(ms == 255){
				ms = 500;					// If 255, delay for 500 ms
			}
			lcdDelay(ms);
		}
	}
}

/*!****************************************************************************
 * @brief Initialization code common to both 'B' and 'R' type displays
 */
static void commonInit(void) {
	gppin_reset(GP_LCD_CS);
	gppin_set(GP_LCD_RST);
	lcdDelay(10);
	gppin_reset(GP_LCD_RST);
	lcdDelay(10);
	gppin_set(GP_LCD_RST);
	lcdDelay(10);
}

/*!****************************************************************************
 * @brief Set rotation display
 */
void setRotation(display_type t, displayRotation_type r){
	st7735_lcdCmd(ST7735_MADCTL);
	switch (r){
		case ST7735_R0:
			st7735_lcdDat(MADCTL_MX | MADCTL_MY | st7735cfg[t].madctl);
			xstart = st7735cfg[t].cstart;
			ystart = st7735cfg[t].rstart;
			width = st7735cfg[t].w;
			height = st7735cfg[t].h;
			break;
		case ST7735_R90:
			st7735_lcdDat(MADCTL_MY | MADCTL_MV | st7735cfg[t].madctl);
			xstart = st7735cfg[t].rstart;
			ystart = st7735cfg[t].cstart;
			width = st7735cfg[t].h;
			height = st7735cfg[t].w;
			break;
		case ST7735_R180:
			st7735_lcdDat(st7735cfg[t].madctl);
			xstart = st7735cfg[t].cstart;
			ystart = st7735cfg[t].rstart;
			width = st7735cfg[t].w;
			height = st7735cfg[t].h;
			break;
		case ST7735_R270:
			st7735_lcdDat(MADCTL_MX | MADCTL_MV | st7735cfg[t].madctl);
			xstart = st7735cfg[t].rstart;
			ystart = st7735cfg[t].cstart;
			width = st7735cfg[t].h;
			height = st7735cfg[t].w;
			break;
	}
}

/*!****************************************************************************
 * @brief Set invert display
 */
void invertDisplay(uint8_t i){
	if(i != 0){
		st7735_lcdCmd(ST7735_INVON);
	}else{
		st7735_lcdCmd(ST7735_INVOFF);
	}
}

/*!****************************************************************************
 * @brief Initialization for ST7735R screens (green or red tabs)
 */
void st7735_init(display_type type, displayRotation_type r){
	spi_init();
	commonInit();
	commandList(rcmd);
	setRotation(type, r);
	invertDisplay(st7735cfg[type].invert);
	st7735_setAddressWindow(0, 0, width - 1, height - 1);
	spi_initBlock();
}

/*!****************************************************************************
 * @brief Write block callback
 */
static void st7735_writeBlockCb(void){
	if(flushcb){
		flushcb(NULL);
	}
}

/*!****************************************************************************
 * @brief Flush video buffer
 */
void st7735_flush(flushcb_type cb){
	flushcb = cb;
	spi_writeBlock(videoBff, sizeof(videoBff)/2, st7735_writeBlockCb);
}

/*!****************************************************************************
 * @brief Fill block callback
 */
static void st7735_fillBlockCb(void){
	if(setbufcb){
		setbufcb(NULL);
	}
}

/*!****************************************************************************
 * @brief Fill video buffer
 */
void st7735_fillBuffer(const lcd_color_type *color, setbufcb_type cb){
	setbufcb = cb;
	spi_fillBlock(videoBff, sizeof(videoBff)/2, color, st7735_fillBlockCb);
}

/******************************** END OF FILE ********************************/

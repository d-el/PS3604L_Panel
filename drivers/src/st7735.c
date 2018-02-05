/*!****************************************************************************
 * @file		st7735.c
 * @author		d_el
 * @version		V1.0
 * @date		31.01.2018
 * @brief		Driver display on controller ST7735B, ST7735R
 * @copyright	Copyright (C) 2017 Storozhenko Roman
 *				All rights reserved
 *				This software may be modified and distributed under the terms
 *				of the BSD license.	 See the LICENSE file for details
 */

/*!****************************************************************************
 * Include
 */
#include "stm32f4xx.h"
#include "gpio.h"
#include "st7735.h"

/*!****************************************************************************
 * Define
 */
// for 1.44 and mini
#define ST7735_TFTWIDTH_128  128
// for mini
#define ST7735_TFTWIDTH_80   80
// for 1.44" display
#define ST7735_TFTHEIGHT_128 128
// for 1.8" and mini display
#define ST7735_TFTHEIGHT_160  160

/*!****************************************************************************
 * Enumeration
 */
enum lcdCommands{
	ST7735_NOP     = 0x00,
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
uint8_t colstart, rowstart, xstart, ystart; // some displays need this changed
uint8_t height, width;
uint8_t tabcolor;

/*!
 * Rather than a bazillion writecommand() and writedata() calls, screen
 * initialization commands and arguments are organized in these tables
 * stored in PROGMEM.  The table may look bulky, but that's mostly the
 * formatting -- storage-wise this is hundreds of bytes more compact
 * than the equivalent code.  Companion function follows.
 */
/// Initialization commands for 7735B screens
const uint8_t Bcmd[] = {
	18,		// 18 commands in list

	ST7735_SWRESET,	0, 50,	// 1: Software reset

	ST7735_SLPOUT, 0, 255, 	// 2: Out of sleep mode

	ST7735_COLMOD, 1, 10, 	// 3: Set color mode
	0x05,					//		16-bit color

	ST7735_FRMCTR1, 3, 10, 	// 4: Frame rate control
	0x00,               	//     	fastest refresh
	0x06,               	//     	6 lines front porch
	0x03,               	//     	3 lines back porch

	ST7735_MADCTL, 1, 0,	// 5: Memory access ctrl (directions)
	0x08,               	//		Row addr/col addr, bottom to top refresh

	ST7735_DISSET5, 2, 0, 	// 6: Display settings #5
	0x15,               	//     	1 clk cycle nonoverlap, 2 cycle gate
							//     	rise, 3 cycle osc equalize
	0x02,					//     	Fix on VTL

	ST7735_INVCTR, 1, 0, 	// 7: Display inversion control
	0x0,                	//     	Line inversion

	ST7735_PWCTR1, 2, 10, 	// 8: Power control
	0x02,               	//     	GVDD = 4.7V
	0x70,               	//     	1.0uA

	ST7735_PWCTR2, 1, 0, 	//  9: Power control
	0x05,               	//     	VGH = 14.7V, VGL = -7.35V

	ST7735_PWCTR3, 2, 0, 	// 10: Power control
	0x01,               	//     	Opamp current small
	0x02,               	//     	Boost frequency

	ST7735_VMCTR1, 2, 10, 	// 11: Power control
	0x3C,               	//     	VCOMH = 4V
	0x38,               	//     	VCOML = -1.1V

	ST7735_PWCTR6, 2, 0, 	// 12: Power control,
	0x11,
	0x15,

	ST7735_GMCTRP1, 16, 0, 	// 13: Magical unicorn dust
	0x09, 0x16, 0x09, 0x20, 	// (seriously though, not sure what
	0x21, 0x1B, 0x13, 0x19, 	// these config values represent)
	0x17, 0x15, 0x1E, 0x2B, 0x04, 0x05, 0x02, 0x0E,

	ST7735_GMCTRN1, 16, 10, // 14: Sparkles and rainbows
	0x0B, 0x14, 0x08, 0x1E, // (ditto)
	0x22, 0x1D, 0x18, 0x1E, 0x1B, 0x1A, 0x24, 0x2B, 0x06, 0x06, 0x02, 0x0F,

	ST7735_CASET, 4, 0, 	// 15: Column addr set
	0x00, 0x02,				// 		XSTART = 2
	0x00, 0x81,				//     	XEND = 129

	ST7735_RASET, 4, 0,  	// 16: Row addr set, 4 args, no delay:
	0x00, 0x02,             //     XSTART = 1
	0x00, 0x81,             //     XEND = 160

	ST7735_NORON, 0, 10, 	// 17: Normal display on

	ST7735_DISPON, 0, 255	// 18: Main screen turn on
};

/// Initialization for 7735R, part 1 (red or green tab)
const uint8_t Rcmd1[] = {
	15,	// 15 commands in list

	ST7735_SWRESET, 0, 150,  // 1: Software reset

	ST7735_SLPOUT, 0, 255,  // 2: Out of sleep mode

	ST7735_FRMCTR1, 3, 0,  	// 3: Frame rate ctrl - normal mode
	0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)

	ST7735_FRMCTR2, 3, 0,  	// 4: Frame rate control - idle mode
	0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)

	ST7735_FRMCTR3, 6, 0,   // 5: Frame rate ctrl - partial mode
	0x01, 0x2C, 0x2D,       //     Dot inversion mode
	0x01, 0x2C, 0x2D,       //     Line inversion mode

	ST7735_INVCTR, 1, 0,  	// 6: Display inversion ctrl
	0x07,                   //     No inversion

	ST7735_PWCTR1, 3, 0,  	// 7: Power control
	0xA2, 0x02,                   //     -4.6V
	0x84,                   //     AUTO mode

	ST7735_PWCTR2, 1, 0,  	// 8: Power control
	0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD

	ST7735_PWCTR3, 2, 0,  	// 9: Power control
	0x0A,                   //     Opamp current small
	0x00,                   //     Boost frequency

	ST7735_PWCTR4, 2, 0,  	// 10: Power control
	0x8A,                   //     BCLK/2, Opamp current small & Medium low
	0x2A,

	ST7735_PWCTR5, 2, 0,  	// 11: Power control
	0x8A, 0xEE,

	ST7735_VMCTR1, 1, 0,  	// 12: Power control
	0x0E,

	ST7735_INVOFF, 0, 0,  	// 13: Don't invert display

	ST7735_MADCTL, 1, 0,  	// 14: Memory access control (directions)
	0xC8,                   //     row addr/col addr, bottom to top refresh

	ST7735_COLMOD, 1, 0,  	// 15: set color mode
	0x05                  	//     16-bit color
};

/// Initialization for 7735R, part 2 (green tab only)
const uint8_t Rcmd2green[] = {
	2,	//  2 commands in list

	ST7735_CASET, 4, 0,  	// 1: Column addr set
	0x00, 0x02,             //     XSTART = 0
	0x00, 0x7F + 0x02,    	//     XEND = 127

	ST7735_RASET, 4, 0,  	// 2: Row addr set
	0x00, 0x01,             //     XSTART = 0
	0x00, 0x9F + 0x01       //     XEND = 159
};

/// Initialization for 7735R, part 2 (red tab only)
const uint8_t Rcmd2red[] = {
	2, 	//  2 commands in list

	ST7735_CASET, 4, 0,  	// 1: Column addr set
	0x00, 0x00,             //     XSTART = 0
	0x00, 0x7F,             //     XEND = 127

	ST7735_RASET, 4, 0,  	// 2: Row addr set
	0x00, 0x00,             //     XSTART = 0
	0x00, 0x9F            	//     XEND = 159
};

/// Initialization for 7735R, part 2 (green 1.44 tab)
const uint8_t Rcmd2green144[] = {
	2,	//  2 commands in list

	ST7735_CASET, 4, 0,  	// 1: Column addr set
	0x00, 0x00,             //     XSTART = 0
	0x00, 0x7F,             //     XEND = 127

	ST7735_RASET, 4, 0,  	// 2: Row addr set
	0x00, 0x00,             //     XSTART = 0
	0x00, 0x7F            	//     XEND = 127
};

/// Initialization for 7735R, part 2 (mini 160x80)
const uint8_t Rcmd2green160x80[] = {
	2,	//  2 commands in list

	ST7735_CASET, 4, 0,  	// 1: Column addr set
	0x00, 0x00,             //     XSTART = 0
	0x00, 0x7F,             //     XEND = 79

	ST7735_RASET, 4, 0, 	// 2: Row addr set
	0x00, 0x00,             //     XSTART = 0
	0x00, 0x9F            	//     XEND = 159
};

/// Initialization for 7735R, part 3 (red or green tab)
const uint8_t Rcmd3[] = {
	4,	//  4 commands in list

	ST7735_GMCTRP1, 16, 0,	// 1: Magical unicorn dust,
	0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d, 0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10,

	ST7735_GMCTRN1, 16, 0,	// 2: Sparkles and rainbows
	0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D, 0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10,

	ST7735_NORON, 0, 10, 	// 3: Normal display on

	ST7735_DISPON, 0, 100, 	// 4: Main screen turn on
};

/*!****************************************************************************
 * @brief SPI initialization
 */
static void spiInit(void){
	//Max speed - fPCLK/2
	RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;		//Clock enable
	RCC->APB1RSTR |= RCC_APB1RSTR_SPI3RST;	//Reset module
	RCC->APB1RSTR &= ~RCC_APB1RSTR_SPI3RST;

	//LCD_SPI->CR1 |= SPI_CR1_BR_1;
	LCD_SPI->CR1 |= SPI_CR1_MSTR;				//Master configuration
	LCD_SPI->CR1 |= SPI_CR1_SSM;				//Software slave management enabled
	LCD_SPI->CR1 |= SPI_CR1_SSI;				//Internal slave select
	LCD_SPI->CR1 &= ~SPI_CR1_DFF;				//8-bit data frame format is selected for transmission/reception
	LCD_SPI->CR1 &= ~SPI_CR1_LSBFIRST;			//MSB transmitted first

	LCD_SPI->CR1 |= SPI_CR1_SPE;				//SPI enable

	gppin_init(GPIOB, 3, alternateFunctionPushPull, pullDisable, 0, 6);	//SPI1_SCK
	//gppin_init(GPIOB, 4, alternateFunctionPushPull, pullDisable, 0, 6);	//SPI1_MISO
	gppin_init(GPIOB, 5, alternateFunctionPushPull, pullDisable, 0, 6);	//SPI1_MOSI
}

/*!****************************************************************************
 *
 */
void initSpiDMA(void){
	/************************************************
	 * DMA
	 */
	uint32_t dmaChannelTx = 0;
	DMA_Stream_TypeDef *pDmaStreamTx = DMA1_Stream7;

	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
    pDmaStreamTx->CR 	= 0;
    pDmaStreamTx->CR 	|= (uint32_t)((dmaChannelTx & 0x03) << 25);     //Channel selection
    pDmaStreamTx->CR 	|= DMA_SxCR_PL_1;                               //Priority level High
    pDmaStreamTx->CR 	|= DMA_SxCR_MSIZE_0;                            //Memory data size half-word (16-bit)
    pDmaStreamTx->CR 	|= DMA_SxCR_PSIZE_0;                            //Memory data size half-word (16-bit)
    pDmaStreamTx->CR 	|= DMA_SxCR_MINC;                               //Memory increment mode enabled
    pDmaStreamTx->CR 	&= ~DMA_SxCR_PINC;                              //Peripheral increment mode disabled
    pDmaStreamTx->CR 	|= DMA_SxCR_CIRC;                        		//Circular mode enable
    pDmaStreamTx->CR 	|= DMA_SxCR_DIR_0;                              //Direction Memory-to-peripheral
    pDmaStreamTx->NDTR   = sizeof(videoBff) / 2;              			//Number of data
    pDmaStreamTx->PAR    = (uint32_t)&LCD_SPI->DR;                		//Peripheral address
    pDmaStreamTx->M0AR   = (uint32_t)&videoBff[0];                      //Memory address

    gppin_set(GP_LCD_DC);
    LCD_SPI->CR1 |= SPI_CR1_DFF;				//16-bit data frame format is selected for transmission/reception
	LCD_SPI->CR2 |= SPI_CR2_TXDMAEN;
	DMA1_Stream7->CR |= DMA_SxCR_EN;
}

/*!****************************************************************************
 * @brief Send data from SPI
 */
void spiSend(uint8_t data){
	LCD_SPI->DR = data;
	while((LCD_SPI->SR & SPI_SR_TXE) != 0);
	while((LCD_SPI->SR & SPI_SR_BSY) != 0);
}

///*!****************************************************************************
// * @brief Send data from SPI
// */
//void st7735_bffSpiWrite(const void *bff, uint16_t len){
//	DMA_Stream_TypeDef *pDmaStreamTx = LCD_DMA_STREAM;
//	pDmaStreamTx->CR &= ~DMA_SxCR_EN;
//	pDmaStreamTx->CR &= ~DMA_SxCR_CIRC;		//Circular mode disable
//	pDmaStreamTx->M0AR = (uint32_t) &bff;		//Memory address
//	pDmaStreamTx->NDTR = len;					//Number of data
//	pDmaStreamTx->CR |= DMA_SxCR_EN;
//	while((LCD_SPI->SR & SPI_SR_BSY) != 0)
//		;
//}
//
///*!****************************************************************************
// * @brief Circular Send data from SPI
// */
//void st7735_bffCircSpiWrite(const void *bff, uint16_t len){
//	DMA_Stream_TypeDef *pDmaStreamTx = LCD_DMA_STREAM;
//	pDmaStreamTx->CR &= ~DMA_SxCR_EN;
//	pDmaStreamTx->CR |= DMA_SxCR_CIRC;		//Circular mode disable
//	pDmaStreamTx->M0AR = (uint32_t) &bff;		//Memory address
//	pDmaStreamTx->NDTR = len;					//Number of data
//	pDmaStreamTx->CR |= DMA_SxCR_EN;
//}

/*!****************************************************************************
 * @brief Send command
 */
void st7735_lcdCmd(uint8_t cmd){
	gppin_reset(GP_LCD_DC);
	spiSend(cmd);
}

/*!****************************************************************************
 * @brief Send data
 */
void st7735_lcdDat(uint8_t data){
	gppin_set(GP_LCD_DC);
	spiSend(data);
}

/*!****************************************************************************
 * @brief Send data 16 bit
 */
void st7735_lcdDat16(uint16_t data){
	gppin_set(GP_LCD_DC);
	spiSend(data >> 0x08);
	spiSend(data & 0xFF);
}

/*!****************************************************************************
 * @brief
 */
void lcdDelay(uint64_t delay){
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
	st7735_lcdDat(x0 + xstart);   	// XSTART
	st7735_lcdDat(0x00);
	st7735_lcdDat(x1 + xstart);   	// XEND

	st7735_lcdCmd(ST7735_RASET);	// row addr set
	st7735_lcdDat(0x00);
	st7735_lcdDat(y0 + ystart);    	// YSTART
	st7735_lcdDat(0x00);
	st7735_lcdDat(y1 + ystart);    	// YEND

	st7735_lcdCmd(ST7735_RAMWR);	// write to RAM
}

/*!****************************************************************************
 * @brief Companion code to the above tables.  Reads and issues
 * a series of LCD commands stored in PROGMEM byte array.
 */
void commandList(const uint8_t *addr){
	uint8_t  numCommands, numArgs;
	uint16_t ms;

	numCommands = *addr++;   				// Number of commands to follow
	while(numCommands--){					// For each command...
		st7735_lcdCmd(*addr++); 			// Read, issue command
		numArgs		= *addr++;    			// Number of args to follow
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
void commonInit(const uint8_t *cmdList) {
	spiInit();

	gppin_reset(GP_LCD_CS);

	gppin_set(GP_LCD_RST);
	lcdDelay(10);
	gppin_reset(GP_LCD_RST);
	lcdDelay(10);
	gppin_set(GP_LCD_RST);
	lcdDelay(10);

	if(cmdList){
		commandList(cmdList);
	}
}

/*!****************************************************************************
 * @brief Set rotation display
 */
void setRotation(uint8_t m){
	uint8_t rotation;

	st7735_lcdCmd(ST7735_MADCTL);

	rotation = m % 4; // can't be higher than 3
	switch (rotation){
		case 0:
			if ((tabcolor == INITR_BLACKTAB) || (tabcolor == INITR_MINI160x80)) {
				st7735_lcdDat(MADCTL_MX | MADCTL_MY | MADCTL_RGB);
			}else{
				st7735_lcdDat(MADCTL_MX | MADCTL_MY | MADCTL_BGR);
			}

			if (tabcolor == INITR_144GREENTAB) {
				height = ST7735_TFTHEIGHT_128;
				width  = ST7735_TFTWIDTH_128;
			} else if (tabcolor == INITR_MINI160x80)  {
				height = ST7735_TFTHEIGHT_160;
				width = ST7735_TFTWIDTH_80;
			} else {
				height = ST7735_TFTHEIGHT_160;
				width  = ST7735_TFTWIDTH_128;
			}
			xstart = colstart;
			ystart = rowstart;
		break;

		case 1:
			if ((tabcolor == INITR_BLACKTAB) || (tabcolor == INITR_MINI160x80)) {
				st7735_lcdDat(MADCTL_MY | MADCTL_MV | MADCTL_RGB);
			} else {
				st7735_lcdDat(MADCTL_MY | MADCTL_MV | MADCTL_BGR);
			}

			if (tabcolor == INITR_144GREENTAB)  {
				width = ST7735_TFTHEIGHT_128;
				height = ST7735_TFTWIDTH_128;
			} else if (tabcolor == INITR_MINI160x80)  {
				width = ST7735_TFTHEIGHT_160;
				height = ST7735_TFTWIDTH_80;
			} else {
				width = ST7735_TFTHEIGHT_160;
				height = ST7735_TFTWIDTH_128;
			}
			ystart = colstart;
			xstart = rowstart;
		break;

		case 2:
			if ((tabcolor == INITR_BLACKTAB) || (tabcolor == INITR_MINI160x80)) {
				st7735_lcdDat(MADCTL_RGB);
			} else {
				st7735_lcdDat(MADCTL_BGR);
			}

			if (tabcolor == INITR_144GREENTAB) {
				height = ST7735_TFTHEIGHT_128;
				width  = ST7735_TFTWIDTH_128;
			} else if (tabcolor == INITR_MINI160x80)  {
				height = ST7735_TFTHEIGHT_160;
				width = ST7735_TFTWIDTH_80;
			} else {
				height = ST7735_TFTHEIGHT_160;
				width  = ST7735_TFTWIDTH_128;
			}
			xstart = colstart;
			ystart = rowstart;
		break;

		case 3:
			if ((tabcolor == INITR_BLACKTAB) || (tabcolor == INITR_MINI160x80)) {
				st7735_lcdDat(MADCTL_MX | MADCTL_MV | MADCTL_RGB);
			} else {
				st7735_lcdDat(MADCTL_MX | MADCTL_MV | MADCTL_BGR);
			}

			if (tabcolor == INITR_144GREENTAB)  {
				width = ST7735_TFTHEIGHT_128;
				height = ST7735_TFTWIDTH_128;
			} else if (tabcolor == INITR_MINI160x80)  {
				width = ST7735_TFTHEIGHT_160;
				height = ST7735_TFTWIDTH_80;
			} else {
				width = ST7735_TFTHEIGHT_160;
				height = ST7735_TFTWIDTH_128;
			}
			ystart = colstart;
			xstart = rowstart;
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
 * @brief Initialization for ST7735B screens
 */
void initB(void){
	commonInit(Bcmd);
	setRotation(0);
}

/*!****************************************************************************
 * @brief Initialization for ST7735R screens (green or red tabs)
 */
void initR(uint8_t options){
	commonInit(Rcmd1);

	if(options == INITR_GREENTAB){
		commandList(Rcmd2green);
		colstart = 2;
		rowstart = 1;
	}

	else if(options == INITR_144GREENTAB){
		height = ST7735_TFTHEIGHT_128;
		width = ST7735_TFTWIDTH_128;
		commandList(Rcmd2green144);
		colstart = 2;
		rowstart = 3;
	}

	else if(options == INITR_MINI160x80){
		height = ST7735_TFTHEIGHT_160;
		width = ST7735_TFTWIDTH_80;
		commandList(Rcmd2green160x80);
		colstart = 24;
		rowstart = 0;
	}

	else{
		// colstart, rowstart left at default '0' values
		commandList(Rcmd2red);
	}

	commandList(Rcmd3);

	// if black, change MADCTL color filter
	if((options == INITR_BLACKTAB) || (options == INITR_MINI160x80)){
		st7735_lcdCmd(ST7735_MADCTL);
		st7735_lcdDat(0xC0);
	}

	tabcolor = options;

	setRotation(3);

	st7735_setAddressWindow(0, 0, 159, 127);
	initSpiDMA();
}

/***************** Copyright (C) Storozhenko Roman ******* END OF FILE *******/

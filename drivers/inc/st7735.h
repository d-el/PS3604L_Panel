/*!****************************************************************************
 * @file		st7735.h
 * @author		d_el
 * @version		V1.0
 * @date		31.01.2018
 * @brief		Driver display on controller ST7735
 * @copyright	Copyright (C) 2017 Storozhenko Roman
 *				All rights reserved
 *				This software may be modified and distributed under the terms
 *				of the BSD license. See the LICENSE file for details
 */

#ifndef st7735_H
#define st7735_H

/*!****************************************************************************
 * Include
 */
#include "stdint.h"

/*!****************************************************************************
 * Define
 */
#define ST7735_W			160
#define ST7735_H			128
#define LCD_SPI				SPI3
#define LCD_DMA_CHANNEL		0
#define LCD_DMA_STREAM		DMA1_Stream7

/*!****************************************************************************
 * Enumeration
 */
enum initRflags{
	INITR_GREENTAB		= 0x0,
	INITR_REDTAB		= 0x1,
	INITR_BLACKTAB		= 0x2,

	INITR_18GREENTAB	= INITR_GREENTAB,
	INITR_18REDTAB		= INITR_REDTAB,
	INITR_18BLACKTAB	= INITR_BLACKTAB,
	INITR_144GREENTAB	= 0x1,
	INITR_MINI160x80	= 0x4
};

/*!****************************************************************************
 * Typedef
 */
typedef enum {
	sky = 0x54fb,
	black = 0x0000,
	white = 0xFFFF,
	green = 0x07E0,
	blue = 0x001f,
	red = 0xF800,
	yellow = 0xFFE0,
	orange = 0xAB00,
	pink = 0xF97F,
	brown = 0x8200,
	gray = 0x8410,
	lilac = 0x91D4,

	darkGreen = 0x3DA5,
	halfLightGray = 0x39E6,
	halfLightYellow = 0xFFF8,
	halfLightRed = 0xFF18,
	halfLightGreen = 0xC7F8,
	halfLightBlue = 0x861F,
} color_type;

typedef uint32_t lcd_color_type;

/*!****************************************************************************
 * Exported variables
 */
extern uint16_t videoBff[ST7735_W * ST7735_H];

/*!****************************************************************************
 * Macro functions
 */
static inline void st7735_setPixel(uint16_t x, uint16_t y, lcd_color_type color){
	videoBff[y * ST7735_W + x] = color;
}

/*!****************************************************************************
 * Function declaration
 */
void st7735_init(void);
void initB(void);
void initR(uint8_t options);

#endif //st7735_H
/***************** Copyright (C) Storozhenko Roman ******* END OF FILE *******/

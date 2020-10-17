/*!****************************************************************************
 * @file		st7735.h
 * @author		d_el
 * @version		V1.0
 * @date		31.01.2018
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		Driver display on controller ST7735
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
#define LCD_DMAMEM_STREAM	DMA2_Stream0
#define LCD_DMA_IRQPrior	15

/*!****************************************************************************
 * Enumeration
 */
typedef enum {
	JD_T18003,
	IPS16080
}display_type;

typedef enum {
	ST7735_R0,
	ST7735_R90,
	ST7735_R180,
	ST7735_R270
}displayRotation_type;

/*!****************************************************************************
 * Typedef
 */
typedef void (*flushcb_type)(void *arg);
typedef void (*setbufcb_type)(void *arg);

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
void st7735_initB(display_type t, displayRotation_type r);
void st7735_initR(display_type t, displayRotation_type r);
void st7735_flush(flushcb_type cb);
void st7735_setBuffer(lcd_color_type *color, setbufcb_type cb);

#endif //st7735_H
/******************************** END OF FILE ********************************/

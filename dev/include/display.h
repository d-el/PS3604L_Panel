﻿/*!****************************************************************************
 * @file		display.h
 * @author		d_el
 * @version		V1.0
 * @date		17.11.2017
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 */
#ifndef display_H
#define display_H

#ifdef __cplusplus
extern "C" {
#endif

/*!****************************************************************************
 * Include
 */
#include "st7735.h"
#include "stdint.h"

/*!****************************************************************************
 * Define
 */
#define DISP_W	ST7735_W
#define DISP_H	ST7735_H

/*!****************************************************************************
 * Typedef
 */
typedef lcd_color_type disp_color_type;

typedef struct {
	const uint8_t *data;
	uint8_t w;
	uint8_t h;
} image_type;

typedef struct {
	const uint8_t n;
	const image_type *image;
} char_type;

typedef struct {
	int length;
	const char_type *chars;
} font_type;

/*!****************************************************************************
 * Exported variables
 */
extern const font_type font8x12;
extern const font_type font6x8;
extern const font_type arial;
extern const font_type dSegBold;
extern const font_type ico;
extern const image_type ImageLogo;
extern const font_type dSegBold8x14;

/*!****************************************************************************
 * Function declaration
 */
void disp_init(void);
void disp_flush(void);
void disp_flushfill(const lcd_color_type *color);
void disp_setColor(lcd_color_type backgroundColor, lcd_color_type contentColor);
void disp_setBackgroundColor(lcd_color_type backgroundColor);
void disp_setContentColor(lcd_color_type contentColor);
void disp_setPixel(uint16_t x, uint16_t y, lcd_color_type color);
void disp_fillScreen(lcd_color_type color);
void disp_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, lcd_color_type color);
void disp_putChar(uint16_t x, uint16_t y, const font_type *font, char c);
void disp_putStr(uint16_t x, uint16_t y, const font_type *font, uint8_t distance, const char *s);
void disp_PrintImageMonochrome(uint8_t x, uint8_t y, const image_type *image);

#ifdef __cplusplus
}
#endif

#endif //display_H
/******************************** END OF FILE ********************************/

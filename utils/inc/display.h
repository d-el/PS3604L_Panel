/*!****************************************************************************
 * @file		display.h
 * @author		d_el
 * @version		V1.0
 * @date		17.11.2017
 * @copyright	Copyright (C) 2017 Storozhenko Roman
 *				All rights reserved
 *				This software may be modified and distributed under the terms
 *				of the BSD license. See the LICENSE file for details
 */
#ifndef display_H
#define display_H

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
 * Enumeration
 */

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
 * Macro functions
 */

/*!****************************************************************************
 * Function declaration
 */
void disp_setColor(lcd_color_type backgroundColor, lcd_color_type contentColor);
void disp_setBackgroundColor(lcd_color_type backgroundColor);
void disp_setContentColor(lcd_color_type contentColor);
void disp_setPixel(uint16_t x, uint16_t y, lcd_color_type color);
void disp_fillScreen(lcd_color_type color);
void disp_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, lcd_color_type color);
void disp_putChar(uint16_t x, uint16_t y, const font_type *font, char c);
void disp_putStr(uint16_t x, uint16_t y, const font_type *font, uint8_t distance, const char *s);
void disp_PrintImageMonochrome(uint8_t x, uint8_t y, lcd_color_type color, lcd_color_type backgroundColor, const image_type *image);

#endif //display_H
/******************************* END OF FILE *********** D_EL ****************/

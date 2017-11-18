/*!****************************************************************************
 * @file		display.c
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		17.11.2017
 * @copyright	Copyright (C) 2017 d_el
 * 				All rights reserved
 * 				This software may be modified and distributed under the terms
 * 				of the BSD license.  See the LICENSE file for details
 */

/*!****************************************************************************
 * Include
 */
#include "display.h"

/*!****************************************************************************
 * MEMORY
 */
disp_color_type 	uiBackgroundColor;
disp_color_type 	uiContentColor;

/*!****************************************************************************
 *
 */
void disp_setColor(disp_color_type backgroundColor, disp_color_type contentColor){
	uiBackgroundColor = backgroundColor;
	uiContentColor = contentColor;
}

/*!****************************************************************************
 *
 */
void disp_setBackgroundColor(disp_color_type backgroundColor){
	uiBackgroundColor = backgroundColor;
}

/*!****************************************************************************
 *
 */
void disp_setContentColor(disp_color_type contentColor){
	uiContentColor = contentColor;
}

/*!****************************************************************************
 *
 */
void disp_setPixel(uint16_t x, uint16_t y, disp_color_type color){
	spfd_setPixel(x, y, color);
}

/*!****************************************************************************
 *
 */
void disp_fillScreen(disp_color_type color){
	for(uint32_t x = 0; x < DISP_W; x++){
		for(uint32_t y = 0; y < DISP_H; y++){
			disp_setPixel(x, y, color);
		}
	}
}

/*!****************************************************************************
 *
 */
void disp_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, disp_color_type color){
	uint16_t x0 = x;
	uint16_t x2 = x + w;
	uint16_t y2 = y + h;

	while(y < y2){
		while(x < x2){
			disp_setPixel(x, y, color);
			x++;
		}
		x = x0;
		y++;
	}
}

/*!****************************************************************************
 *
 */
void disp_putChar(uint16_t x, uint16_t y, const font_type *font, char c){
	uint8_t width, height;

	c = c - font->chars[0].n;
	uint8_t c_width = font->chars[(uint8_t)c].image->w;
	uint8_t c_height = font->chars[(uint8_t)c].image->h;

	uint8_t *pdata = (uint8_t*) font->chars[(uint8_t)c].image->data;

	uint8_t mask;
	uint8_t bytesPerLine = ((c_width - 1) / 8 + 1);
	uint8_t count;

	for(height = 0; height < c_height; height++){

		for(width = 0; width < bytesPerLine; width++){
			count = c_width - (width * 8);
			if(count > 8){
				count = 8;
			}
			mask = 0x80;
			for(int j = 0; j < count; j++){
				if((*pdata & mask) != 0){
					disp_setPixel(x + (width * 8) + j, y + height, uiBackgroundColor);
				}else{
					disp_setPixel(x + (width * 8) + j, y + height, uiContentColor);
				}
				mask = mask >> 1;
			}
			pdata++;
		}
	}
}

/*!****************************************************************************
 *
 */
void disp_putStr(uint16_t x, uint16_t y, const font_type *font, uint8_t distance, const char *s){
	while(*s != 0){
		disp_putChar(x, y, font, *s);
		x += font->chars[*s - 0x20].image->w + distance;
		s++;
	}
}

/*!****************************************************************************
 *
 */
void disp_PrintImageMonochrome(uint8_t x, uint8_t y, disp_color_type color, disp_color_type backgroundColor, const image_type *image){
	uint8_t width, height;
	uint8_t c_width = image->w;
	uint8_t c_height = image->h;
	uint8_t *pdata = (uint8_t*) image->data;

	uint8_t mask;
	uint8_t bytesPerLine = ((c_width - 1) / 8 + 1);
	uint8_t count;

	for(height = 0; height < c_height; height++){

		for(width = 0; width < bytesPerLine; width++){
			count = c_width - (width * 8);
			if(count > 8){
				count = 8;
			}
			mask = 0x80;
			for(int j = 0; j < count; j++){
				if((*pdata & mask) != 0){
					disp_setPixel(x + (width * 8) + j, y + height, uiBackgroundColor);
				}else{
					disp_setPixel(x + (width * 8) + j, y + height, uiContentColor);
				}
				mask = mask >> 1;
			}
			pdata++;
		}
	}
}

/******************************* END OF FILE *********** D_EL ****************/

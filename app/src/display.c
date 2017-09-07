/*!****************************************************************************
 * @file		display.c
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		25.04.2017
 * @copyright	GNU Lesser General Public License v3
 * @brief		Character generator, primitive graphics
 */

/*!****************************************************************************
 * Include
 */
#include "display.h"

/*!****************************************************************************
 * MEMORY
 */
char 		str[30];	///< String buffer
uint16_t 	uiBackgroundColor;
uint16_t 	uiContentColor;

/*!****************************************************************************
 * @brief
 */
void lcd_setColor(uint16_t backgroundColor, uint16_t contentColor){
	uiBackgroundColor = backgroundColor;
	uiContentColor = contentColor;
}

/*!****************************************************************************
 * @brief Вывод пикселя
 */
void lcd_setPixel(uint16_t x, uint16_t y, uint16_t color){
	spfd_setPixel(x, y, color);
}

/*!****************************************************************************
 * @brief Заливка одним цветом
 */
void lcd_fillScreen(uint16_t color){
	color = ~color;
	uint32_t color32 = ((uint32_t) color << 16) | color;
	uint32_t *data = (uint32_t*) &videoBff[0];
	uint32_t *dataEnd = data + (DISP_W * DISP_H / 2);

	while(data < dataEnd){
		*data++ = color32;
	}
}

/*!****************************************************************************
 * @brief
 */
void lcd_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color){
	uint16_t x0 = x;
	uint16_t x2 = x + w;
	uint16_t y2 = y + h;

	while(y < y2){
		while(x < x2){
			lcd_setPixel(x, y, color);
			x++;
		}
		x = x0;
		y++;
	}
}

/*!****************************************************************************
 * @brief Вывод символа
 */
void lcd_putChar(uint16_t x, uint16_t y, const font_type *font, char c){
	uint8_t width, height;

	c = c - font->chars[0].n;
	uint8_t c_width = font->chars[c].image->w;
	uint8_t c_height = font->chars[c].image->h;

	uint8_t *pdata = (uint8_t*) font->chars[c].image->data;

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
					lcd_setPixel(x + (width * 8) + j, y + height, uiBackgroundColor);
				}else{
					lcd_setPixel(x + (width * 8) + j, y + height, uiContentColor);
				}
				mask = mask >> 1;
			}
			pdata++;
		}
	}
}

/*!****************************************************************************
 * @brief Вывод строки
 */
void lcd_putStr(uint16_t x, uint16_t y, const font_type *font, uint8_t distance, const char *s){
	while(*s != 0){
		lcd_putChar(x, y, font, *s);
		x += font->chars[*s - 0x20].image->w + distance;
		s++;
	}
}

/*!****************************************************************************
 * @brief Вывод картинки
 */
void lcd_PrintImage16bit(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const void *pIm){
	uint16_t *image = (uint16_t*) pIm;
	uint16_t *imageEnd;
	uint16_t *video = videoBff + x;
	uint16_t iw, ih;

	for(ih = 0; ih < h; ih++){
		imageEnd = image + w;
		while(image < imageEnd){
			*video++ = ~__REV16(*image++);
		}
		video += DISP_W - w;
	}
}

/*!****************************************************************************
 * @brief Вывод картинки 8bit R/G/B - 3/3/2
 */
void lcd_PrintImage8bit(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const void *pIm){
	uint8_t *image = (uint8_t*) pIm;
	uint8_t *imageEnd;
	uint16_t *video = videoBff + x;
	uint16_t iw, ih;
	uint16_t color;

	for(ih = 0; ih < h; ih++){
		imageEnd = image + w;
		while(image < imageEnd){
			color = (*image & 0xE0) << 8; //RED
			color |= (*image & 0x1C) << 6; //GREEN
			color |= (*image & 0x03) << 3; //BLUE
			*video++ = ~__REV16(color++);
			image++;
		}
		video += DISP_W - w;
	}
}

/*!****************************************************************************
 * @brief
 */
void lcd_PrintImageMonochrome(uint8_t x, uint8_t y, uint16_t color, uint16_t backgroundColor, const image_type *image){
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
					lcd_setPixel(x + (width * 8) + j, y + height, uiBackgroundColor);
				}else{
					lcd_setPixel(x + (width * 8) + j, y + height, uiContentColor);
				}
				mask = mask >> 1;
			}
			pdata++;
		}
	}
}

/*************** LGPL ************** END OF FILE *********** D_EL ************/

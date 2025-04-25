/*!****************************************************************************
 * @file		display.c
 * @author		d_el
 * @version		V1.0
 * @date		17.11.2017
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 */

/*!****************************************************************************
 * Include
 */
#include <assert.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include "display.h"

/*!****************************************************************************
 * MEMORY
 */
static disp_color_type	uiBackgroundColor;
static disp_color_type	uiContentColor;
static SemaphoreHandle_t	dispFlushSem;

/*!****************************************************************************
 * Initialize display subsystem
 */
void disp_init(void){
	st7735_init(JD_T18003, ST7735_R270);
	dispFlushSem = xSemaphoreCreateBinary();
	assert(dispFlushSem != NULL);
}

/*!****************************************************************************
 * Flush callback
 */
void disp_cb(void *arg){
	(void)arg;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(dispFlushSem, &xHigherPriorityTaskWoken);
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

/*!****************************************************************************
 * Flush video buffer
 */
void disp_flush(void){
	st7735_flush(disp_cb);
	xSemaphoreTake(dispFlushSem, portMAX_DELAY);
}

/*!****************************************************************************
 * Flush video buffer and fill
 */
void disp_flushfill(const lcd_color_type *color){
	st7735_flush(disp_cb);
	xSemaphoreTake(dispFlushSem, portMAX_DELAY);
	st7735_fillBuffer(color, disp_cb);
	xSemaphoreTake(dispFlushSem, portMAX_DELAY);
}

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
	st7735_setPixel(x, y, color);
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

	uint8_t nc = (uint8_t)c - font->chars[0].n;
	uint8_t c_width = font->chars[nc].image->w;
	uint8_t c_height = font->chars[nc].image->h;

	const uint8_t *pdata = font->chars[nc].image->data;

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
		x += font->chars[*(uint8_t*)s - font->chars[0].n].image->w + distance;
		s++;
	}
}

/*!****************************************************************************
 *
 */
void disp_PrintImageMonochrome(uint8_t x, uint8_t y, const image_type *image){
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

/******************************** END OF FILE ********************************/

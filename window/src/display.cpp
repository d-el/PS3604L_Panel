/*!****************************************************************************
 * @file		display.c
 * @author		d_el
 * @version		V1.1
 * @date		11.01.2026
 * @copyright	The MIT License (MIT). Copyright (c) 2026 Storozhenko Roman
 */

/*!****************************************************************************
 * Include
 */
#include <assert.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <algorithm>
#include "display.h"

/*!****************************************************************************
 *
 */
Disp::Disp(const disp_driver_t* driver):
m_driver(*driver){ }

/*!****************************************************************************
 * Initialize display subsystem
 */
void Disp::init(){
	if(m_driver.init){
		m_driver.init();
	}
	m_dispFlushSem = (void*)xSemaphoreCreateBinary();
	assert(m_dispFlushSem != NULL);
}

/*!****************************************************************************
 * Flush callback
 */
void disp_cb(void *sem){
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR((SemaphoreHandle_t)sem, &xHigherPriorityTaskWoken);
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

/*!****************************************************************************
 * Flush video buffer
 */
void Disp::flush(void){
	m_driver.flush(disp_cb, m_dispFlushSem);
	xSemaphoreTake((SemaphoreHandle_t)m_dispFlushSem, portMAX_DELAY);
}

/*!****************************************************************************
 * Flush video buffer and fill
 */
void Disp::flushfill(const disp_color_type *color){
	m_driver.flush(disp_cb, m_dispFlushSem);
	xSemaphoreTake((SemaphoreHandle_t)m_dispFlushSem, portMAX_DELAY);
	m_driver.fillBuffer(color, disp_cb, m_dispFlushSem);
	xSemaphoreTake((SemaphoreHandle_t)m_dispFlushSem, portMAX_DELAY);
}

/*!****************************************************************************
 *
 */
void Disp::setColor(disp_color_type backgroundColor, disp_color_type contentColor){
	m_uiBackgroundColor = backgroundColor;
	m_uiContentColor = contentColor;
}

/*!****************************************************************************
 *
 */
void Disp::setBackgroundColor(disp_color_type backgroundColor){
	m_uiBackgroundColor = backgroundColor;
}

/*!****************************************************************************
 *
 */
void Disp::keepBackgroundColor(bool useBackground){
	m_uiUseBackground = useBackground;
}
/*!****************************************************************************
 *
 */
void Disp::setContentColor(disp_color_type contentColor){
	m_uiContentColor = contentColor;
}

/*!****************************************************************************
 *
 */
void Disp::setPixel(uint16_t x, uint16_t y, disp_color_type color){
	m_driver.setPixel(x, y, color);
}

/*!****************************************************************************
 *
 */
disp_color_type Disp::getPixel(uint16_t x, uint16_t y){
	return m_driver.getPixel(x, y);
}

/*!****************************************************************************
 *
 */
void Disp::fillScreen(disp_color_type color){
	for(uint32_t x = 0; x < m_driver.w; x++){
		for(uint32_t y = 0; y < m_driver.h; y++){
			setPixel(x, y, color);
		}
	}
}

/*!****************************************************************************
 *
 */
void Disp::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, disp_color_type color){
	uint16_t x0 = x;
	uint16_t x2 = x + w;
	uint16_t y2 = y + h;

	while(y < y2){
		while(x < x2){
			setPixel(x, y, color);
			x++;
		}
		x = x0;
		y++;
	}
}

/*!****************************************************************************
 *
 */
void Disp::putChar(uint16_t x, uint16_t y, const font_type *font, char c){
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
					disp_color_type back = m_uiUseBackground ? getPixel(x + (width * 8) + j, y + height) :
															m_uiBackgroundColor;
					setPixel(x + (width * 8) + j, y + height, back);
				}else{
					setPixel(x + (width * 8) + j, y + height, m_uiContentColor);
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
void Disp::putStr(uint16_t x, uint16_t y, const font_type *font, const char *s, uint8_t distance){
	while(*s != 0){
		putChar(x, y, font, *s);
		x += font->chars[*(uint8_t*)s - font->chars[0].n].image->w + distance;
		s++;
	}
}

/*!****************************************************************************
 *
 */
void Disp::putStrCenter(uint16_t y, const font_type *font, const char *s, uint8_t distance){
	uint16_t with = 0;
	const char *c = s;
	while(*c != 0){
		with += font->chars[*(uint8_t*)c - font->chars[0].n].image->w + distance;
		c++;
	}
	putStr((m_driver.w - with) / 2, y, font, s, distance);
}

/*!****************************************************************************
 *
 */
void Disp::printImageMonochrome(uint8_t x, uint8_t y, const image_type *image){
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
					setPixel(x + (width * 8) + j, y + height, m_uiBackgroundColor);
				}else{
					setPixel(x + (width * 8) + j, y + height, m_uiContentColor);
				}
				mask = mask >> 1;
			}
			pdata++;
		}
	}
}

/*!****************************************************************************
 */
uint16_t Disp::abs(int16_t x){
	return ((x) < 0 ? -(x) : (x));
}

/*!****************************************************************************
 * returns random integer from 1 to lim (Gerhard's generator)
 */
uint16_t Disp::rand(uint16_t lim){
	static uint32_t a = 100001; // could be made the seed value
	a = (a * 32719 + 3) % 32749;
	return ((a % lim) + 1);
}

/*!****************************************************************************
 */
void Disp::fastVLine(int16_t x, int16_t y, uint16_t h, disp_color_type color){
	while(h != 0){
		m_driver.setPixel(x, y + h-- - 1, color);
	}
}

/*!****************************************************************************
 */
void Disp::fastHLine(int16_t x, int16_t y, uint16_t w, disp_color_type color){
	while(w != 0){
		m_driver.setPixel(x + w-- - 1, y, color);
	}
}

/*!****************************************************************************
 */
void Disp::line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, disp_color_type color){
	int16_t deltaX = abs(x2 - x1);
	int16_t deltaY = abs(y2 - y1);
	int8_t signX = x1 < x2 ? 1 : -1;
	int8_t signY = y1 < y2 ? 1 : -1;
	int16_t error2;

	int16_t error = deltaX - deltaY;

	m_driver.setPixel(x2, y2, color);

	while(x1 != x2 || y1 != y2){
		m_driver.setPixel(x1, y1, color);
		error2 = error * 2;
		//
		if(error2 > -deltaY){
			error -= deltaY;
			x1 += signX;
		}
		if(error2 < deltaX){
			error += deltaX;
			y1 += signY;
		}
	}
}

/*!****************************************************************************
 */
void Disp::circle(int16_t x0, int16_t y0, uint16_t radius, disp_color_type color){
	int16_t error, x, y;
	error = -radius;
	x = radius;
	y = 0;

	while(x >= y){
		m_driver.setPixel(x0 + x, y0 + y, color);
		m_driver.setPixel(x0 - x, y0 + y, color);
		m_driver.setPixel(x0 + x, y0 - y, color);
		m_driver.setPixel(x0 - x, y0 - y, color);
		m_driver.setPixel(x0 + y, y0 + x, color);
		m_driver.setPixel(x0 - y, y0 + x, color);
		m_driver.setPixel(x0 + y, y0 - x, color);
		m_driver.setPixel(x0 - y, y0 - x, color);

		error += y;
		y++;
		error += y;
		if(error >= 0){
			x--;
			error -= x;
			error -= x;
		}
	}
}

/*!****************************************************************************
 */
void Disp::circleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, disp_color_type color){
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	while(x < y){
		if(f >= 0){
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;
		if(cornername & 0x4){
			m_driver.setPixel(x0 + x, y0 + y, color);
			m_driver.setPixel(x0 + y, y0 + x, color);
		}
		if(cornername & 0x2){
			m_driver.setPixel(x0 + x, y0 - y, color);
			m_driver.setPixel(x0 + y, y0 - x, color);
		}
		if(cornername & 0x8){
			m_driver.setPixel(x0 - y, y0 + x, color);
			m_driver.setPixel(x0 - x, y0 + y, color);
		}
		if(cornername & 0x1){
			m_driver.setPixel(x0 - y, y0 - x, color);
			m_driver.setPixel(x0 - x, y0 - y, color);
		}
	}
}

/*!****************************************************************************
 */
void Disp::fillCircle(int16_t x0, int16_t y0, int16_t r, disp_color_type color){
	fastVLine(x0, y0 - r, 2 * r + 1, color);
	fillCircleHelper(x0, y0, r, 3, 0, color);
}

/*!****************************************************************************
 */
void Disp::fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, disp_color_type color){
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	while(x < y){
		if(f >= 0){
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		if(cornername & 0x1){
			fastVLine(x0 + x, y0 - y, 2 * y + 1 + delta, color);
			fastVLine(x0 + y, y0 - x, 2 * x + 1 + delta, color);
		}
		if(cornername & 0x2){
			fastVLine(x0 - x, y0 - y, 2 * y + 1 + delta, color);
			fastVLine(x0 - y, y0 - x, 2 * x + 1 + delta, color);
		}
	}
}

/*!****************************************************************************
 */
void Disp::rect(int16_t x, int16_t y, int16_t w, int16_t h, disp_color_type color){
	fastHLine(x, y, w, color);
	fastHLine(x, y + h - 1, w, color);
	fastVLine(x, y, h, color);
	fastVLine(x + w - 1, y, h, color);
}

/*!****************************************************************************
 */
void Disp::roundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, disp_color_type color){
	//Smarter version
	fastHLine(x + r, y, w - 2 * r, color); // Top
	fastHLine(x + r, y + h - 1, w - 2 * r, color); // Bottom
	fastVLine(x, y + r, h - 2 * r, color); // Left
	fastVLine(x + w - 1, y + r, h - 2 * r, color); // Right
	//Draw four corners
	circleHelper(x + r, y + r, r, 1, color);
	circleHelper(x + w - r - 1, y + r, r, 2, color);
	circleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
	circleHelper(x + r, y + h - r - 1, r, 8, color);
}

/*!****************************************************************************
 */
void Disp::fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, disp_color_type color){
	// smarter version
	fillRect(x + r, y, w - 2 * r, h, color);
	// draw four corners
	fillCircleHelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
	fillCircleHelper(x + r, y + r, r, 2, h - 2 * r - 1, color);
}

/*!****************************************************************************
 */
void Disp::triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, disp_color_type color){
	line(x0, y0, x1, y1, color);
	line(x1, y1, x2, y2, color);
	line(x2, y2, x0, y0, color);
}

/*!****************************************************************************
 */
void Disp::fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, disp_color_type color){
	int16_t a, b, y, last;

	// Sort coordinates by Y order (y2 >= y1 >= y0)
	if(y0 > y1){
		std::swap(y0, y1);
		std::swap(x0, x1);
	}
	if(y1 > y2){
		std::swap(y2, y1);
		std::swap(x2, x1);
	}
	if(y0 > y1){
		std::swap(y0, y1);
		std::swap(x0, x1);
	}

	if(y0 == y2){ // Handle awkward all-on-same-line case as its own thing
		a = b = x0;
		if(x1 < a)
			a = x1;
		else if(x1 > b)
			b = x1;
		if(x2 < a)
			a = x2;
		else if(x2 > b)
			b = x2;
		fastHLine(a, y0, b - a + 1, color);
		return;
	}

	int16_t dx01 = x1 - x0, dy01 = y1 - y0, dx02 = x2 - x0, dy02 = y2 - y0, dx12 = x2 - x1, dy12 = y2 - y1;
	int32_t sa = 0, sb = 0;

	// For upper part of triangle, find scanline crossings for segments
	// 0-1 and 0-2.	 If y1=y2 (flat-bottomed triangle), the scanline y1
	// is included here (and second loop will be skipped, avoiding a /0
	// error there), otherwise scanline y1 is skipped here and handled
	// in the second loop...which also avoids a /0 error here if y0=y1
	// (flat-topped triangle).
	if(y1 == y2)
		last = y1;	 // Include y1 scanline
	else
		last = y1 - 1; // Skip it

	for(y = y0; y <= last; y++){
		a = x0 + sa / dy01;
		b = x0 + sb / dy02;
		sa += dx01;
		sb += dx02;
		/* longhand:
		 a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
		 b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
		 */
		if(a > b)
			std::swap(a, b);
		fastHLine(a, y, b - a + 1, color);
	}

	// For lower part of triangle, find scanline crossings for segments
	// 0-2 and 1-2.	 This loop is skipped if y1=y2.
	sa = dx12 * (y - y1);
	sb = dx02 * (y - y0);
	for(; y <= y2; y++){
		a = x1 + sa / dy12;
		b = x0 + sb / dy02;
		sa += dx12;
		sb += dx02;
		/* longhand:
		 a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
		 b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
		 */
		if(a > b)
			std::swap(a, b);
		fastHLine(a, y, b - a + 1, color);
	}
}

/*!****************************************************************************
 */
uint16_t Disp::getWith(){
	return m_driver.w;
}

/*!****************************************************************************
 */
uint16_t Disp::getHeight(){
	return m_driver.h;
}

/*!****************************************************************************
 */
const char* Disp::geName(){
	return m_driver.name;
}

/*!****************************************************************************
 */
void Disp::setBrightness(uint16_t brightness){
	if(m_driver.setBrightness){
		m_driver.setBrightness(brightness);
	}
}

/******************************** END OF FILE ********************************/

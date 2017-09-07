/*!****************************************************************************
 * @file    graphics.c 
 * @author  Adafruit, D_EL
 * @version V1.0
 * @date    26-03-2015
 * @brief   
 */

/*!****************************************************************************
 * Include
 */
#include "graphics.h"

/*!****************************************************************************
 */
unsigned char _abs(signed int x){
	return ((x) < 0 ? -(x) : (x));
}

/*!****************************************************************************
 * returns random integer from 1 to lim (Gerhard's generator)
 */
uint16_t _rand(uint16_t lim){
	static uint32_t a = 100001; // could be made the seed value
	a = (a * 32719 + 3) % 32749;
	return ((a % lim) + 1);
}

/*!****************************************************************************
 */
void grf_pixel(int16_t x, int16_t y, uint16_t color){
	lcd_setPixel(x, y, color);
}

/*!****************************************************************************
 */
void grf_fastVLine(int16_t x, int16_t y, uint16_t h, uint16_t color){
	while(h != 0){
		lcd_setPixel(x, y + h-- - 1, color);
	}
}

/*!****************************************************************************
 */
void grf_fastHLine(int16_t x, int16_t y, uint16_t w, uint16_t color){
	while(w != 0){
		lcd_setPixel(x + w-- - 1, y, color);
	}
}

/*!****************************************************************************
 */
void grf_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color){
	//инициализация переменных
	int16_t deltaX = abs(x2 - x1);
	int16_t deltaY = abs(y2 - y1);
	int8_t signX = x1 < x2 ? 1 : -1;
	int8_t signY = y1 < y2 ? 1 : -1;
	int16_t error2;
	//инициализация ошибки 
	int16_t error = deltaX - deltaY;
	//вывод последней точки
	grf_pixel(x2, y2, color);
	//основной цикл
	while(x1 != x2 || y1 != y2){
		grf_pixel(x1, y1, color);
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
void grf_circle(int16_t x0, int16_t y0, uint16_t radius, uint16_t color){
	int16_t error, x, y;
	error = -radius;
	x = radius;
	y = 0;
	
	while(x >= y){
		grf_pixel(x0 + x, y0 + y, color);
		grf_pixel(x0 - x, y0 + y, color);
		grf_pixel(x0 + x, y0 - y, color);
		grf_pixel(x0 - x, y0 - y, color);
		grf_pixel(x0 + y, y0 + x, color);
		grf_pixel(x0 - y, y0 + x, color);
		grf_pixel(x0 + y, y0 - x, color);
		grf_pixel(x0 - y, y0 - x, color);
		
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
void grf_circleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color){
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
			grf_pixel(x0 + x, y0 + y, color);
			grf_pixel(x0 + y, y0 + x, color);
		}
		if(cornername & 0x2){
			grf_pixel(x0 + x, y0 - y, color);
			grf_pixel(x0 + y, y0 - x, color);
		}
		if(cornername & 0x8){
			grf_pixel(x0 - y, y0 + x, color);
			grf_pixel(x0 - x, y0 + y, color);
		}
		if(cornername & 0x1){
			grf_pixel(x0 - y, y0 - x, color);
			grf_pixel(x0 - x, y0 - y, color);
		}
	}
}

/*!****************************************************************************
 */
void grf_fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color){
	grf_fastVLine(x0, y0 - r, 2 * r + 1, color);
	grf_fillCircleHelper(x0, y0, r, 3, 0, color);
}

/*!****************************************************************************
 */
void grf_fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color){
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
			grf_fastVLine(x0 + x, y0 - y, 2 * y + 1 + delta, color);
			grf_fastVLine(x0 + y, y0 - x, 2 * x + 1 + delta, color);
		}
		if(cornername & 0x2){
			grf_fastVLine(x0 - x, y0 - y, 2 * y + 1 + delta, color);
			grf_fastVLine(x0 - y, y0 - x, 2 * x + 1 + delta, color);
		}
	}
}

/*!****************************************************************************
 */
void grf_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color){
	grf_fastHLine(x, y, w, color);
	grf_fastHLine(x, y + h - 1, w, color);
	grf_fastVLine(x, y, h, color);
	grf_fastVLine(x + w - 1, y, h, color);
}

/*!****************************************************************************
 */
void grf_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color){
	lcd_fillRect(x, y, w, h, color);
}

/*!****************************************************************************
 */
void grf_roundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color){
	//Smarter version
	grf_fastHLine(x + r, y, w - 2 * r, color); // Top
	grf_fastHLine(x + r, y + h - 1, w - 2 * r, color); // Bottom
	grf_fastVLine(x, y + r, h - 2 * r, color); // Left
	grf_fastVLine(x + w - 1, y + r, h - 2 * r, color); // Right
	//Draw four corners
	grf_circleHelper(x + r, y + r, r, 1, color);
	grf_circleHelper(x + w - r - 1, y + r, r, 2, color);
	grf_circleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
	grf_circleHelper(x + r, y + h - r - 1, r, 8, color);
}

/*!****************************************************************************
 */
void grf_fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color){
	// smarter version
	grf_fillRect(x + r, y, w - 2 * r, h, color);
	// draw four corners
	grf_fillCircleHelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
	grf_fillCircleHelper(x + r, y + r, r, 2, h - 2 * r - 1, color);
}

/*!****************************************************************************
 */
void grf_fillScreen(uint16_t color){
	grf_fillRect(0, 0, DISPH, DISPV, color);
}

/*!****************************************************************************
 */
void grf_triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color){
	grf_line(x0, y0, x1, y1, color);
	grf_line(x1, y1, x2, y2, color);
	grf_line(x2, y2, x0, y0, color);
}

/*!****************************************************************************
 */
void grf_fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color){
	int16_t a, b, y, last;
	
	// Sort coordinates by Y order (y2 >= y1 >= y0)
	if(y0 > y1){
		swap(y0, y1);
		swap(x0, x1);
	}
	if(y1 > y2){
		swap(y2, y1);
		swap(x2, x1);
	}
	if(y0 > y1){
		swap(y0, y1);
		swap(x0, x1);
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
		grf_fastHLine(a, y0, b - a + 1, color);
		return;
	}
	
	int16_t dx01 = x1 - x0, dy01 = y1 - y0, dx02 = x2 - x0, dy02 = y2 - y0, dx12 = x2 - x1, dy12 = y2 - y1;
	int32_t sa = 0, sb = 0;
	
	// For upper part of triangle, find scanline crossings for segments
	// 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
	// is included here (and second loop will be skipped, avoiding a /0
	// error there), otherwise scanline y1 is skipped here and handled
	// in the second loop...which also avoids a /0 error here if y0=y1
	// (flat-topped triangle).
	if(y1 == y2)
		last = y1;   // Include y1 scanline
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
			swap(a, b);
		grf_fastHLine(a, y, b - a + 1, color);
	}
	
	// For lower part of triangle, find scanline crossings for segments
	// 0-2 and 1-2.  This loop is skipped if y1=y2.
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
			swap(a, b);
		grf_fastHLine(a, y, b - a + 1, color);
	}
}

/******************* (C) COPYRIGHT ***************** END OF FILE ********* D_EL *****/

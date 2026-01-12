/*!****************************************************************************
 * @file		display.h
 * @author		d_el
 * @version		V1.1
 * @date		11.01.2026
 * @copyright	The MIT License (MIT). Copyright (c) 2026 Storozhenko Roman
 */
#ifndef display_H
#define display_H

/*!****************************************************************************
 * Include
 */
#include <stdint.h>
#include <stdbool.h>
#include <display-driver.h>
#include "font.h"

/*!****************************************************************************
 * Function declaration
 */
class Disp{
public:
	Disp(const disp_driver_t* driver);
	void init();
	void flush(void);
	void flushfill(const disp_color_type *color);
	void setColor(disp_color_type backgroundColor, disp_color_type contentColor);
	void setBackgroundColor(disp_color_type backgroundColor);
	void keepBackgroundColor(bool useBackground);
	void setContentColor(disp_color_type contentColor);
	void setPixel(uint16_t x, uint16_t y, disp_color_type color);
	disp_color_type getPixel(uint16_t x, uint16_t y);
	void fillScreen(disp_color_type color);
	void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, disp_color_type color);
	void putChar(uint16_t x, uint16_t y, const font_type *font, char c);
	void putStr(uint16_t x, uint16_t y, const font_type *font, const char *s, uint8_t distance=0);
	void putStrCenter(uint16_t y, const font_type *font, const char *s, uint8_t distance=0);
	void printImageMonochrome(uint8_t x, uint8_t y, const image_type *image);

	void pixel(int16_t x, int16_t y, disp_color_type color);
	void fastVLine(int16_t x, int16_t y, uint16_t h, disp_color_type color);
	void fastHLine(int16_t x, int16_t y, uint16_t w, disp_color_type color);
	void line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, disp_color_type color);
	void circle(int16_t x0, int16_t y0, uint16_t radius, disp_color_type color);
	void circleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, disp_color_type color);
	void fillCircle(int16_t x0, int16_t y0, int16_t r, disp_color_type color);
	void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, disp_color_type color);
	void rect(int16_t x, int16_t y, int16_t w, int16_t h, disp_color_type color);
	void roundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, disp_color_type color);
	void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, disp_color_type color);
	void triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, disp_color_type color);
	void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, disp_color_type color);

	uint16_t getWith();
	void setBrightness(uint16_t brightness);

private:
	const disp_driver_t m_driver;
	disp_color_type m_uiBackgroundColor = 0;
	disp_color_type m_uiContentColor = 0;
	bool m_uiUseBackground = 0;
	void* m_dispFlushSem = nullptr;

private:
	uint16_t abs(int16_t x);
	uint16_t rand(uint16_t lim);
};

#endif //display_H
/******************************** END OF FILE ********************************/

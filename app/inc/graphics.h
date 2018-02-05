/*!****************************************************************************
 * @file		graphics.h
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		17.11.2017
 * @copyright	Copyright (C) 2017 d_el
 * 				All rights reserved
 * 				This software may be modified and distributed under the terms
 * 				of the BSD license.  See the LICENSE file for details
 */
#ifndef GRAPHICS_H
#define GRAPHICS_H

/*!****************************************************************************
 * Include
 */
#include "stdint.h"
#include "st7735.h"

/*!****************************************************************************
 * User define
 */
#define GRF_DISP_W   DISP_W
#define GRF_DISP_H   DISP_H

/*!****************************************************************************
 * Enumeration
 */

/******************************************************************************
 * Typedef
 */
typedef lcd_color_type gfx_color_type;

/*!****************************************************************************
 * Exported variables
 */

/*!****************************************************************************
 * Macro functions
 */

/*!****************************************************************************
 * Function declaration
 */
uint16_t grf_rand(uint16_t lim);
void grf_pixel(int16_t x, int16_t y, gfx_color_type color);
void grf_fastVLine(int16_t x, int16_t y, uint16_t h, gfx_color_type color);
void grf_fastHLine(int16_t x, int16_t y, uint16_t w, gfx_color_type color);
void grf_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, gfx_color_type color);
void grf_circle(int16_t x0, int16_t y0, uint16_t radius, gfx_color_type color);
void grf_circleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, gfx_color_type color);
void grf_fillCircle(int16_t x0, int16_t y0, int16_t r, gfx_color_type color);
void grf_fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, gfx_color_type color);
void grf_rect(int16_t x, int16_t y, int16_t w, int16_t h, gfx_color_type color);
void grf_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, gfx_color_type color);
void grf_roundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, gfx_color_type color);
void grf_fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, gfx_color_type color);
void grf_fillScreen(uint16_t color);
void grf_triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, gfx_color_type color);
void grf_fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, gfx_color_type color);

#endif //GRAPHICS_H
/******************************* END OF FILE *********** D_EL ****************/

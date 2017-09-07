/*!****************************************************************************
 * @file    graphics.h 
 * @author  Adafruit, D_EL
 * @version V1.0
 * @date    26-03-2015
 * @brief   
 */
#ifndef GRAPHICS_H
#define GRAPHICS_H

/*!****************************************************************************
 * Include
 */
#include "stdint.h"
#include "stdlib.h"
#include "display.h"

/*!****************************************************************************
 * User define
 */
#define DISPH   128
#define DISPV   160

/*!****************************************************************************
 * User enum
 */

/*!****************************************************************************
 * User typedef
 */

/*!****************************************************************************
 * Extern viriables
 */

/*!****************************************************************************
 * Macro functions
 */
#define swap(a, b) { int16_t t = a; a = b; b = t; }

/*!****************************************************************************
 * Prototypes for the functions
 */
unsigned char _abs(signed int x);
uint16_t _rand(uint16_t lim);

void grf_pixel(int16_t x, int16_t y, uint16_t color);
void grf_fastVLine(int16_t x, int16_t y, uint16_t h, uint16_t color);
void grf_fastHLine(int16_t x, int16_t y, uint16_t w, uint16_t color);
void grf_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void grf_circle(int16_t x0, int16_t y0, uint16_t radius, uint16_t color);
void grf_circleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
void grf_fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void grf_fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);
void grf_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void grf_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void grf_roundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
void grf_fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
void grf_fillScreen(uint16_t color);
void grf_triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void grf_fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);

#endif //GRAPHICS_H

/******************* (C) COPYRIGHT ***************** END OF FILE ********* D_EL *****/

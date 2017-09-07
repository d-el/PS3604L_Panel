/*!****************************************************************************
 * @file		display.h
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		25.04.2017
 * @copyright	GNU Lesser General Public License v3
 * @brief		Character generator, primitive graphics
 */
#ifndef display_H
#define display_H

/*!****************************************************************************
 * Include
 */
#include "spfd54124b.h"

/*!****************************************************************************
 * Define
 */
#define DISP_W	SPFD54124B_W
#define DISP_H	SPFD54124B_H

/*!****************************************************************************
 * Enumeration
 */

/*!****************************************************************************
 * Typedef
 */
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
extern char str[30];
extern const font_type font8x12;
extern const font_type font6x8;
extern const font_type arial;
extern const font_type dSegBold;
extern const font_type ico;
extern const image_type ImageLogo;

/*!****************************************************************************
 * Macro functions
 */

/*!****************************************************************************
 * Function prototype
 */
void lcd_setColor(uint16_t backgroundColor, uint16_t contentColor);
void lcd_setPixel(uint16_t x, uint16_t y, uint16_t color);
void lcd_fillScreen(uint16_t color);
void lcd_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void lcd_putChar(uint16_t x, uint16_t y, const font_type *font, char c);
void lcd_putStr(uint16_t x, uint16_t y, const font_type *font, uint8_t distance, const char *s);
void lcd_PrintImage16bit(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const void *pIm);
void lcd_PrintImage8bit(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const void *pIm);
void lcd_PrintImageMonochrome(uint8_t x, uint8_t y, uint16_t color, uint16_t backgroundColor, const image_type *image);

#endif //display_H
/*************** LGPL ************** END OF FILE *********** D_EL ************/

/*!****************************************************************************
 * @file		font.h
 * @author		d_el
 * @version		V1.0
 * @date		11.01.2026
 * @copyright	License (MIT). Copyright (c) 2026 Storozhenko Roman
 * @brief
 */

#ifndef font_H
#define font_H

/*!****************************************************************************
 * Include
 */
#include <stdint.h>

typedef enum {
	sky = 0x54fb,
	black = 0x0000,
	white = 0xFFFF,
	green = 0x07E0,
	blue = 0x001f,
	red = 0xF800,
	yellow = 0xFFE0,
	orange = 0xAB00,
	pink = 0xF97F,
	brown = 0x8200,
	gray = 0x8410,
	lilac = 0x91D4,

	darkGreen = 0x3DA5,
	halfLightGray = 0x39E6,
	halfLightYellow = 0xFFF8,
	halfLightRed = 0xFF18,
	halfLightGreen = 0xC7F8,
	halfLightBlue = 0x861F,
} color_type;

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

extern const font_type font8x12;
extern const font_type font5x7;
extern const font_type font6x8;
extern const font_type arial;
extern const font_type dSegBold;
extern const font_type ico;
extern const image_type ImageLogo;
extern const font_type dSegBold8x14;

#endif //font_H
/******************************** END OF FILE ********************************/

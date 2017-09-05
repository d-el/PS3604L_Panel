/*!****************************************************************************
 * @file		spfd54124b.h
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		19.12.2015
 * @copyright	GNU Lesser General Public License v3
 * @brief		Driver display on controller spfd54124b
 */
#ifndef spfd54124b_H
#define spfd54124b_H

/*!****************************************************************************
 * Include
 */
#include "stm32f4xx.h"
#include "string.h"
#include "gpio.h"

/*!****************************************************************************
 * Define
 */
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
	halfLightGray = 0xDF7B,
	halfLightYellow = 0xFFF8,
	halfLightRed = 0xFF18,
	halfLightGreen = 0xC7F8,
	halfLightBlue = 0x861F,
} color_type;

//#define displayOffsetX  2	//Для книжной
//#define displayOffsetY  1	//Для книжной
#define displayOffsetX  1	//Для альбомной
#define displayOffsetY  2	//Для альбомной
//#define SPFD54124B_W 128	//Для книжной
//#define SPFD54124B_H 160	//Для книжной
#define SPFD54124B_W 160	//Для альбомной
#define SPFD54124B_H 128	//Для альбомной

/*!****************************************************************************
 * Enumeration
 */

/*!****************************************************************************
 * Typedef
 */

/*!****************************************************************************
 * Exported variables
 */
extern uint16_t videoBff[];

/*!****************************************************************************
 * Macro functions
 */
static inline void spfd_setPixel(uint16_t x, uint16_t y, uint16_t color){
	videoBff[(y * SPFD54124B_W) + x] = ~__REV16(color);
}

/*!****************************************************************************
 * Function declaration
 */
void spfd_init(void);
void spfd_disable(void);
void spfd_contrast(uint8_t data);

#endif //spfd54124b_H
/*************** LGPL ************** END OF FILE *********** D_EL ************/

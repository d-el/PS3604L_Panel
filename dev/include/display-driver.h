/*!****************************************************************************
 * @file		display-driver.h
 * @author		d_el
 * @version		V1.0
 * @date		11.01.2026
 * @copyright	License (MIT). Copyright (c) 2026 Storozhenko Roman
 * @brief
 */

#ifndef display_driver_H
#define display_driver_H

/*!****************************************************************************
 * Include
 */
#include "stdint.h"

/*!****************************************************************************
 * Typedef
 */
typedef void (*lcd_flushcb_type)(void* arg);
typedef void (*lcd_setbufcb_type)(void* arg);
typedef uint16_t disp_color_type;

typedef struct{
	void (*init)(void);
	void (*setPixel)(uint16_t x, uint16_t y, disp_color_type color);
	disp_color_type (*getPixel)(uint16_t x, uint16_t y);
	void (*flush)(lcd_flushcb_type cb, void* arg);
	void (*fillBuffer)(const disp_color_type *color, lcd_setbufcb_type cb, void* arg);
	void (*setBrightness)(uint16_t brightness);
	uint16_t h, w;
	const char* name;
} disp_driver_t;

#endif //display_driver_H
/******************************** END OF FILE ********************************/

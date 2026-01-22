/*!****************************************************************************
 * @file		st7735.h
 * @author		d_el
 * @version		V1.1
 * @date		11.01.2026
 * @copyright	The MIT License (MIT). Copyright (c) 2026 Storozhenko Roman
 * @brief		Driver display on controller ST7735
 */

#ifndef st7735_H
#define st7735_H

#ifdef __cplusplus
extern "C" {
#endif


/*!****************************************************************************
 * Include
 */
#include <stdint.h>
#include "display-driver.h"

/*!****************************************************************************
 * Enumeration
 */
typedef enum {
	JD_T18003,
	IPS16080
}display_type;

typedef enum {
	ST7735_R0,
	ST7735_R90,
	ST7735_R180,
	ST7735_R270
}displayRotation_type;

typedef uint16_t lcd_color_type;
extern const disp_driver_t st7735_driver;

/*!****************************************************************************
 * Function declaration
 */
void st7735_init(display_type type, displayRotation_type r);

#ifdef __cplusplus
}
#endif

#endif //st7735_H
/******************************** END OF FILE ********************************/

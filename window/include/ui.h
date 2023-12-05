/*!****************************************************************************
 * @file		ui.h
 * @author		d_el
 * @version		V1.0
 * @date		20.12.2015, by d_el
 * @brief		user interface
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 */
#ifndef ui_H
#define ui_H

/*!****************************************************************************
 * Include
 */
#include "stdint.h"
#include "display.h"

/*!****************************************************************************
 * User define
 */

/*!****************************************************************************
 * User enum
 */

/*!****************************************************************************
 * User typedef
 */
typedef struct {
	disp_color_type cursor;
	disp_color_type voltage;
	disp_color_type current;
	disp_color_type capacity;
	disp_color_type imax;
	disp_color_type mode;
	disp_color_type background;
} uicolor_type;

typedef struct {
	uint16_t freq;
	uint16_t time;
} beepmem_type;

typedef struct {
	beepmem_type welcome;
	beepmem_type shutdown;
	beepmem_type key;
	beepmem_type vallim;
	beepmem_type cvToCc;
	beepmem_type ccToCv;
	beepmem_type ovfCurrent;
	beepmem_type chargeFinish;
	beepmem_type error;
	beepmem_type encoLim;
	beepmem_type encoTransition;
} uibeep_type;

typedef struct {
	uicolor_type color;
	uibeep_type beep;
} ui_type;

/*!****************************************************************************
 * External variables
 */
extern const ui_type ui;

/*!****************************************************************************
 * Macro functions
 */

/*!****************************************************************************
 * Prototypes for the functions
 */

#endif //ui_H
/******************************** END OF FILE ********************************/

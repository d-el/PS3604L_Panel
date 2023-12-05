/*!****************************************************************************
 * @file		ui.c
 * @author		d_el
 * @version		V1.0
 * @date		20.12.2015, by d_el
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		user interface
 */

/*!****************************************************************************
 * Include
 */
#include "ui.h"

/*!****************************************************************************
 * MEMORY
 */
const ui_type ui = {
	.color = {
		.cursor = green,
		.voltage = white,
		.current = white,
		.capacity = white,
		.imax = white,
		.mode = white,
		.background = black
	},

	.beep = {
		.welcome		= { 4100, 250 },
		.shutdown		= { 4100, 50 },
		.key			= { 4100, 30 },
		.vallim			= { 4100, 500 },
		.cvToCc			= { 3000, 50 },
		.ccToCv			= { 4500, 50 },
		.ovfCurrent		= { 4500, 500 },
		.chargeFinish	= { 4100, 2000 },
		.error			= { 3000, 100 },
		.encoLim		= { 4100, 200 },
		.encoTransition = { 4100, 30 },
	}
};

/******************************** END OF FILE ********************************/

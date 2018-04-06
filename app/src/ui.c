/*!****************************************************************************
 * @file		ui.c
 * @author		d_el
 * @version		V1.0
 * @date		20.12.2015, by d_el
 * @brief		user interface
 */

/*!****************************************************************************
 * Include
 */
#include "ui.h"
#include "display.h"

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
	},

	.beep = {
		.welcome		= { 4100, 250 },
		.goodbye		= { 4100, 50 },
		.key			= { 4100, 30 },
		.vallim			= { 4100, 500 },
		.cvToCc			= { 3000, 50 },
		.ccToCv			= { 4500, 50 },
		.ovfCurrent		= { 4500, 500 },
		.chargeReady	= { 4100, 2000 },
		.error			= { 3000, 1000 },
		.encoLim		= { 4100, 200 },
		.encoTransition = { 4100, 30 },
	}
};

/***************** (C) COPYRIGHT ************** END OF FILE ******** d_el ****/


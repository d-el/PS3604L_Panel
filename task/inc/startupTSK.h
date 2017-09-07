/*!****************************************************************************
 * @file		startupTSK.h
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		02.08.2017
 * @copyright	GNU Lesser General Public License v3
 * @brief		This task create start screen
 */
#ifndef startupTSK_H
#define startupTSK_H

/*!****************************************************************************
 * Include
 */
#include "display.h"
#include "beep.h"
#include "ui.h"
#include "version.h"
#include "systemTSK.h"
#include "FreeRTOS.h"
#include "task.h"

/*!****************************************************************************
 * Define
 */

/*!****************************************************************************
 * Enumeration
 */

/*!****************************************************************************
 * Typedef
 */

/*!****************************************************************************
 * Exported variables
 */
extern uint32_t startCounter;

/*!****************************************************************************
 * Macro functions
 */

/*!****************************************************************************
 * Function declaration
 */
void startupTSK(void *pPrm);

#endif //startupTSK_H
/*************** LGPL ************** END OF FILE *********** D_EL ************/

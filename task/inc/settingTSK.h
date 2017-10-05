/*!****************************************************************************
 * @file		settingTSK.h
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		01.01.2016
 * @copyright	GNU Lesser General Public License v3
 * @brief		This task create start screen
 */
#ifndef settingTSK_H
#define settingTSK_H

/*!****************************************************************************
 * Include
 */
#include "FreeRTOS.h"
#include "OSinit.h"
#include "pstypes.h"
#include "storage.h"
#include "rtc.h"
#include "main.h"
#include "menuSystem.h"
#include "enco.h"
#include "systemTSK.h"

/*!****************************************************************************
 * Define
 */

/*!****************************************************************************
 * Enumeration
 */

/*!****************************************************************************
 * Typedef
 */
typedef struct {
	uint16_t u;
	uint16_t i;
} settingSct_type;

/*!****************************************************************************
 * Exported variables
 */

/*!****************************************************************************
 * Macro functions
 */

/*!****************************************************************************
 * Function declaration
 */
void settingTSK(void *pPrm);
uint32_t PrepareU(const menuItem_type *item);
uint32_t PrepareI(const menuItem_type *item);
uint32_t regSave(const menuItem_type *item);
uint32_t savePointU(const menuItem_type *item);
uint32_t savePointI(const menuItem_type *item);
uint32_t setBright(const menuItem_type *item);
uint32_t rtcPeriodic(const menuItem_type *item);
uint32_t rtcSelectc(const menuItem_type *item);
uint32_t rtcUselectc(const menuItem_type *item);
uint32_t rtcCh(const menuItem_type *item);

#endif //settingTSK_H
/*************** LGPL ************** END OF FILE *********** D_EL ************/

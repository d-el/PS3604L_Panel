/*!****************************************************************************
 * @file		settingTSK.h
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		01.01.2016
 * @copyright	The MIT License (MIT). Copyright (c) 2020 Storozhenko Roman
 * @brief		This task create start screen
 */
#ifndef settingTSK_H
#define settingTSK_H

/*!****************************************************************************
 * Include
 */
#include <stdint.h>
#include "menuSystem.h"
#include "time.h"

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
	uint32_t u;
	uint32_t i;
	uint16_t dacu;
	uint16_t daci;
	uint16_t adcu;
	uint16_t adci;
	uint32_t measu;
	uint32_t measi;
	uint8_t calibratePoint;
	time_t unixTime;	//At 00:00 hours, Jan 1, 1970 UTC
} settingSct_type;

/*!****************************************************************************
 * Exported variables
 */
extern settingSct_type settingSct;

/*!****************************************************************************
 * Macro functions
 */

/*!****************************************************************************
 * Function declaration
 */
void settingTSK(void *pPrm);
itemState_type PrepareU(const menuItem_type *item);
itemState_type PrepareI(const menuItem_type *item);
itemState_type savePointU(const menuItem_type *item);
itemState_type savePointI(const menuItem_type *item);
itemState_type calibrExit(const menuItem_type *item);
itemState_type setBright(const menuItem_type *item);
itemState_type rtcSelect(const menuItem_type *item);
itemState_type rtcUnselect(const menuItem_type *item);
itemState_type netUpdate(const menuItem_type *item);

#endif //settingTSK_H
/******************************** END OF FILE ********************************/

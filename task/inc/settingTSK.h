/*!****************************************************************************
 * @file    settingTSK.h
 * @author  d_el
 * @version V1.0
 * @date    01.01.2016, by d_el
 * @brief   --
 * @copyright GNU Public License
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
 * User define
 */

/*!****************************************************************************
 * User enum
 */

/*!****************************************************************************
 * User typedef
 */
typedef struct {
	uint16_t u;
	uint16_t i;
} settingSct_type;

/*!****************************************************************************
 * Extern viriables
 */

/*!****************************************************************************
 * Macro functions
 */

/*!****************************************************************************
 * Prototypes for the functions
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
/*************** GNU GPL ************** END OF FILE ********* D_EL ***********/

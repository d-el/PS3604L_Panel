/*!****************************************************************************
 * @file		settingTSK.c
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		01.01.2016
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		This task create start screen
 */

/*!****************************************************************************
 * Include
 */
#include <FreeRTOS.h>
#include <task.h>
#include "pstypes.h"
#include "rtc.h"
#include "main.h"
#include "menuSystem.h"
#include "enco.h"
#include "netif.h"
#include "ledpwm.h"
#include "regulatorConnTSK.h"
#include "settingTSK.h"
#include "systemTSK.h"

/*!****************************************************************************
 * MEMORY
 */
settingSct_type settingSct;
uint8_t calibratePoint;
time_t 		unixTime;	//At 00:00 hours, Jan 1, 1970 UTC

/*!****************************************************************************
 * @brief    Setting system task
 */
void settingTSK(void *pPrm){
	(void)pPrm;
	menuEngine(mN_voltmeter);
	prm_store(SYSEEPADR, prmEepSys);
	selWindow(baseWindow);
	while(1){
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}

/*!****************************************************************************
 * @brief    setup regulator to mode_raw
 */
itemState_type PrepareU(const menuItem_type *item){
	static uint32_t pointU[4] = { 10000, 100000, 19000000, 30000000 };
	calibratePoint = item->flags.bit.pfParamert;
	fp.tf.task.daci = 4095;
	fp.tf.task.dacu = 0;
	fp.tf.task.u = pointU[calibratePoint];
	fp.tf.task.mode = mode_raw;
	sendCommand(setSwitchOn);
	if(waitForTf() != 0){
		return (itemState_type) {.state = menuItemError, .string = "No Connect"};
	}else{
		return (itemState_type) {.state = menuItemOk};
	}
}

/*!****************************************************************************
 * @brief    setup regulator to mode_raw
 */
itemState_type PrepareI(const menuItem_type *item){
	static uint32_t pointI[4] = { 10000, 100000, 1500000, 3000000 };
	calibratePoint = item->flags.bit.pfParamert;
	fp.tf.task.daci = 0;
	fp.tf.task.dacu = 4095;
	fp.tf.task.i = pointI[calibratePoint];
	fp.tf.task.mode = mode_raw;
	sendCommand(setSwitchOn);
	if(waitForTf() != 0){
		return (itemState_type) {.state = menuItemError, .string = "No Connect"};
	}else{
		return (itemState_type) {.state = menuItemOk};
	}
}

/*!****************************************************************************
 * @brief
 */
itemState_type savePointU(const menuItem_type *item){
	(void)item;
	sendCommand(setSavePointU0 + calibratePoint);
	if(waitForTf() != 0){
		return (itemState_type) {.state = menuItemError, .string = "No Connect"};
	}else{
		return (itemState_type) {.state = menuItemOk};
	}
}

/*!****************************************************************************
 * @brief
 */
itemState_type savePointI(const menuItem_type *item){
	(void)item;
	sendCommand(setSavePointI0 + calibratePoint);
	if(waitForTf() != 0){
		return (itemState_type) {.state = menuItemError, .string = "No Connect"};
	}else{
		return (itemState_type) {.state = menuItemOk};
	}
}

/*!****************************************************************************
 * @brief    setup regulator to mode_off
 */
itemState_type regSave(const menuItem_type *item){
	(void)item;
	sendCommand(setSaveSettings);
	fp.tf.task.mode = mode_off;
	if(waitForTf() != 0){
		return (itemState_type) {.state = menuItemError, .string = "No Connect"};
	}else{
		return (itemState_type) {.state = menuItemOk};
	}
}

/*!****************************************************************************
 * @brief    set LCD Bright callback
 */
itemState_type setBright(const menuItem_type *item){
	setLcdBrightness(item->prmHandle->prm->t_u16Frmt);
	return (itemState_type) {.state = menuItemOk};
}

/*!****************************************************************************
 * @brief    RTC select callback
 */
itemState_type rtcSelect(const menuItem_type *item){
	(void)item;
	unixTime = time(NULL) + (3600 * fp.fpSet.timezone);

	return (itemState_type) {.state = menuItemOk};
}

/*!****************************************************************************
 * @brief    RTC select callback
 */
itemState_type rtcUnselect(const menuItem_type *item){
	(void)item;
	timezoneUpdate();
	rtc_setTimeUnix(unixTime - (3600 * fp.fpSet.timezone));
	return (itemState_type) {.state = menuItemOk};
}

/*!****************************************************************************
 * @brief    NET pfUnselect
 */
itemState_type netUpdate(const menuItem_type *item){
	(void)item;
	netSettingUpdate();
	return (itemState_type) {.state = menuItemOk};
}

/******************************** END OF FILE ********************************/

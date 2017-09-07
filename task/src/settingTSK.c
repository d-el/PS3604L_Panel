/*!****************************************************************************
 * @file    settingTSK.c
 * @author  d_el
 * @version V1.0
 * @date    01.01.2016, by d_el
 * @brief   --
 * @copyright GNU Public License
 */

/*!****************************************************************************
 * Include
 */
#include "settingTSK.h"

/*!****************************************************************************
 * MEMORY
 */
settingSct_type settingSct;
uint8_t calibratePoint;

/*!****************************************************************************
 * @brief    Setting system task
 */
void settingTSK(void *pPrm){
	enSetNtic(0);

	menuEngine(&item_voltmeter);
	selWindow(baseWindow);
	while(1){
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}

/*!****************************************************************************
 * @brief    setup regulator to mode_raw
 */
uint32_t PrepareU(const menuItem_type *item){
	static uint32_t pointU[4] = { 10000, 100000, 19000000, 30000000 };
	calibratePoint = item->flags.bit.pfParamert;
	fp.tf.task.daci = 4095;
	fp.tf.task.dacu = 0;
	fp.tf.task.u = pointU[calibratePoint];
	fp.tf.task.mode = mode_raw;
	sendCommand(setSwitchOn);
	if(waitForTf() != 0){
		return 1;
	}else{
		return 0;
	}
}

/*!****************************************************************************
 * @brief    setup regulator to mode_raw
 */
uint32_t PrepareI(const menuItem_type *item){
	static uint32_t pointI[4] = { 10000, 100000, 1500000, 3000000 };
	calibratePoint = item->flags.bit.pfParamert;
	fp.tf.task.daci = 0;
	fp.tf.task.dacu = 4095;
	fp.tf.task.i = pointI[calibratePoint];
	fp.tf.task.mode = mode_raw;
	sendCommand(setSwitchOn);
	if(waitForTf() != 0){
		return 1;
	}else{
		return 0;
	}
}

/*!****************************************************************************
 * @brief
 */
uint32_t savePointU(const menuItem_type *item){
	sendCommand(setSavePointU0 + calibratePoint);
	if(waitForTf() != 0){
		return 1;
	}else{
		return 0;
	}
}

/*!****************************************************************************
 * @brief
 */
uint32_t savePointI(const menuItem_type *item){
	sendCommand(setSavePointI0 + calibratePoint);
	if(waitForTf() != 0){
		return 1;
	}else{
		return 0;
	}
}

/*!****************************************************************************
 * @brief    setup regulator to mode_off
 */
uint32_t regSave(const menuItem_type *item){
	sendCommand(setSaveSettings);
	fp.tf.task.mode = mode_off;
	if(waitForTf() != 0){
		return 1;
	}else{
		return 0;
	}
}

/*!****************************************************************************
 * @brief    set LCD Bright callback
 */
uint32_t setBright(const menuItem_type *item){
	setLcdBrightness(item->prmHandle->prm->_u16Frmt);
	return 0;
}

/*!****************************************************************************
 * @brief    RTC periodic callback
 */
uint32_t rtcPeriodic(const menuItem_type *item){
	unixTime = time(NULL);
	return 0;
}

/*!****************************************************************************
 * @brief    RTC select callback
 */
uint32_t rtcSelectc(const menuItem_type *item){
	unixTime = time(NULL);
	gmtime_r(&unixTime, &timeStrct);
	return 0;
}

/*!****************************************************************************
 * @brief    RTC change callback
 */
uint32_t rtcCh(const menuItem_type *item){
	unixTime = mktime(&timeStrct);
	rtc_setTimeUnix(unixTime);
	return 0;
}

/*************** GNU GPL ************** END OF FILE ********* D_EL ***********/

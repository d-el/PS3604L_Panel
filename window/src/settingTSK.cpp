/*!****************************************************************************
 * @file		settingTSK.c
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		01.01.2016
 * @copyright	The MIT License (MIT). Copyright (c) 2020 Storozhenko Roman
 * @brief		This task create start screen
 */

/*!****************************************************************************
 * Include
 */
#include <FreeRTOS.h>
#include <task.h>
#include <rtc.h>
#include <enco.h>
#include <lwip/netif.h>
#include <ledpwm.h>
#include <regulatorConnTSK.h>
#include <systemTSK.h>
#include "settingTSK.h"
#include "menuSystem.h"

using namespace Menu;

extern const MenuItem m1;
extern const MenuItem m2;
extern const MenuItem subm1;
extern const MenuItem subm2;
extern const MenuItem subm3;
extern const MenuItem subm4;
extern const MenuItem m3;

const MenuItem m0("m0", &Prm::brightness, true, 0, nullptr, nullptr, nullptr, nullptr, &m1, nullptr, nullptr, nullptr);
const MenuItem m1("m1", &Prm::brightness, true, 0, nullptr, nullptr, nullptr, nullptr, &m2, &m0, nullptr, &subm1);
	const MenuItem subm1("subm1", &Prm::brightness, false, 0, nullptr, nullptr, nullptr, nullptr, &subm2, nullptr, &m1, nullptr);
	const MenuItem subm2("subm2", &Prm::brightness, true, 0, nullptr, nullptr, nullptr, nullptr, &subm3, &subm1, &m1, nullptr);
	const MenuItem subm3("subm3", &Prm::brightness, false, 0, nullptr, nullptr, nullptr, nullptr, &subm4, &subm2, &m1, nullptr);
	const MenuItem subm4("subm4", &Prm::brightness, true, 0, nullptr, nullptr, nullptr, nullptr, nullptr, &subm3, &m1, nullptr);
const MenuItem m2("m2", &Prm::brightness, true, 0, nullptr, nullptr, nullptr, nullptr, &m3, &m1, nullptr, nullptr);
const MenuItem m3("m3", &Prm::brightness, true, 0, nullptr, nullptr, nullptr, nullptr, nullptr, &m2, nullptr, nullptr);


/*!****************************************************************************
 * @brief    Setting system task
 */
void settingTSK(void *pPrm){
	(void)pPrm;
	run(&m0);
	saveparameters();
	selWindow(baseWindow);
	vTaskSuspend(NULL);
}

/*!****************************************************************************
 * @brief    setup regulator to mode_raw
 */
//itemState_type PrepareU(const menuItem_type *item){
//	static uint32_t pointU[4] = { 10000, 100000, 19000000, 30000000 };
//	settingSct.calibratePoint = item->flags.bit.pfParamert;
//	settingSct.u = pointU[settingSct.calibratePoint];
//	bool regstate = reg_setDacCurrent(2048);
//	reg_setDacVoltage(settingSct.u);
//	reg_setMode(reg_raw);
//	reg_setEnable(true);
//	if(regstate){
//		return (itemState_type) {.state = menuItemOk};
//	}else{
//		return (itemState_type) {.state = menuItemError, .string = "No Connect"};
//	}
//}

/*!****************************************************************************
 * @brief    setup regulator to mode_raw
 */
//itemState_type PrepareI(const menuItem_type *item){
//	static uint32_t pointI[4] = { 10000, 100000, 1500000, 3000000 };
//	settingSct.calibratePoint = item->flags.bit.pfParamert;
//	settingSct.i = pointI[settingSct.calibratePoint];
//	reg_setDacCurrent(settingSct.i);
//	bool regstate = reg_setDacVoltage(2048);
//	reg_setMode(reg_raw);
//	reg_setEnable(true);
//	if(regstate){
//		return (itemState_type) {.state = menuItemOk};
//	}else{
//		return (itemState_type) {.state = menuItemError, .string = "No Connect"};
//	}
//}

/*!****************************************************************************
 * @brief
 */
//itemState_type savePointU(const menuItem_type *item){
//	(void)item;
//	reg_setVoltagePoint(settingSct.u, settingSct.calibratePoint);
//	reg_setDacVoltage(settingSct.dacu);
//	regMeas_t regmeas;
//	bool regstate = reg_getState(&regmeas);
//	if(regstate){
//		settingSct.adci = regmeas.iadc;
//		settingSct.adcu = regmeas.vadc;
//		settingSct.measu = regmeas.voltage;
//		return (itemState_type) {.state = menuItemOk};
//	}else{
//		return (itemState_type) {.state = menuItemError, .string = "No Connect"};
//	}
//}

/*!****************************************************************************
 * @brief
 */
//itemState_type savePointI(const menuItem_type *item){
//	(void)item;
//	reg_setCurrentPoint(settingSct.i, settingSct.calibratePoint);
//	reg_setDacCurrent(settingSct.daci);
//	regMeas_t regmeas;
//	bool regstate = reg_getState(&regmeas);
//	if(regstate){
//		settingSct.adci = regmeas.iadc;
//		settingSct.adcu = regmeas.vadc;
//		settingSct.measi = regmeas.current;
//		return (itemState_type) {.state = menuItemOk};
//	}else{
//		return (itemState_type) {.state = menuItemError, .string = "No Connect"};
//	}
//}

/*!****************************************************************************
 * @brief
 */
//itemState_type calibrExit(const menuItem_type *item){
//	(void)item;
//	bool regstate = reg_setEnable(false);
//	if(regstate){
//		return (itemState_type) {.state = menuItemOk};
//	}else{
//		return (itemState_type) {.state = menuItemError, .string = "No Connect"};
//	}
//}

/*!****************************************************************************
 * @brief    set LCD Bright callback
 */
//itemState_type setBright(const menuItem_type *item){
//	setLcdBrightness(item->prmHandle->prm->t_u16Frmt);
//	return (itemState_type) {.state = menuItemOk};
//}

/*!****************************************************************************
 * @brief    RTC select callback
 */
//itemState_type rtcSelect(const menuItem_type *item){
//	(void)item;
//	settingSct.unixTime = time(NULL) + (3600 * fp.fpSettings.timezone);
//	return (itemState_type) {.state = menuItemOk};
//}

/*!****************************************************************************
 * @brief    RTC select callback
 */
//itemState_type rtcUnselect(const menuItem_type *item){
//	(void)item;
//	timezoneUpdate();
//	rtc_setTimeUnix(settingSct.unixTime - (3600 * fp.fpSettings.timezone));
//	return (itemState_type) {.state = menuItemOk};
//}

/*!****************************************************************************
 * @brief    NET pfUnselect
 */
//itemState_type netUpdate(const menuItem_type *item){
//	(void)item;
//	netSettingUpdate();
//	return (itemState_type) {.state = menuItemOk};
//}

/******************************** END OF FILE ********************************/

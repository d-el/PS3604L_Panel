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

/*!****************************************************************************
 * @brief    set LCD Bright callback
 */
ItemState setBright(const MenuItem* m){
	(void)m;
	setLcdBrightness(Prm::brightness.val * 10);
	return ItemState{ true, "" };
}

/*!****************************************************************************
 * @brief    RTC select callback
 */
ItemState rtcSelect(const MenuItem* m){
	(void)m;
	Prm::utcTime.val = time(NULL) + (3600 * Prm::timezone.val);
	return ItemState { true, "" };
}

/*!****************************************************************************
 * @brief    RTC select callback
 */
ItemState rtcUnselect(const MenuItem* m){
	(void)m;
	timezoneUpdate(Prm::timezone.val);
	rtc_setTimeUnix(Prm::utcTime.val - (3600 * Prm::timezone.val));
	return ItemState { true, "" };
}

/*!****************************************************************************
 * @brief    setup regulator
 */
ItemState PrepareU(const MenuItem* m){
	static uint32_t pointU[4] = { 10000, 100000, 19000000, 30000000 };
	Prm::point.val = m->arg;
	Prm::Ureal.val = pointU[Prm::point.val];
	bool regstate = reg_setDacCurrent(2048);
	reg_setDacVoltage(Prm::Ureal.val);
	reg_setMode(reg_raw);
	reg_setEnable(true);
	if(regstate){
		return ItemState { true, "" };
	}else{
		return ItemState { true, "error connect" };
	}
}

/*!****************************************************************************
 * @brief    setup regulator
 */
ItemState PrepareI(const MenuItem* m){
	static uint32_t pointI[4] = { 10000, 100000, 1500000, 3000000 };
	Prm::point.val = m->arg;
	Prm::Ireal.val = pointI[Prm::point.val];
	reg_setDacCurrent(Prm::Ireal.val);
	bool regstate = reg_setDacVoltage(2048);
	reg_setMode(reg_raw);
	reg_setEnable(true);
	if(regstate){
		return ItemState { true, "" };
	}else{
		return ItemState { true, "error connect" };
	}
}

/*!****************************************************************************
 * @brief
 */
ItemState updateReg(const MenuItem* m){
	(void)m;
	regMeas_t regmeas;
	bool regstate = reg_getState(&regmeas);
	if(regstate){
		Prm::Iadc.val = regmeas.iadc;
		Prm::Uadc.val = regmeas.vadc;
		Prm::Umeas.val = regmeas.voltage;
		Prm::Imeas.val = regmeas.current;
		return ItemState { true, "" };
	}else{
		return ItemState { false, "error connect"};
	}
}

/*!****************************************************************************
 * @brief
 */
ItemState savePointU(const MenuItem* m){
	(void)m;
	reg_setVoltagePoint(Prm::Ureal.val, Prm::point.val);
	reg_setDacVoltage(Prm::Udac.val);
	return ItemState { true, "" };
}

/*!****************************************************************************
 * @brief
 */
ItemState savePointI(const MenuItem* m){
	(void)m;
	reg_setCurrentPoint(Prm::Ireal.val, Prm::point.val);
	reg_setDacCurrent(Prm::Idac.val);
	return ItemState { true, "" };
}

/*!****************************************************************************
 * @brief    NET pfUnselect
 */
ItemState netUpdate(const MenuItem* m){
	(void)m;
	netSettingUpdate();
	return ItemState { true, "" };
}

/*!****************************************************************************
 * @brief
 */
ItemState calibrExit(const MenuItem* m){
	(void)m;
	bool regstate = reg_setEnable(false);
	if(regstate){
		return ItemState { true, "" };
	}else{
		return ItemState { false, "error connect"};
	}
}

extern const MenuItem m1;
	extern const MenuItem m00;
	extern const MenuItem m01;
	extern const MenuItem m02;
	extern const MenuItem m03;
		extern const MenuItem m000;
		extern const MenuItem m001;
		extern const MenuItem m002;
		extern const MenuItem m003;
		extern const MenuItem m004;

	extern const MenuItem m10;
	extern const MenuItem m11;
	extern const MenuItem m12;
	extern const MenuItem m13;
		extern const MenuItem m100;
		extern const MenuItem m101;
		extern const MenuItem m102;
		extern const MenuItem m103;
		extern const MenuItem m104;

extern const MenuItem m2;
	extern const MenuItem m20;
	extern const MenuItem m21;

extern const MenuItem m3;
	extern const MenuItem m30;
	extern const MenuItem m31;
	extern const MenuItem m32;
extern const MenuItem m4;

const MenuItem m0("Vmeter", nullptr, true, 0, nullptr, nullptr, nullptr, nullptr, &m1, nullptr, &m00);
	const MenuItem m00("Point1", nullptr, true, 0, nullptr, PrepareU, calibrExit, nullptr, &m01, nullptr, &m000);
	const MenuItem m01("Point2", nullptr, true, 1, nullptr, PrepareU, calibrExit, nullptr, &m02, &m00, &m000);
	const MenuItem m02("Point3", nullptr, true, 2, nullptr, PrepareU, calibrExit, nullptr, &m03, &m01, &m000);
	const MenuItem m03("Point4", nullptr, true, 3, nullptr, PrepareU, calibrExit, nullptr, nullptr, &m02, &m000);
		const MenuItem m000("Ureal", &Prm::Ureal, true, 0, savePointU, nullptr, nullptr, updateReg, &m001, nullptr, nullptr);
		const MenuItem m001("DacU", &Prm::Udac, true, 0, savePointU, nullptr, nullptr, updateReg, &m002, &m000, nullptr);
		const MenuItem m002("AdcU", &Prm::Uadc, false, 0, savePointU, nullptr, nullptr, updateReg, &m003, &m001, nullptr);
		const MenuItem m003("AdcI", &Prm::Iadc, false, 0, savePointU, nullptr, nullptr, updateReg, &m004, &m002, nullptr);
		const MenuItem m004("Umeas", &Prm::Umeas, false, 0, savePointU, nullptr, nullptr, updateReg, nullptr, &m003, nullptr);

const MenuItem m1("Ameter", nullptr, true, 0, nullptr, nullptr, nullptr, nullptr, &m2, &m0, &m10);
	const MenuItem m10("Point1", nullptr, true, 0, nullptr, PrepareI, calibrExit, nullptr, &m11, nullptr, &m100);
	const MenuItem m11("Point2", nullptr, true, 1, nullptr, PrepareI, calibrExit, nullptr, &m12, &m10, &m100);
	const MenuItem m12("Point3", nullptr, true, 2, nullptr, PrepareI, calibrExit, nullptr, &m13, &m11, &m100);
	const MenuItem m13("Point4", nullptr, true, 3, nullptr, PrepareI, calibrExit, nullptr, nullptr, &m12, &m100);
		const MenuItem m100("Ireal", &Prm::Ireal, true, 0, savePointI, nullptr, nullptr, nullptr, &m101, nullptr, nullptr);
		const MenuItem m101("DacI", &Prm::Idac, true, 0, savePointI, nullptr, nullptr, updateReg, &m102, &m100, nullptr);
		const MenuItem m102("AdcU", &Prm::Uadc, false, 0, savePointI, nullptr, nullptr, updateReg, &m103, &m101, nullptr);
		const MenuItem m103("AdcI", &Prm::Iadc, false, 0, savePointI, nullptr, nullptr, updateReg, &m104, &m102, nullptr);
		const MenuItem m104("Imeas", &Prm::Imeas, false, 0, savePointI, nullptr, nullptr, updateReg, nullptr, &m103, nullptr);

const MenuItem m2("Date & Time", nullptr, true, 0, nullptr, nullptr, nullptr, nullptr, &m3, &m1, &m20);
	const MenuItem m20("Clock", &Prm::utcTime, true, 0, nullptr, rtcSelect, rtcUnselect, nullptr, &m21, nullptr, nullptr, clockEditor);
	const MenuItem m21("Time Zone", &Prm::timezone, true, 0, nullptr, nullptr, nullptr, nullptr, nullptr, &m20, nullptr);

const MenuItem m3("LAN", nullptr, true, 0, nullptr, nullptr, netUpdate, nullptr, &m4, &m2, &m30);
	const MenuItem m30("IP address", &Prm::ipadr, true, 0, nullptr, nullptr, nullptr, nullptr, &m31, nullptr, nullptr, ipAddressEditor);
	const MenuItem m31("subnet mask", &Prm::netmask, true, 0, nullptr, nullptr, nullptr, nullptr, &m32, &m30, nullptr, ipAddressEditor);
	const MenuItem m32("gateway", &Prm::gateway, true, 0, nullptr, nullptr, nullptr, nullptr, nullptr, &m31, nullptr, ipAddressEditor);

const MenuItem m4("Bright", &Prm::brightness, true, 0, setBright, nullptr, nullptr, nullptr, nullptr, &m3, nullptr);

/*!****************************************************************************
 * @brief    Setting system task
 */
void settingTSK(void *pPrm){
	(void)pPrm;
	run(&m0);
	saveparametersSystem();
	selWindow(baseWindow);
	vTaskSuspend(NULL);
}

/******************************** END OF FILE ********************************/

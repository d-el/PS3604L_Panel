/*!****************************************************************************
 * @file		settingTSK.c
 * @author		d_el - Storozhenko Roman
 * @version		V1.1
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
 * @brief    set LCD Bright callback
 */
ItemState updateDST(const MenuItem* m){
	(void)m;
	timezoneUpdate(Prm::timezone.val);
	return ItemState { true, "" };
}

/*!****************************************************************************
 * @brief    Calibrate date select callback
 */
ItemState calibrDateSelect(const MenuItem* m){
	(void)m;
	time_t time = 0;
	reg_getCalibrationTime(&time);
	Prm::utcTime.val = time;
	return ItemState { true, "" };
}

/*!****************************************************************************
 * @brief    Calibrate date unselect callback
 */
ItemState calibrDateUnselect(const MenuItem* m){
	(void)m;

	return ItemState { true, "" };
}

/*!****************************************************************************
 * @brief    RTC select callback
 */
ItemState rtcSelect(const MenuItem* m){
	(void)m;
	Prm::utcTime.val = time(NULL);
	return ItemState { true, "" };
}

/*!****************************************************************************
 * @brief    RTC select callback
 */
ItemState rtcUnselect(const MenuItem* m){
	(void)m;
	rtc_setTimeUnix(Prm::utcTime.val);
	return ItemState { true, "" };
}

/*!****************************************************************************
 * @brief    setup regulator
 */
ItemState PrepareU(const MenuItem* m){
	static uint32_t pointU[6] = { 10'000, 8000'000, 16'000'000, 22'000'000, 30'000'000, 35'000'000 }; // uV
	Prm::point.val = m->arg;
	Prm::Ureal.val = pointU[Prm::point.val];
	Prm::Udac.val = 0;
	int32_t dacMaxValue = 0;
	bool result = reg_getDacMaxValue(&dacMaxValue);
	if(result){
		Prm::Idac.val = dacMaxValue / 4;
		reg_setMode(reg_raw);
		reg_setEnable(true);
		reg_setDacCurrent(Prm::Idac.val);
		result = reg_setDacVoltage(Prm::Udac.val);
	}
	if(result){
		return ItemState { true, "" };
	}else{
		return ItemState { false, "error connect" };
	}
}

/*!****************************************************************************
 * @brief    setup regulator
 */
ItemState PrepareMicroI(const MenuItem* m){
	static uint32_t pointI[4] = { 10, 950, 50000 }; // uA
	Prm::point.val = m->arg;
	Prm::Ireal.val = pointI[Prm::point.val];
	int32_t dacMaxValue = 0;
	bool result = reg_getDacMaxValue(&dacMaxValue);
	if(result){
		Prm::Idac.val = dacMaxValue / 32;
		Prm::Udac.val = 0;
		reg_setMode(reg_raw);
		reg_setEnable(true);
		reg_setDacCurrent(Prm::Idac.val);
		result = reg_setDacVoltage(Prm::Udac.val);
	}
	if(result){
		return ItemState { true, "" };
	}else{
		return ItemState { false, "error connect" };
	}
}

/*!****************************************************************************
 * @brief    setup regulator
 */
ItemState PrepareI(const MenuItem* m){
	static uint32_t pointI[6] = { 1000, 10'000, 500'000, 2'200'000, 3'000'000, 3'500'000 }; // uA
	Prm::point.val = m->arg;
	Prm::Ireal.val = pointI[Prm::point.val];
	int32_t dacMaxValue = 0;
	bool result = reg_getDacMaxValue(&dacMaxValue);
	if(result){
		Prm::Udac.val = dacMaxValue / 4;
		Prm::Idac.val = 0;
		reg_setMode(reg_raw);
		reg_setEnable(true);
		reg_setDacCurrent(Prm::Idac.val);
		result = reg_setDacVoltage(Prm::Udac.val);
	}
	if(result){
		return ItemState { true, "" };
	}else{
		return ItemState { false, "error connect" };
	}
}

/*!****************************************************************************
 * @brief    setup regulator
 */
ItemState PrepareWireResistance(const MenuItem* m){
	(void)m;
	reg_setMode(reg_limitation);
	reg_setEnable(true);
	reg_setCurrent(3000000);
	bool regstate = reg_setVoltage(5000000);
	if(regstate){
		return ItemState { true, "" };
	}else{
		return ItemState { false, "error connect" };
	}
}

/*!****************************************************************************
 * @brief    setup regulator
 */
ItemState prepareInfo(const MenuItem* m){
	(void)m;
	bool regstate = reg_getSerial(&Prm::reg_serial.val);
	if(regstate){
		return ItemState { true, "" };
	}else{
		return ItemState { false, "error connect" };
	}
}

/*!****************************************************************************
 * @brief
 */
ItemState updateReg(const MenuItem* m){
	(void)m;
	regState_t regmeas;
	bool regstate = reg_getState(&regmeas);
	if(regstate){
		Prm::Iadc.val = regmeas.iadc;
		Prm::Uadc.val = regmeas.vadc;
		Prm::Umeas.val = regmeas.voltage;
		Prm::Imeas.val = regmeas.current;
		Prm::resistance = regmeas.resistance;
		Prm::Vdc = regmeas.input_voltage / 100000;
		Prm::temp_heatsink = regmeas.temp_heatsink;
		Prm::temp_shunt = regmeas.temp_shunt;
		Prm::temp_ref = regmeas.temp_ref;
		return ItemState { true, "" };
	}else{
		return ItemState { false, "error connect"};
	}
}

/*!****************************************************************************
 * @brief
 */
ItemState saveSettings(const MenuItem* m){
	(void)m;

	Prm::save_settings.val = Prm::save_do;
	if(reg_setSaveSettings(reg_save_do)){
		return ItemState { true, "" };
	}else{
		return ItemState { false, "error connect"};
	}
}

/*!****************************************************************************
 * @brief
 */
ItemState updateSaveSettings(const MenuItem* m){
	(void)m;

	regSave_t save;
	if(reg_getSaveSettings(&save)){
		Prm::save_settings.val = save;
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
	reg_setDacVoltage(Prm::Udac.val);
	reg_setVoltagePoint(Prm::Ureal.val, Prm::point.val);
	return ItemState { true, "" };
}

/*!****************************************************************************
 * @brief
 */
ItemState savePointMicroI(const MenuItem* m){
	(void)m;
	reg_setDacVoltage(Prm::Udac.val);
	reg_setMicroCurrentPoint(Prm::Ireal.val, Prm::point.val);
	return ItemState { true, "" };
}

/*!****************************************************************************
 * @brief
 */
ItemState setCrange(const MenuItem* m){
	(void)m;
	reg_setCrange((regCrange_t)Prm::crange_set.val);
	return ItemState { true, "" };
}

/*!****************************************************************************
 * @brief
 */
ItemState savePointI(const MenuItem* m){
	(void)m;
	reg_setDacCurrent(Prm::Idac.val);
	reg_setCurrentPoint(Prm::Ireal.val, Prm::point.val);
	return ItemState { true, "" };
}

/*!****************************************************************************
 * @brief
 */
ItemState setWireresistanse(const MenuItem* m){
	(void)m;
	reg_setWireResistance(Prm::wirecompensateOnOff.val ? Prm::wireResistance.val : 0);
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

extern const MenuItem
m1,
	m11,
		m110,
		m111,
		m112,
		m113,
		m114,
		m115,
			m1130,
			m1131,
			m1132,
			m1133,
			m1134,
	m12,
		m120,
		m121,
		m122,
			m1230,
			m1231,
			m1232,
			m1233,
			m1234,
			m1235,
	m13,
		m130,
		m131,
		m132,
		m133,
		m134,
		m135,
			m1330,
			m1331,
			m1332,
			m1333,
			m1334,
			m1335,
	m14,
	m15,
		m150,
		m151,
		m152,
	m16,
		m160,
		m161,
		m162,
		m163,
		m164,
	m17,
	m18,

m2,
	m20,
	m21,
	m22,
	m23,
		m230,
		m231,
		m232,
		m233,
		m234,
		m235,
	m24,
		m240,
		m241,
		m242,
		m243,
		m244,
		m245,
m3,
	m30,
	m31,
	m32,
	m33,
m4;

const MenuItem
m1("Regulator", nullptr, true, 0, nullptr, nullptr, nullptr, nullptr, &m2, nullptr, &m11),
	m11("Vmeter", nullptr, true, 0, nullptr, nullptr, nullptr, nullptr, &m12, nullptr, &m110),
		m110("Point0", nullptr, true, 0, nullptr, PrepareU, calibrExit, nullptr, &m111, nullptr, &m1130),
		m111("Point1", nullptr, true, 1, nullptr, PrepareU, calibrExit, nullptr, &m112, &m110, &m1130),
		m112("Point2", nullptr, true, 2, nullptr, PrepareU, calibrExit, nullptr, &m113, &m111, &m1130),
		m113("Point3", nullptr, true, 3, nullptr, PrepareU, calibrExit, nullptr, &m114, &m112, &m1130),
		m114("Point4", nullptr, true, 4, nullptr, PrepareU, calibrExit, nullptr, &m115, &m113, &m1130),
		m115("Point5", nullptr, true, 5, nullptr, PrepareU, calibrExit, nullptr, nullptr, &m114, &m1130),
			m1130("Ureal", &Prm::Ureal, true, 0, savePointU, nullptr, nullptr, updateReg, &m1131, nullptr),
			m1131("DacU", &Prm::Udac, true, 0, savePointU, nullptr, nullptr, updateReg, &m1132, &m1130),
			m1132("AdcU", &Prm::Uadc, false, 0, savePointU, nullptr, nullptr, updateReg, &m1133, &m1131),
			m1133("AdcI", &Prm::Iadc, false, 0, savePointU, nullptr, nullptr, updateReg, &m1134, &m1132),
			m1134("Umeas", &Prm::Umeas, false, 0, savePointU, nullptr, nullptr, updateReg, nullptr, &m1133),

	m12("MicroAmeter", nullptr, true, 0, nullptr, nullptr, nullptr, nullptr, &m13, &m11, &m120),
		m120("Point1", nullptr, true, 0, nullptr, PrepareMicroI, calibrExit, nullptr, &m121, nullptr, &m1230),
		m121("Point2", nullptr, true, 1, nullptr, PrepareMicroI, calibrExit, nullptr, &m122, &m120, &m1230),
		m122("Point3", nullptr, true, 2, nullptr, PrepareMicroI, calibrExit, nullptr, nullptr, &m121, &m1230),
			m1230("Ireal", &Prm::Ireal, true, 0, savePointMicroI, nullptr, nullptr, updateReg, &m1231, nullptr),
			m1231("DacU", &Prm::Udac, true, 0, savePointMicroI, nullptr, nullptr, updateReg, &m1232, &m1230),
			m1232("AdcU", &Prm::Uadc, false, 0, nullptr, nullptr, nullptr, updateReg, &m1233, &m1231),
			m1233("AdcI", &Prm::Iadc, false, 0, nullptr, nullptr, nullptr, updateReg, &m1234, &m1232),
			m1234("Imeas", &Prm::Imeas, false, 0, nullptr, nullptr, nullptr, updateReg, &m1235, &m1233),
			m1235("cRange", &Prm::crange_set, true, 0, setCrange, nullptr, nullptr, updateReg, nullptr, &m1234),

	m13("Ameter", nullptr, true, 0, nullptr, nullptr, nullptr, nullptr, &m14, &m12, &m130),
		m130("Point0", nullptr, true, 0, nullptr, PrepareI, calibrExit, nullptr, &m131, nullptr, &m1330),
		m131("Point1", nullptr, true, 1, nullptr, PrepareI, calibrExit, nullptr, &m132, &m130, &m1330),
		m132("Point2", nullptr, true, 2, nullptr, PrepareI, calibrExit, nullptr, &m133, &m131, &m1330),
		m133("Point3", nullptr, true, 3, nullptr, PrepareI, calibrExit, nullptr, &m134, &m132, &m1330),
		m134("Point4", nullptr, true, 4, nullptr, PrepareI, calibrExit, nullptr, &m135, &m133, &m1330),
		m135("Point5", nullptr, true, 5, nullptr, PrepareI, calibrExit, nullptr, nullptr, &m134, &m1330),
			m1330("Ireal", &Prm::Ireal, true, 0, savePointI, nullptr, nullptr, updateReg, &m1331, nullptr),
			m1331("DacI", &Prm::Idac, true, 0, savePointI, nullptr, nullptr, updateReg, &m1332, &m1330),
			m1332("AdcU", &Prm::Uadc, false, 0, savePointI, nullptr, nullptr, updateReg, &m1333, &m1331),
			m1333("AdcI", &Prm::Iadc, false, 0, savePointI, nullptr, nullptr, updateReg, &m1334, &m1332),
			m1334("AdcIEx", &Prm::IadcEx, false, 0, savePointI, nullptr, nullptr, updateReg, &m1335, &m1333),
			m1335("Imeas", &Prm::Imeas, false, 0, savePointI, nullptr, nullptr, updateReg, nullptr, &m1334),

	m14("CalibrationTime", &Prm::utcTime, false, 0, nullptr, calibrDateSelect, calibrDateUnselect, nullptr, &m15, &m13, nullptr, clockEditor),

	m15("Wire", nullptr, false, 0, nullptr, PrepareWireResistance, calibrExit, nullptr, &m16, &m14, &m150),
		m150("Compensation", &Prm::wirecompensateOnOff, true, 0, setWireresistanse, nullptr, nullptr, updateReg, &m151, nullptr),
		m151("Wire", &Prm::wireResistance, true, 0, setWireresistanse, nullptr, nullptr, updateReg, &m152, &m150),
		m152("Res", &Prm::resistance, false, 0, nullptr, nullptr, nullptr, updateReg, nullptr, &m151),

	m16("cRange", &Prm::crange_set, true, 0, setCrange, nullptr, nullptr, updateReg, &m17, &m15),

	m17("Info", nullptr, false, 0, nullptr, prepareInfo, nullptr, nullptr, &m18, &m16, &m160),
		m160("Serial", &Prm::reg_serial, false, 0, nullptr, nullptr, nullptr, updateReg, &m161, nullptr),
		m161("Vdc", &Prm::Vdc, false, 0, nullptr, nullptr, nullptr, updateReg, &m162, &m160),
		m162("Ths", &Prm::temp_heatsink, false, 0, nullptr, nullptr, nullptr, updateReg, &m163, &m161),
		m163("Tsh", &Prm::temp_shunt, false, 0, nullptr, nullptr, nullptr, updateReg, &m164, &m162),
		m164("Tref", &Prm::temp_ref, false, 0, nullptr, nullptr, nullptr, updateReg, nullptr, &m163),

	m18("Save", &Prm::save_settings, true, 0, saveSettings, nullptr, nullptr, updateSaveSettings, nullptr, &m17, nullptr),

m2("Date&Time", nullptr, true, 0, nullptr, nullptr, nullptr, nullptr, &m3, &m1, &m20),
	m20("Clock", &Prm::utcTime, true, 0, nullptr, rtcSelect, rtcUnselect, nullptr, &m21, nullptr, nullptr, clockEditor),
	m21("Time Zone", &Prm::timezone, true, 0, nullptr, nullptr, nullptr, nullptr, &m22, &m20),
	m22("DST", &Prm::dstOnOff, true, 0, nullptr, nullptr, nullptr, nullptr, &m23, &m21),
	m23("Start", nullptr, true, 0, nullptr, nullptr, updateDST, nullptr, &m24, &m22, &m230),
		m230("Mon", &Prm::DSTSMon, true, 0, nullptr, nullptr, nullptr, nullptr, &m231, nullptr),
		m231("Week", &Prm::DSTSWeek, true, 0, nullptr, nullptr, nullptr, nullptr, &m232, &m230),
		m232("Day", &Prm::DSTSDay, true, 0, nullptr, nullptr, nullptr, nullptr, &m233, &m231),
		m233("Hour", &Prm::DSTSHour, true, 0, nullptr, nullptr, nullptr, nullptr, &m234, &m232),
		m234("Min", &Prm::DSTSMin, true, 0, nullptr, nullptr, nullptr, nullptr, &m235, &m233),
		m235("Sec", &Prm::DSTSSec, true, 0, nullptr, nullptr, nullptr, nullptr, nullptr, &m234),
	m24("End", nullptr, true, 0, nullptr, nullptr, updateDST, nullptr, nullptr, &m23, &m240),
		m240("Mon", &Prm::DSTEMon, true, 0, nullptr, nullptr, nullptr, nullptr, &m241, nullptr),
		m241("Week", &Prm::DSTEWeek, true, 0, nullptr, nullptr, nullptr, nullptr, &m242, &m240),
		m242("Day", &Prm::DSTEDay, true, 0, nullptr, nullptr, nullptr, nullptr, &m243, &m241),
		m243("Hour", &Prm::DSTEHour, true, 0, nullptr, nullptr, nullptr, nullptr, &m244, &m242),
		m244("Min", &Prm::DSTEMin, true, 0, nullptr, nullptr, nullptr, nullptr, &m245, &m243),
		m245("Sec", &Prm::DSTESec, true, 0, nullptr, nullptr, nullptr, nullptr, nullptr, &m244),

m3("LAN", nullptr, true, 0, nullptr, nullptr, netUpdate, nullptr, &m4, &m2, &m30),
	m30("IP address", &Prm::ipadr, true, 0, nullptr, nullptr, nullptr, nullptr, &m31, nullptr, nullptr, ipAddressEditor),
	m31("subnet mask", &Prm::netmask, true, 0, nullptr, nullptr, nullptr, nullptr, &m32, &m30, nullptr, ipAddressEditor),
	m32("gateway", &Prm::gateway, true, 0, nullptr, nullptr, nullptr, nullptr, &m33, &m31, nullptr, ipAddressEditor),
	m33("MAC", &Prm::mac0, false, 0, nullptr, nullptr, nullptr, nullptr, nullptr, &m32, nullptr, ipMacEditor),

m4("Bright", &Prm::brightness, true, 0, setBright, nullptr, nullptr, nullptr, nullptr, &m3);

/*!****************************************************************************
 * @brief    Setting system task
 */
void settingTSK(void *pPrm){
	(void)pPrm;
	run(&m1);
	saveparametersSystem();
	selWindow(baseWindow);
	vTaskSuspend(NULL);
}

/******************************** END OF FILE ********************************/

/*!****************************************************************************
 * @file		base.h
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		01.01.2015
 * @copyright	The MIT License (MIT). Copyright (c) 2020 Storozhenko Roman
 * @brief		This task is base GUI
 */

/*!****************************************************************************
 * Include
 */
#include <inttypes.h>
#include <string.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include "plog.h"
#include "ui.h"
#include "rtc.h"
#include "beep.h"
#include "key.h"
#include "display.h"
#include "graphics.h"
#include "systemTSK.h"
#include "regulatorConnTSK.h"
#include "sysTimeMeas.h"
#include "baseTSK.h"
#include <prmSystem.h>
#include <enco.h>

/******************************************************************************
 * Base task
 */

enum {
	VAR_VOLT = 0,
	VAR_CURR,
	VAR_MODE,
	VAR_NUMBER
};

void baseTSK(void *pPrm){
	(void)pPrm;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	uint8_t varParam = VAR_VOLT;
	char str[30];

	struct BaeParameter{
		union{
			struct{
				const Prm::Val<uint16_t> *voltage;
				const Prm::Val<uint16_t> *current;
				const Prm::Val<uint8_t> *mode;
			};
			Prm::IVal *p[3];
		};
	};

	const std::array<BaeParameter, 3> params = {
			&Prm::baseu0, &Prm::basei0, &Prm::basemode0,
			&Prm::baseu1, &Prm::basei1, &Prm::basemode1,
			&Prm::baseu2, &Prm::basei2, &Prm::basemode2
	};

	disp_setColor(black, red);
	disp_fillScreen(black);
	ksSet(30, 5, kUp | kDown);
	enco_settic(3);

	while(1){
		regMeas_t regmeas = {};
		reg_getState(&regmeas);
		bool regenable = false;
		reg_getEnable(&regenable);

		/**************************************
		 * Key process
		 */
		if(keyProc() != 0){
			BeepTime(ui.beep.key.time, ui.beep.key.freq);

			if(keyState(kSet)){
				varParam++;
				if(varParam >= VAR_NUMBER){
					varParam = VAR_VOLT;
				}
			}else if(keyState(kMode)){
				if(!regenable){
					selWindow(chargerWindow);
				}else{
					BeepTime(ui.beep.error.time, ui.beep.error.freq);
				}
			}else if(keyState(kView)){					//Next preset
				if(!regenable){
					if(Prm::basepreset.val < VAR_MODE)
						Prm::basepreset.val++;
					else
						Prm::basepreset.val = 0;
				}else{
					BeepTime(ui.beep.error.time, ui.beep.error.freq);
				}
			}else if(keyState(kOnOff)){
				uint8_t result = 0;
				if(!regenable){
					reg_setEnable(true);
				}else{
					reg_setEnable(false);
				}
				if(result != 0){
					disp_putStr(0, 0, &arial, 0, "Error Connect");
					vTaskDelay(1000);
				}
			}else if(keyState(kUp)){
				params[Prm::basepreset.val].p[varParam]->bigstep(1);
			}else if(keyState(kDown)){
				params[Prm::basepreset.val].p[varParam]->bigstep(-1);
			}else if(keyState(kZero)){
				params[Prm::basepreset.val].p[varParam]->setdef();
			}
		}

		/***************************************
		 * Encoder process
		 */
		params[Prm::basepreset.val].p[varParam]->step(enco_update());

		/***************************************
		 * Task for regulator
		 */

		reg_setVoltage(params[Prm::basepreset.val].voltage->val * 1000);
		uint32_t currentval = params[Prm::basepreset.val].current->val * 1000;
		switch(params[Prm::basepreset.val].mode->val){
			case Prm::mask_basemode::Imax:
				reg_setMode(reg_overcurrentShutdown);
				reg_setCurrent(currentval);
				break;

			case Prm::mask_basemode::Limiting:
				reg_setMode(reg_limitation);
				reg_setCurrent(currentval);
				break;

			case Prm::mask_basemode::Unprotected:
				reg_setMode(reg_limitation);
				reg_setCurrent(I_SHORT_CIRCUIT);
				break;
		}

		/**************************************
		 * Copy measure data
		 */
		uint32_t measV = (regmeas.voltage + 500) / 1000; // uV to mV
		/*if(measV > 99999999){
			measV = 99999999;
		}*/

		uint32_t measI = regmeas.current;
		/*if(measI > 9999999){
			measI = 9999999;
		}*/

		/**************************************
		 * Output data to display
		 */
		//Print voltage
		if(regenable){
			snprintf(str, sizeof(str), "%02" PRIu32 ".%02" PRIu32 "0", measV / 1000, ((measV + 5) / 10) % 100);
		}else{
			params[Prm::basepreset.val].voltage->tostring(str, sizeof(str));
		}
		if(varParam == VAR_VOLT){
			disp_setColor(black, ui.color.cursor);
		}else{
			disp_setColor(black, ui.color.voltage);
		}
		disp_putStr(16, 0, &dSegBold, 6, str);
		disp_putChar(150, 18, &font8x12, 'V');

		//Print current
		if(varParam == VAR_CURR){
			disp_setColor(black, ui.color.cursor);
		}else{
			disp_setColor(black, ui.color.current);
		}

		if(regenable){
			if(measI < 99000){
				snprintf(str, sizeof(str), "%02" PRIu32 ".%02" PRIu32, measI / 1000, (measI + 5) / 10 % 100);
				disp_putChar(150, 36, &font8x12, 'm');
				disp_putChar(150, 49, &font8x12, 'A');
			}else{
				snprintf(str, sizeof(str), "%02" PRIu32 ".%03" PRIu32, measI / 1000000, (measI / 1000) % 1000);
				disp_putChar(150, 36, &font8x12, ' ');
				disp_putChar(150, 49, &font8x12, 'A');
			}

		}else{
			strcpy(str, "--.---");
			disp_putChar(150, 36, &font8x12, ' ');
			disp_putChar(150, 49, &font8x12, 'A');
		}
		disp_putStr(16, 36, &dSegBold, 6, str);

		//Print current settings
		switch(Prm::basepreset.val){
			case 0:
				grf_fillRect(0, 104, 53, 3, white);
				grf_fillRect(105, 104, 55, 3, black);
				break;
			case 1:
				grf_fillRect(53, 104, 53, 3, white);
				grf_fillRect(0, 104, 53, 3, black);
				break;
			case 2:
				grf_fillRect(105, 104, 55, 3, white);
				grf_fillRect(53, 104, 53, 3, black);
				break;
		}

		//Print limiting value
		if(varParam == VAR_CURR){
			disp_setColor(black, ui.color.cursor);
		}else{
			disp_setColor(black, ui.color.imax);
		}
		params[Prm::basepreset.val].current->tostring(str, sizeof(str));
		disp_putStr(16, 70, &arial, 0, str);
		disp_putChar(64, 72, &font8x12, 'A');

		//Print mode
		if(varParam == VAR_MODE){
			disp_setColor(black, ui.color.cursor);
		}else{
			disp_setColor(black, ui.color.mode);
		}
		params[Prm::basepreset.val].mode->tostring(str, sizeof(str));
		disp_putStr(16, 88, &arial, 0, str);

		//Print line
		grf_line(0, 107, 159, 107, halfLightGray);

		//Print status bar
		printStatusBar();

		disp_flushfill(&ui.color.background);

		//Cyclic delay
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(BASE_TSK_PERIOD));
	}
}

/*!****************************************************************************
 *
 */
void printStatusBar(void){
	static uint8_t errPrev = 0;
	static uint8_t modeIlimPrev = 0;
	static uint8_t ovfCurrent = 0;
	static char str[30];

	regMeas_t regmeas = {};
	bool regstate = reg_getState(&regmeas);

	if(modeIlimPrev != regmeas.state.m_limitation){
		if(regmeas.state.m_limitation != 0){
			BeepTime(ui.beep.cvToCc.time, ui.beep.cvToCc.freq);
		}else{
			BeepTime(ui.beep.ccToCv.time, ui.beep.ccToCv.freq);
		}
		modeIlimPrev = regmeas.state.m_limitation;
	}

	if((regmeas.state.m_overCurrent != 0) && (ovfCurrent == 0)){
		BeepTime(ui.beep.ovfCurrent.time, ui.beep.ovfCurrent.freq);
	}
	ovfCurrent = regmeas.state.m_overCurrent;

	//if(regmeas.state.m_errorTemperatureSensor || regmeas.state.m_overheated || regmeas.state.m_reverseVoltage || !regstate){
	if(0){
		BeepTime(ui.beep.error.time, ui.beep.error.freq);
		disp_setColor(black, white);
		if(errPrev == 0){
			grf_fillRect(0, 108, 160, 19, black);
			errPrev = 0;
		}

		if(regmeas.state.m_errorTemperatureSensor){
			disp_putStr(16, 112, &arial, 0, "Err Temp Sensor");
		}else if(regmeas.state.m_overheated != 0){
			disp_putStr(16, 112, &arial, 0, "Overflow Temp");
		}else if(regmeas.state.m_reverseVoltage != 0){
			disp_putStr(16, 112, &arial, 0, "Reverse Voltage");
		}else if(!regstate){
			disp_putStr(35, 112, &arial, 0, "No Connect");
		}

		errPrev = 1;
	}else{	//Not failure
		if(errPrev != 0){
			grf_fillRect(0, 108, 160, 19, black);
			errPrev = 0;
		}
		disp_setColor(black, white);

		//Print output power
		snprintf(str, sizeof(str), "%02" PRIu32 ".%03" PRIu32 " W", regmeas.power / 1000, regmeas.power % 1000);
		disp_putStr(0, 110, &font6x8, 0, str);

		//Print load resistance
		if(regmeas.resistance != 99999000){
			if(regmeas.resistance < 10000){
				snprintf(str, sizeof(str), "%02" PRIu32 ".%03" PRIu32 " \xB1", regmeas.resistance / 1000, regmeas.resistance % 1000);
				disp_putStr(0, 120, &font6x8, 0, str);
			}else{
				snprintf(str, sizeof(str), "%05" PRIu32 " \xB1", regmeas.resistance / 1000);
				disp_putStr(0, 120, &font6x8, 0, str);
			}
		}else{
			disp_putStr(0, 120, &font6x8, 0, " ---   \xB1");
		}

		//Print regulator temperature
		snprintf(str, sizeof(str), "%02" PRIu16 ".%" PRIu16 "\xB0\x43", regmeas.temperature / 10, regmeas.temperature % 10);
		disp_putStr(60, 120, &font6x8, 0, str);

		//Print time
		struct tm tm;
		time_t unixTime = time(NULL);
		localtime_r(&unixTime, &tm);
		strftime(str, sizeof(str), "%H:%M:%S", &tm);
		disp_putStr(110, 110, &font6x8, 0, str);
		strftime(str, sizeof(str), "%d.%m.%y", &tm);
		disp_putStr(110, 120, &font6x8, 0, str);

		//Print LAN status
		static TickType_t xTime;
		static uint8_t ledon = 0;
		if(fp.state.lanLink != 0){
			if(fp.state.lanActive != 0){
				xTime = xTaskGetTickCount();
				ledon = 1;
				fp.state.lanActive = 0;
			}
			if((ledon != 0)&&((xTaskGetTickCount() - xTime) >= 200)){
				ledon = 0;
			}

			if(ledon == 0){
				disp_setColor(black, white);
			}else{
				disp_setColor(black, red);
			}

			snprintf(str, sizeof(str), "LAN");
			disp_putStr(69, 110, &font6x8, 0, str);
		}
		else{
			snprintf(str, sizeof(str), "    ");
			disp_putStr(69, 110, &font6x8, 0, str);
		}

		errPrev = 0;
	}
}

/******************************** END OF FILE ********************************/

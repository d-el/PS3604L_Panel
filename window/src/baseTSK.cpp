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
#include <plog.h>
#include <hal/beep.h>
#include <hal/key.h>
#include <hal/enco.h>
#include <hal/sysTimeMeas.h>
#include <prmSystem.h>
#include "ui.h"
#include "display.h"
#include "systemTSK.h"
#include "regulatorConnTSK.h"
#include "baseTSK.h"
#include "footer.h"


#define BASE_TSK_PERIOD		(20) //[ms]

enum {
	VAR_VOLT = 0,
	VAR_CURR,
	VAR_NUMBER
};

/******************************************************************************
 * Base task
 */
void baseTSK(void *pPrm){
	Disp& disp = *(Disp*)pPrm;
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

	disp.setColor(black, red);
	disp.fillScreen(black);
	ksSet(30, 3, kUp | kDown);
	enco_settic(1);
	bool func = false;

	while(1){
		regState_t regmeas = {};
		reg_getState(&regmeas);
		bool regenable = false;
		reg_enableGet(&regenable);
		regTarget_t target = {};
		if(reg_getremote()){
			reg_getTarget(&target);
		}
		regCrange_t crange;
		reg_crangeGet(&crange);

		auto nextPreset = [&params](){
			if(Prm::basepreset.val < params.size() - 1)
				Prm::basepreset.val++;
			else
				Prm::basepreset.val = 0;
		};

		/**************************************
		 * Key process
		 */
		if(keyProc() != 0){
			if(reg_getremote()){
				BeepTime(Prm::bpErrOnOff ? ui.beep.error.time : 0, ui.beep.error.freq);
			}else{
				BeepTime(Prm::bpKeyOnOff ? ui.beep.key.time : 0, ui.beep.key.freq);
				if(func == false){
					if(keyState(kNext)){
						varParam++;
						if(varParam >= VAR_NUMBER){
							varParam = VAR_VOLT;
						}
					}else if(keyState(kMode)){
						if(!regenable){
							selWindow(chargerWindow);
						}else{
							BeepTime(Prm::bpErrOnOff ? ui.beep.error.time : 0, ui.beep.error.freq);
						}
					}else if(keyState(kFunc)){
						func = true;
					}else if(keyState(kOnOff)){
						uint8_t result = 0;
						if(!regenable){
							reg_enableSet(true);
						}else{
							reg_enableSet(false);
						}
						if(result != 0){
							disp.putStr(0, 0, &arial, "Error Connect");
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
				else{ // func == true
					func = false;
					if(keyState(kNext)){
						if(!regenable){
							nextPreset();
						}else{
							BeepTime(Prm::bpErrOnOff ? ui.beep.error.time : 0, ui.beep.error.freq);
						}
					}
					else if(keyState(kMode)){
						selWindow(settingWindow);
					}
					else if(keyState(kFunc)){
						func = false;
					}
					else if(keyState(kUp)){
						Prm::crange_set.val = Prm::mask_crange::crange_hi;
						reg_crangeSet(reg_crange_hi);
					}
					else if(keyState(kDown)){
						Prm::crange_set.val = Prm::mask_crange::crange_auto;
						reg_crangeSet(reg_crange_auto);
					}
				}
			}
		}

		auto encoval = enco_update();
		if(encoval != 0){
			if(reg_getremote()){
				BeepTime(Prm::bpErrOnOff ? ui.beep.error.time : 0, ui.beep.error.freq);
			}else{
				params[Prm::basepreset.val].p[varParam]->step(encoval);
			}
		}

		/***************************************
		 * Task for regulator
		 */
		reg_voltageSet(params[Prm::basepreset.val].voltage->val * 1000);
		uint32_t currentval = params[Prm::basepreset.val].current->val * 1000;
		switch(params[Prm::basepreset.val].mode->val){
			case Prm::mask_basemode::Imax:
				reg_modeSet(reg_overcurrentShutdown);
				reg_currentSet(currentval);
				break;

			case Prm::mask_basemode::Limiting:
				reg_modeSet(reg_limitation);
				reg_currentSet(currentval);
				break;
		}
		reg_timeSet(0);

		/**************************************
		* Output data to display
		*/
		auto printValue = [](char* str, size_t len, int32_t u){
			int32_t viewv = (u + 500) / 1000;
			if(viewv >= 0){
				return snprintf(str, len, "%02" PRIi32 ".%03" PRIi32, viewv / 1000, viewv % 1000);
			}else{
				viewv = -viewv;
				return snprintf(str, len, "-%" PRIi32 ".%03" PRIi32, viewv / 1000, viewv % 1000);
			}
		};

		// Print voltage
		printValue(str, sizeof(str), regmeas.voltage);
		if(!reg_getremote() && varParam == VAR_VOLT){
			disp.setColor(black, ui.color.cursor);
		}else{
			disp.setColor(black, ui.color.voltage);
		}
		disp.putStr(10, 0, &dSegBold, str, 6);
		disp.putChar(146, 14, &arial, 'V');
		// Print voltage setting
		int32_t setv = reg_getremote() ? (target.voltage_set + 500) / 1000 : params[Prm::basepreset.val].voltage->val;
		snprintf(str, sizeof(str), "%2" PRIi32 ".%03" PRIi32 " V", setv / 1000, setv % 1000);
		disp.putStr(12, 33, &arial, str);

		//Print current
		int32_t measI = regmeas.current;
		if(!reg_getremote() && varParam == VAR_CURR){
			disp.setColor(black, ui.color.cursor);
		}else{
			disp.setColor(black, ui.color.current);
		}
		bool minus = false;
		if(measI < 0) measI = -measI, minus = true;
		if(measI < 9999 && crange == reg_crange_auto){
			snprintf(str, sizeof(str), "%s%" PRIi32 ".%03" PRIi32, minus ? "-" : "0", measI / 1000, measI % 1000);
			disp.putChar(147, 54, &font8x12, 'm');
		}else if(measI < 59000  && crange == reg_crange_auto){
			snprintf(str, sizeof(str), "%02" PRIi32 ".%03" PRIi32, measI / 1000, measI % 1000);
			disp.putChar(147, 54, &font8x12, 'm');
		}else{
			measI = (measI + 50) / 100;
			snprintf(str, sizeof(str), "%01" PRIi32 ".%04" PRIi32, measI / 10000, measI % 10000);
		}
		disp.putChar(146, 69, &arial, 'A');
		disp.putStr(10, 53, &dSegBold, str, 6);
		// Print limiting value
		int32_t limi = reg_getremote() ? (target.current_set + 500) / 1000 : params[Prm::basepreset.val].current->val;
		snprintf(str, sizeof(str), "%1" PRIi32 ".%03" PRIi32 " A", limi / 1000, limi % 1000);
		disp.putStr(12, 87, &arial, str);

		disp.setColor(black, red);
		if(!regenable){
			snprintf(str, sizeof(str), "%s", "DIS");
			disp.putStr(130, 89, &font8x12, str);
		}
		else{
			if(regmeas.status.m_limitation){
				snprintf(str, sizeof(str), "%s", "CC");
			}
			else{
				snprintf(str, sizeof(str), "%s", "CV");
			}
			disp.putStr(106, 89, &font8x12, str, 0);
		}

		if((!reg_getremote() && crange == reg_crange_auto)){
			if(regmeas.status.cRangeLoOverflow){
				snprintf(str, sizeof(str), "%s", "CRH");
			}else{
				snprintf(str, sizeof(str), "%s", "CRL");
			}
			disp.putStr(75, 89, &font8x12, str);
		}

		// Print current settings
		switch(Prm::basepreset.val){
			case 0:
				disp.fillRect(0, 104, 53, 3, white);
				disp.fillRect(105, 104, 55, 3, black);
				break;
			case 1:
				disp.fillRect(53, 104, 53, 3, white);
				disp.fillRect(0, 104, 53, 3, black);
				break;
			case 2:
				disp.fillRect(105, 104, 55, 3, white);
				disp.fillRect(53, 104, 53, 3, black);
				break;
		}

		if(func){
			disp.fillRoundRect(0, 8, 6, 12, 2, red);
		}

		// Print status bar
		printFooter(disp);

		disp.flushfill(&ui.color.background);

		// Cyclic delay
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(BASE_TSK_PERIOD));
	}
}

/******************************** END OF FILE ********************************/

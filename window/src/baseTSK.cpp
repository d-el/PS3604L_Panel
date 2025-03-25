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
#include "footer.h"

/******************************************************************************
 * Base task
 */

enum {
	VAR_VOLT = 0,
	VAR_CURR,
	/*VAR_MODE,*/
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
		regState_t regmeas = {};
		reg_getState(&regmeas);
		bool regenable = false;
		reg_getEnable(&regenable);

		/**************************************
		 * Key process
		 */
		if(keyProc() != 0){
			if(reg_getremote()){
				BeepTime(ui.beep.error.time, ui.beep.error.freq);
			}else{
				BeepTime(ui.beep.key.time, ui.beep.key.freq);
				if(keyState(kNext)){
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
				}else if(keyState(kFunc)){					//Next preset
					if(keyDin(kNext) && !regenable){
						selWindow(settingWindow);
					}
					if(!regenable){
						if(Prm::basepreset.val < VAR_CURR)
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
		}

		auto encoval = enco_update();
		if(encoval != 0){
			if(reg_getremote()){
				BeepTime(ui.beep.error.time, ui.beep.error.freq);
			}else{
				params[Prm::basepreset.val].p[varParam]->step(encoval);
			}
		}

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
		}
		reg_setTime(0);

		/**************************************
		 * Copy measure data
		 */
		int32_t measV = (regmeas.voltage + 500) / 1000; // uV to mV
		int32_t measI = regmeas.current;

		/**************************************
		 * Output data to display
		 */
		//Print voltage
		int32_t viewvoltage = /*regenable ? */measV /*: params[Prm::basepreset.val].voltage->val*/;
		if(viewvoltage >= 0){
			snprintf(str, sizeof(str), "%02" PRIi32 ".%03" PRIi32, viewvoltage / 1000, viewvoltage % 1000);
		}else{
			viewvoltage = -viewvoltage;
			snprintf(str, sizeof(str), "-%" PRIi32 ".%03" PRIi32, viewvoltage / 1000, viewvoltage % 1000);
		}
		if(varParam == VAR_VOLT){
			disp_setColor(black, ui.color.cursor);
		}else{
		disp_setColor(black, ui.color.voltage);
		}
		disp_putStr(10, 0, &dSegBold, 6, str);
		disp_putChar(146, 18, &font8x12, 'V');

		//Print voltage setting
		if(varParam == VAR_VOLT){
			disp_setColor(black, ui.color.cursor);
		}else{
			disp_setColor(black, ui.color.voltage);
		}
		size_t len = params[Prm::basepreset.val].voltage->tostring(str, sizeof(str));
		//snprintf(&str[len], sizeof(str) - len, " V");
		disp_putStr(12, 33, &arial, 0, str);
//		disp_putChar(60, 35, &font8x12, 'V');

		//Print current
		if(varParam == VAR_CURR){
			disp_setColor(black, ui.color.cursor);
		}else{
			disp_setColor(black, ui.color.current);
		}

		//if(regenable){
		bool minus = false;
		if(measI < 0) measI = -measI, minus = true;
		if(measI < 9999 && Prm::crange_set.val == Prm::crange_auto){
			snprintf(str, sizeof(str), "%s%" PRIi32 ".%03" PRIi32, minus ? "-" : "0", measI / 1000, measI % 1000);
			disp_putChar(146, 61, &font8x12, 'm');
		}else if(measI < 59000  && Prm::crange_set.val == Prm::crange_auto){
			snprintf(str, sizeof(str), "%02" PRIi32 ".%03" PRIi32, measI / 1000, measI % 1000);
			disp_putChar(146, 61, &font8x12, 'm');
		}else{
			measI = (measI + 50) / 100;
			snprintf(str, sizeof(str), "%01" PRIi32 ".%04" PRIi32, measI / 10000, measI % 10000);
		}
		disp_putChar(146, 73, &font8x12, 'A');
//		}else{
//			strcpy(str, "--.---");
//			disp_putChar(146, 36, &font8x12, ' ');
//			disp_putChar(146, 49, &font8x12, 'A');
//		}
		disp_putStr(10, 53, &dSegBold, 6, str);

		// Print limiting value
		if(varParam == VAR_CURR){
			disp_setColor(black, ui.color.cursor);
		}else{
			disp_setColor(black, ui.color.imax);
		}
		params[Prm::basepreset.val].current->tostring(str, sizeof(str));
//		snprintf(&str[len], sizeof(str) - len, " A");
		disp_putStr(12, 87, &arial, 0, str);
//		disp_putChar(60, 89, &font8x12, 'A');

		disp_setColor(black, red);
		if(!regenable){
			snprintf(str, sizeof(str), "%s", "DIS");
			disp_putStr(130, 89, &font8x12, 0, str);
		}
		else{
			if(regmeas.status.m_limitation){
				snprintf(str, sizeof(str), "%s", "CC");
			}
			else{
				snprintf(str, sizeof(str), "%s", "CV");
			}
			disp_putStr(106, 89, &font8x12, 0, str);
		}

		if(Prm::crange_set.val == Prm::crange_auto && regenable){
			if(regmeas.status.cRangeLoOverflow){
				snprintf(str, sizeof(str), "%s", "CRH");
			}else{
				snprintf(str, sizeof(str), "%s", "CRL");
			}
			disp_putStr(75, 89, &font8x12, 0, str);
		}

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

		if(!reg_getremote()){

//			//Print mode
//			if(varParam == VAR_MODE){
//				disp_setColor(black, ui.color.cursor);
//			}else{
//				disp_setColor(black, ui.color.mode);
//			}
//			params[Prm::basepreset.val].mode->tostring(str, sizeof(str));
//			disp_putStr(10, 88, &arial, 0, str);
		}

		//Print status bar
		printFooter();

		disp_flushfill(&ui.color.background);

		//Cyclic delay
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(BASE_TSK_PERIOD));
	}
}

/******************************** END OF FILE ********************************/

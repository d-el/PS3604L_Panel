/*!****************************************************************************
 * @file		charge.с
 * @author		d_el
 * @version		V1.0
 * @date		31.01.20187
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
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
#include <hal/beep.h>
#include <hal/key.h>
#include <hal/enco.h>
#include <display.h>
#include <regulatorConnTSK.h>
#include <ui.h>
#include <prmSystem.h>
#include <systemTSK.h>
#include <prmSystem.h>
#include "footer.h"
#include "chargeTSK.h"

#define CH_TSK_PERIOD	20 //[ms]

enum {
	C_VOLT,
	C_CURR,
	C_TIME,
	C_MODE,
	mode_number
};

enum {
	ch_modeTime = 0,
	ch_modeCurrent = 1,
};

/******************************************************************************
 * Charger task
 */
void chargeTSK(void *pPrm){
	Disp& disp = *(Disp*)pPrm;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	uint8_t varParam = 0;
	char str[32];
	bool finishBeep = false;

	struct BaeParameter{
		union{
			struct{
				const Prm::Val<uint16_t> *voltage;
				const Prm::Val<uint16_t> *current;
				const Prm::Val<uint32_t> *time;
				const Prm::Val<uint8_t> *mode;
			};
			Prm::IVal *p[4];
		};
	};

	const BaeParameter params = {
			&Prm::chargeru,
			&Prm::chargeri,
			&Prm::chargert,
			&Prm::chargermode
	};

	disp.fillScreen(black);
	ksSet(30, 3, kUp | kDown);
	enco_settic(1);

	while(1){
		regState_t regmeas = {};
		reg_getState(&regmeas);
		bool stateenable = false;
		reg_enableGet(&stateenable);

		/**************************************
		 * Key process
		 */
		if(keyProc() != 0){
			BeepTime(Prm::bpKeyOnOff ? ui.beep.key.time : 0, ui.beep.key.freq);
			if(keyState(kOnOff)){
				if(!stateenable){
					reg_enableSet(true);
				}else{
					reg_enableSet(false);
				}
			}
			if(!stateenable){
				if(keyState(kNext)){
					varParam++;
					if(varParam == mode_number)
						varParam = 0;
				}else if(keyState(kMode)){
						selWindow(baseWindow);
				}else if(keyState(kUp)){
					params.p[varParam]->bigstep(1);
				}else if(keyState(kDown)){
					params.p[varParam]->bigstep(-1);
				}else if(keyState(kZero)){
					params.p[varParam]->setdef();
				}
			}
		}

		/***************************************
		 * Encoder process
		 */
		if(!stateenable){
			params.p[varParam]->step(enco_update());
		}else{
			enco_update();
		}

		/***************************************
		 * Task for regulator
		 */
		reg_voltageSet(params.voltage->val * 1000);
		reg_currentSet(params.current->val * 1000);
		reg_timeSet(params.mode->val == ch_modeTime ? params.time->val : 0);
		reg_modeSet(params.mode->val == ch_modeTime ? reg_limitation : reg_lowCurrentShutdown);

		int32_t measV = (regmeas.voltage + 500) / 1000; // uV to mV
		int32_t measI = (regmeas.current + 500) / 1000; // uA to mA

		/**************************************
		 * Output data to display
		 */
		//Skip time
		if((params.mode->val == ch_modeCurrent) && (varParam == C_TIME)){
			varParam++;
		}
		//Print voltage
		if(stateenable){
			bool minus = false;
			if(measV < 0) measV = -measV, minus = true;
			snprintf(str, sizeof(str), "U:        %s%02" PRIi32 ".%03" PRIi32, minus ? "-" : "", measV / 1000, measV % 1000);
		}else{
			snprintf(str, sizeof(str), "U:        %02" PRIu16 ".%03" PRIu16, params.voltage->val / 1000, params.voltage->val % 1000);
		}
		if(varParam == C_VOLT && !stateenable){
			disp.setColor(black, ui.color.cursor);
		}else{
			disp.setColor(black, ui.color.voltage);
		}
		disp.putStr(10, 00, &arial, str);

		//Print current
		if(stateenable){
			bool minus = false;
			if(measI < 0) measI = -measI, minus = true;
			snprintf(str, sizeof(str), "I:          %s%01" PRIi32 ".%03" PRIi32 " A", minus ? "-" : "", measI / 1000, measI % 1000);
		}else{
			snprintf(str, sizeof(str), "I:          %0" PRIu16 ".%03" PRIu16 " A", params.current->val / 1000, params.current->val % 1000);
		}
		if(varParam == C_CURR && !stateenable){
			disp.setColor(black, ui.color.cursor);
		}else{
			disp.setColor(black, ui.color.current);
		}
		disp.putStr(10, 20, &arial, str);

		//Time
		uint32_t reg_time_s = (regmeas.time + 500) / 1000;
		uint32_t set_time_s = params.time->val / 1000;
		uint32_t view_time_s = 0;
		if(stateenable){
			view_time_s = params.mode->val == ch_modeTime ? set_time_s - reg_time_s : reg_time_s;
		}else{
			view_time_s = params.mode->val == ch_modeTime ? set_time_s : reg_time_s;
		}
		if(view_time_s < 60){
			snprintf(str, sizeof(str), "Time:  %" PRIu32 "s", view_time_s);
		}else if(view_time_s < 3600){
			snprintf(str, sizeof(str), "Time:  %" PRIu32 "m %02" PRIu32 "s", view_time_s / 60, view_time_s % 60);
		}else{
			snprintf(str, sizeof(str), "Time:  %02" PRIu32 "h%02" PRIu32 "m%02" PRIu32 "s", view_time_s / 3600, view_time_s / 60 % 60, view_time_s % 60);
		}
		if(varParam == C_TIME && !stateenable){
			disp.setColor(black, ui.color.cursor);
		}else{
			disp.setColor(black, ui.color.current);
		}
		disp.putStr(10, 40, &arial, str);

		// Print Mode
		if(params.mode->val == ch_modeTime){
			snprintf(str, sizeof(str), "Mode: TIME");
		}else{
			snprintf(str, sizeof(str), "Mode: VOLTAGE");
		}
		if(varParam == C_MODE && !stateenable){
			disp.setColor(black, ui.color.cursor);
		}else{
			disp.setColor(black, ui.color.mode);
		}
		disp.putStr(10, 60, &arial, str);

		//Print Capacity
		snprintf(str, sizeof(str), "C:         %01" PRIu32 ".%03" PRIu32 " Ah", regmeas.capacity / 1000, regmeas.capacity % 1000);
		disp.setColor(black, ui.color.capacity);
		disp.putStr(10, 80, &arial, str);

		if(finishBeep && (regmeas.disablecause == v_timeShutdown || regmeas.disablecause == v_lowCurrentShutdown)){
			BeepTime(Prm::bpChFinOnOff ? ui.beep.chargeFinish.time : 0, ui.beep.chargeFinish.freq);
			finishBeep = false;
		}
		if(stateenable){
			finishBeep = true;
		}

		//Print status bar
		printFooter(disp);

		disp.flushfill(&ui.color.background);
		//Cyclic delay
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(CH_TSK_PERIOD));
	}
}

/***************** Copyright (C) Storozhenko Roman ******* END OF FILE *******/

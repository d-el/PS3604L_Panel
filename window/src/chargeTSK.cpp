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
#include <beep.h>
#include <key.h>
#include <display.h>
#include <regulatorConnTSK.h>
#include <ui.h>
#include <graphics.h>
#include <prmSystem.h>
#include <systemTSK.h>
#include "chargeTSK.h"
#include <prmSystem.h>
#include <enco.h>
#include "footer.h"

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
	(void)pPrm;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	uint8_t varParam = 0;
	char str[30];
	bool finishBeep = false;

	struct BaeParameter{
		union{
			struct{
				const Prm::Val<uint16_t> *voltage;
				const Prm::Val<uint16_t> *current;
				const Prm::Val<uint16_t> *time;
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

	disp_fillScreen(black);
	ksSet(30, 5, kUp | kDown);
	enco_settic(5);

	while(1){
		regState_t regmeas = {};
		reg_getState(&regmeas);
		bool stateenable = false;
		reg_getEnable(&stateenable);

		/**************************************
		 * Key process
		 */
		if(keyProc() != 0){
			BeepTime(ui.beep.key.time, ui.beep.key.freq);
			if(keyState(kOnOff)){
				if(!stateenable){
					reg_setEnable(true);
				}else{
					reg_setEnable(false);
				}
			}
			if(!stateenable){
				if(keyState(kSet)){
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
		reg_setVoltage(params.voltage->val * 1000);
		reg_setCurrent(params.current->val * 1000);
		reg_setTime(params.mode->val == ch_modeTime ? params.time->val * 60*1000 : 0);

		uint32_t measV = (regmeas.voltage + 500) / 1000; // uV to mV
		uint32_t measI = (regmeas.current + 500) / 1000; // uA to mA

		/**************************************
		 * Output data to display
		 */
		//Skip time
		if((params.mode->val == ch_modeCurrent) && (varParam == C_TIME)){
			varParam++;
		}
		//Print voltage
		if(stateenable){
			snprintf(str, sizeof(str), "U:        %02" PRIu32 ".%03" PRIu32, measV / 1000, measV % 1000);
		}else{
			snprintf(str, sizeof(str), "U:        %02" PRIu16 ".%03" PRIu16, params.voltage->val / 1000, params.voltage->val % 1000);
		}
		if(varParam == C_VOLT && !stateenable){
			disp_setColor(black, ui.color.cursor);
		}else{
			disp_setColor(black, ui.color.voltage);
		}
		disp_putStr(10, 00, &arial, 0, str);

		//Print current
		if(stateenable){
			snprintf(str, sizeof(str), "I:          %01" PRIu32 ".%03" PRIu32 " A", measI / 1000, measI % 1000);
		}else{
			snprintf(str, sizeof(str), "I:          %0" PRIu16 ".%03" PRIu16 " A", params.current->val / 1000, params.current->val % 1000);
		}
		if(varParam == C_CURR && !stateenable){
			disp_setColor(black, ui.color.cursor);
		}else{
			disp_setColor(black, ui.color.current);
		}
		disp_putStr(10, 20, &arial, 0, str);

		//Time
		if(params.mode->val == ch_modeTime){
			if(stateenable){
				snprintf(str, sizeof(str), "Time:  %lum %02" PRIu32 "s   ", (params.time->val * 60 - regmeas.time) / 60, (params.time->val * 60 - regmeas.time) % 60);
			}else{
				snprintf(str, sizeof(str), "Time:  %" PRIu16 "m 00s     ", params.time->val);
			}
		}else{
			snprintf(str, sizeof(str), "Time:  %" PRIu32 "m %02" PRIu32 "s   ", (regmeas.time) / 60, (regmeas.time) % 60);

		}
		if(varParam == C_TIME && !stateenable){
			disp_setColor(black, ui.color.cursor);
		}else{
			disp_setColor(black, ui.color.current);
		}
		disp_putStr(10, 40, &arial, 0, str);

		// Print Mode
		if(params.mode->val == ch_modeTime){
			snprintf(str, sizeof(str), "Mode: TIME         ");
		}else{
			snprintf(str, sizeof(str), "Mode: VOLTAGE");
		}
		if(varParam == C_MODE && !stateenable){
			disp_setColor(black, ui.color.cursor);
		}else{
			disp_setColor(black, ui.color.mode);
		}
		disp_putStr(10, 60, &arial, 0, str);

		//Print Capacity
		snprintf(str, sizeof(str), "C:         %01" PRIu32 ".%03" PRIu32 " Ah", regmeas.capacity / 1000, regmeas.capacity % 1000);
		disp_setColor(black, ui.color.capacity);
		disp_putStr(10, 80, &arial, 0, str);

		if(finishBeep && (regmeas.disablecause == v_timeShutdown || regmeas.disablecause == v_lowCurrentShutdown)){
			BeepTime(ui.beep.chargeFinish.time, ui.beep.chargeFinish.freq);
			finishBeep = false;
		}
		if(stateenable){
			finishBeep = true;
		}

		//Print status bar
		printFooter();

		disp_flushfill(&ui.color.background);
		//Cyclic delay
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(CH_TSK_PERIOD));
	}
}

/***************** Copyright (C) Storozhenko Roman ******* END OF FILE *******/

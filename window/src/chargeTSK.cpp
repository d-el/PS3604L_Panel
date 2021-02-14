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
#include <prmEditor.h>
#include <systemTSK.h>
#include "chargeTSK.h"
#include "baseTSK.h"

/******************************************************************************
 * Memory
 */
charge_type ch;

/******************************************************************************
 * Charger task
 */
void chargeTSK(void *pPrm){
	(void)pPrm;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	const prmHandle_type *sHandle = &prmh[Nchu];
	const prmHandle_type *pHandle = &prmh[Nchu];
	uint8_t varParam = 0;
	bool switchStatePrev = false;
	char str[30];

	disp_fillScreen(black);
	ksSet(30, 10, kUp | kDown);
	prmEditorSetNtic(5);

	while(1){
		regMeas_t regmeas = {};
		reg_getState(&regmeas);
		bool stateenable = false;
		reg_getEnable(&stateenable);

		/**************************************
		 * Key process
		 */
		uint8_t bigstepUp = 0, bigstepDown = 0, setDef = 0;
		if(keyProc() != 0){
			BeepTime(ui.beep.key.time, ui.beep.key.freq);
			if(keyState(kSet)){
				varParam++;
				if(varParam == 4)
					varParam = 0;
			}else if(keyState(kMode)){
				if(!stateenable){
					selWindow(baseWindow);
				}else{
					BeepTime(ui.beep.error.time, ui.beep.error.freq);
				}
			}else if(keyState(kOnOff)){
				if(!stateenable){
					reg_setEnable(true);
				}else{
					reg_setEnable(false);
				}
			}else if(keyState(kUp)){
				bigstepUp = 1;
			}else if(keyState(kDown)){
				bigstepDown = 1;
			}else if(keyState(kZero)){
				setDef = 1;
			}
		}

		if(!stateenable){
			/***************************************
			 * Encoder process
			 */
			prmEditorStatus_type enstatus;
			sHandle = pHandle + varParam;
			if(bigstepUp != 0){
				enstatus = prmEditorBigStepUp(sHandle);
				bigstepUp = 0;
			}else if(bigstepDown != 0){
				enstatus = prmEditorBigStepDown(sHandle);
				bigstepDown = 0;
			}else if(setDef != 0){
				prmEditorWriteVal(sHandle, &(sHandle)->def);
				enstatus = enCharge;
				setDef = 0;
			}else{
				enstatus = prmEditorUpDate(sHandle);
			}
			if((enstatus == enLimDown) || (enstatus == enLimUp)){
				BeepTime(ui.beep.encoLim.time, ui.beep.encoLim.freq);
			}else if((enstatus == enTransitionDown) || (enstatus == enTransitionUp)){
				BeepTime(ui.beep.encoTransition.time, ui.beep.encoTransition.freq);
			}
		}

		/***************************************
		 * Task for regulator
		 */
		reg_setVoltage(ch.u * 1000);
		reg_setCurrent(ch.i * 1000);
		switch(ch.mode){
			case (ch_modeTime):
				reg_setMode(reg_timeShutdown);
				reg_setTime(ch.t * 60);
				break;
			case (ch_modeCurrent):
				reg_setMode(reg_lowCurrentShutdown);
				break;
			default:
				break;
		}

		uint32_t measV = regmeas.voltage; //[uV]
		if(measV > 99999999){
			measV = 99999999;
		}
		uint32_t measI = regmeas.current; //[uA]
		if(measI > 9999999){
			measI = 9999999;
		}

		/**************************************
		 * Output data to display
		 */
		//Skip time
		if((ch.mode == ch_modeCurrent) && (varParam == C_TIME)){
			varParam++;
		}
		//Print voltage
		if(stateenable){
			snprintf(str, sizeof(str), "U:        %02" PRIu32 ".%03"PRIu32, measV / 1000000, (measV / 1000) % 1000);
		}else{
			snprintf(str, sizeof(str), "U:        %02" PRIu16 ".%03"PRIu16, ch.u / 1000, ch.u % 1000);
		}
		if(varParam == C_VOLT && !stateenable){
			disp_setColor(black, ui.color.cursor);
		}else{
			disp_setColor(black, ui.color.voltage);
		}
		disp_putStr(10, 00, &arial, 0, str);

		//Print current
		if(stateenable){
			snprintf(str, sizeof(str), "I:          %01" PRIu32 ".%03" PRIu32 " A", measI / 1000000, (measI / 1000) % 1000);
		}else{
			snprintf(str, sizeof(str), "I:          %0" PRIu16 ".%03" PRIu16 " A", ch.i / 1000, ch.i % 1000);
		}
		if(varParam == C_CURR && !stateenable){
			disp_setColor(black, ui.color.cursor);
		}else{
			disp_setColor(black, ui.color.current);
		}
		disp_putStr(10, 20, &arial, 0, str);

		//Time
		if(ch.mode == ch_modeTime){
			if(stateenable){
				snprintf(str, sizeof(str), "Time:  %lum %02" PRIu32 "s   ", (ch.t * 60 - regmeas.time) / 60, (ch.t * 60 - regmeas.time) % 60);
			}else{
				snprintf(str, sizeof(str), "Time:  %" PRIu16 "m 00s     ", ch.t);
			}
		}else{
			if(stateenable){
				snprintf(str, sizeof(str), "Time:  %" PRIu32 "m %02"PRIu32"s   ", (regmeas.time) / 60, (regmeas.time) % 60);
			}else{
				snprintf(str, sizeof(str), "Time:  - - -            ");
			}

		}
		if(varParam == C_TIME && !stateenable){
			disp_setColor(black, ui.color.cursor);
		}else{
			disp_setColor(black, ui.color.current);
		}
		disp_putStr(10, 40, &arial, 0, str);

		//Print Mode
		if(ch.mode == ch_modeTime){
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

		grf_line(0, 107, 159, 107, halfLightGray);


		if(switchStatePrev && !stateenable && ch.t * 60 == regmeas.time){
			BeepTime(ui.beep.chargeReady.time, ui.beep.chargeReady.freq);
		}
		switchStatePrev = stateenable;

		//Print status bar
		printStatusBar();

		disp_flushfill(&ui.color.background);
		//Cyclic delay
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(CH_TSK_PERIOD));
	}
}

/***************** Copyright (C) Storozhenko Roman ******* END OF FILE *******/

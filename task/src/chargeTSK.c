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
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "ui.h"
#include "pstypes.h"
#include "graphics.h"
#include "display.h"
#include "key.h"
#include "prmSystem.h"
#include "prmEditor.h"
#include "beep.h"
#include "chargeTSK.h"
#include "regulatorConnTSK.h"
#include "systemTSK.h"
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
	TickType_t 				xLastWakeTime = xTaskGetTickCount();
	const prmHandle_type 	*sHandle = &prmh[Nchu];
	const prmHandle_type 	*pHandle = &prmh[Nchu];
	uint32_t 				measV;	//[uV]
	uint32_t 				measI;	//[uA]
	uint8_t 				varParam = 0;
	uint8_t 				bigstepUp = 0;
	uint8_t 				bigstepDown = 0;
	uint8_t 				setDef = 0;
	uint8_t 				chargerIsOn = 0;
	char 					str[30];

	disp_fillScreen(black);

	//Print static element
	grf_line(0, 107, 159, 107, halfLightGray);
	ksSet(30, 10, kUp | kDown);
	prmEditorSetNtic(5);

	while(1){
		/**************************************
		 * Key process
		 */
		if(keyProc() != 0){
			BeepTime(ui.beep.key.time, ui.beep.key.freq);
			if(keyState(kSet)){
				varParam++;
				if(varParam == 4)
					varParam = 0;
			}else if(keyState(kMode)){
				if(fp.tf.state.bit.switchIsON == 0){
					selWindow(baseWindow);
				}else{
					BeepTime(ui.beep.error.time, ui.beep.error.freq);
				}
			}else if(keyState(kOnOff)){
				if(fp.tf.state.bit.switchIsON == 0){
					sendCommand(setSwitchOn);
				}else{
					sendCommand(setSwitchOff);
				}
			}else if(keyState(kUp)){
				bigstepUp = 1;
			}else if(keyState(kDown)){
				bigstepDown = 1;
			}else if(keyState(kZero)){
				setDef = 1;
			}
		}

		if(fp.tf.state.bit.switchIsON == 0){
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
		fp.tf.task.u = ch.u * 1000;
		fp.tf.task.i = ch.i * 1000;
		switch(ch.mode){
			case (ch_modeTime):
				fp.tf.task.mode = mode_timeShutdown;
				fp.tf.task.time = ch.t * 60;
				break;
			case (ch_modeCurrent):
				fp.tf.task.mode = mode_lowCurrentShutdown;
				break;
			default:
				break;
		}

		/**************************************
		 * Copy measure data
		 */
		measV = fp.tf.meas.u;
		if(measV > 99999999){
			measV = 99999999;
		}
		measI = fp.tf.meas.i;
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
		if(fp.tf.state.bit.switchIsON != 0){
			snprintf(str, sizeof(str), "U:        %02"PRIu32".%03"PRIu32, measV / 1000000, (measV / 1000) % 1000);
		}else{
			snprintf(str, sizeof(str), "U:        %02"PRIu16".%03"PRIu16, ch.u / 1000, ch.u % 1000);
		}
		if((varParam == C_VOLT) && (fp.tf.state.bit.switchIsON == 0)){
			disp_setColor(black, ui.color.cursor);
		}else{
			disp_setColor(black, ui.color.voltage);
		}
		disp_putStr(10, 00, &arial, 0, str);

		//Print current
		if(fp.tf.state.bit.switchIsON != 0){
			snprintf(str, sizeof(str), "I:          %01"PRIu32".%03"PRIu32" A", measI / 1000000, (measI / 1000) % 1000);
		}else{
			snprintf(str, sizeof(str), "I:          %0"PRIu16".%03"PRIu16" A", ch.i / 1000, ch.i % 1000);
		}
		if((varParam == C_CURR) && (fp.tf.state.bit.switchIsON == 0)){
			disp_setColor(black, ui.color.cursor);
		}else{
			disp_setColor(black, ui.color.current);
		}
		disp_putStr(10, 20, &arial, 0, str);

		//Time
		if(ch.mode == ch_modeTime){
			if(fp.tf.state.bit.switchIsON != 0){
				snprintf(str, sizeof(str), "Time:  %lum %02"PRIu32"s   ", (ch.t * 60 - fp.tf.meas.time) / 60, (ch.t * 60 - fp.tf.meas.time) % 60);
			}else{
				snprintf(str, sizeof(str), "Time:  %"PRIu16"m 00s     ", ch.t);
			}
		}else{
			if(fp.tf.state.bit.switchIsON != 0){
				snprintf(str, sizeof(str), "Time:  %"PRIu32"m %02"PRIu32"s   ", (fp.tf.meas.time) / 60, (fp.tf.meas.time) % 60);
			}else{
				snprintf(str, sizeof(str), "Time:  - - -            ");
			}

		}
		if((varParam == C_TIME) && (fp.tf.state.bit.switchIsON == 0)){
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
		if((varParam == C_MODE) && (fp.tf.state.bit.switchIsON == 0)){
			disp_setColor(black, ui.color.cursor);
		}else{
			disp_setColor(black, ui.color.mode);
		}
		disp_putStr(10, 60, &arial, 0, str);

		//Print Capacity
		snprintf(str, sizeof(str), "C:         %01"PRIu32".%03"PRIu32" A/h", fp.tf.meas.capacity / 1000, fp.tf.meas.capacity % 1000);
		disp_setColor(black, ui.color.capacity);
		disp_putStr(10, 80, &arial, 0, str);

		//Detection finish
		if((chargerIsOn != 0) && (fp.tf.state.bit.switchIsON == 0)){
			if(keyState(kOnOff)){
				BeepTime(ui.beep.chargeReady.time, ui.beep.chargeReady.freq);
				chargerIsOn = 0;
				while(keyProc() == 0){
					printStatusBar();
					vTaskDelayUntil(&xLastWakeTime, CH_TSK_PERIOD);
				}
			}
		}
		if(fp.tf.state.bit.switchIsON != 0){
			chargerIsOn = 1;
		}

		//Print status bar
		printStatusBar();

		//Cyclic delay
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(CH_TSK_PERIOD));
	}
}

/***************** Copyright (C) Storozhenko Roman ******* END OF FILE *******/

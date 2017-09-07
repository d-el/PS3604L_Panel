/******************************************************************************
 * @file			charge.c
 * @author		D_EL - Storozhenko Roman
 * @version      V1.0
 * @date
 * @copyright GNU Public License
 */

#include "chargeTSK.h"

/******************************************************************************
 * Memory
 */
charge_type ch;

/******************************************************************************
 * Charger task
 */
void chargeTSK(void *pPrm){
	TickType_t 				xLastWakeTime = xTaskGetTickCount();
	const prmHandle_type 	*sHandle = &prmh[Nchu];
	const prmHandle_type 	*pHandle = &prmh[Nchu];
	uint32_t 				measV;	//[uV]
	uint32_t 				measI;	//[uA]
	uint8_t 				varParam = 0;
	uint8_t 				bigstepUp = 0;
	uint8_t 				bigstepDown = 0;
	uint8_t 				setDef = 0;
	enStatus_type 			enstatus;
	uint8_t 				chargerIsOn = 0;

	lcd_fillScreen(black);

	//Печать статических символов
	grf_line(0, 107, 159, 107, halfLightGray);
	ksSet(30, 10, kUp | kDown);
	enSetNtic(5);

	while(1){
		/**************************************
		 * Обработка кнопок
		 */
		if(keyProc() != 0){
			BeepTime(ui.beep.key.time, ui.beep.key.freq);
			if(keyState(kNext)){
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
			 * Вынимаем значение с энкодера
			 */
			sHandle = pHandle + varParam;
			if(bigstepUp != 0){
				enstatus = enBigStepUp(sHandle);
				bigstepUp = 0;
			}else if(bigstepDown != 0){
				enstatus = enBigStepDown(sHandle);
				bigstepDown = 0;
			}else if(setDef != 0){
				enWriteVal(sHandle, &(sHandle)->def);
				enstatus = enCharge;
				setDef = 0;
			}else{
				enstatus = enUpDate(sHandle);
			}
			if((enstatus == enLimDown) || (enstatus == enLimUp)){
				BeepTime(ui.beep.encoLim.time, ui.beep.encoLim.freq);
			}else if((enstatus == enTransitionDown) || (enstatus == enTransitionUp)){
				BeepTime(ui.beep.encoTransition.time, ui.beep.encoTransition.freq);
			}
		}

		/***************************************
		 * Задание регулятора
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
		 * Перекладываем измеренные данные
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
		 * Вывод на дисплей
		 */
		//Ставим прочерк, если режим ch_modeCurrent
		if((ch.mode == ch_modeCurrent) && (varParam == C_TIME)){
			varParam++;	                                    //Не разрешаем фоновое редактирование TIME
		}
		//Печать значения напряжения
		if(fp.tf.state.bit.switchIsON != 0){
			sprintf(str, "U:        %02u.%03u", measV / 1000000, (measV / 1000) % 1000);
		}else{
			sprintf(str, "U:        %02u.%03u", ch.u / 1000, ch.u % 1000);
		}
		if((varParam == C_VOLT) && (fp.tf.state.bit.switchIsON == 0)){
			lcd_setColor(black, ui.color.cursor);
		}else{
			lcd_setColor(black, ui.color.voltage);
		}
		lcd_putStr(10, 00, &arial, 0, str);

		//Печать значения тока
		if(fp.tf.state.bit.switchIsON != 0){
			sprintf(str, "I:         %01u.%03u A", measI / 1000000, (measI / 1000) % 1000);
		}else{
			sprintf(str, "I:         %01u.%03u A", ch.i / 1000, ch.i % 1000);
		}
		if((varParam == C_CURR) && (fp.tf.state.bit.switchIsON == 0)){
			lcd_setColor(black, ui.color.cursor);
		}else{
			lcd_setColor(black, ui.color.current);
		}
		lcd_putStr(10, 20, &arial, 0, str);

		//Time
		if(ch.mode == ch_modeTime){
			if(fp.tf.state.bit.switchIsON != 0){
				sprintf(str, "Time:  %um %02us   ", (ch.t * 60 - fp.tf.meas.time) / 60, (ch.t * 60 - fp.tf.meas.time) % 60);
			}else{
				sprintf(str, "Time:  %um 00s     ", ch.t);
			}
		}else{
			if(fp.tf.state.bit.switchIsON != 0){
				sprintf(str, "Time:  %um %02us   ", (fp.tf.meas.time) / 60, (fp.tf.meas.time) % 60);
			}else{
				sprintf(str, "Time:  - - -            ");
			}

		}
		if((varParam == C_TIME) && (fp.tf.state.bit.switchIsON == 0)){
			lcd_setColor(black, ui.color.cursor);
		}else{
			lcd_setColor(black, ui.color.current);
		}
		lcd_putStr(10, 40, &arial, 0, str);

		//Печать значения режима
		if(ch.mode == ch_modeTime){
			sprintf(str, "Mode: TIME         ");
		}else{
			sprintf(str, "Mode: VOLTAGE");
		}
		if((varParam == C_MODE) && (fp.tf.state.bit.switchIsON == 0)){
			lcd_setColor(black, ui.color.cursor);
		}else{
			lcd_setColor(black, ui.color.mode);
		}
		lcd_putStr(10, 60, &arial, 0, str);

		//Печать значения емкости
		sprintf(str, "C:         %01u.%03u A/h", fp.tf.meas.capacity / 1000, fp.tf.meas.capacity % 1000);
		lcd_setColor(black, ui.color.capacity);
		lcd_putStr(10, 80, &arial, 0, str);

		//Детект окончания
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

		printStatusBar();

		/*************************************/
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(CH_TSK_PERIOD));
	}
}

/******************* (C) COPYRIGHT ********** END OF FILE ********* D_EL *****/

/*!****************************************************************************
 * @file		systemTSK.c
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		14.09.2015
 * @copyright	GNU Lesser General Public License v3
 * @brief		System control task
 */

/*!****************************************************************************
 * Include
 */
#include "systemTSK.h"

/*!****************************************************************************
 * Memory
 */
frontPanel_type fp; ///< Data structure front panel

/*!****************************************************************************
 * Local prototypes for the functions
 */
void loadParameters(void);

/*!****************************************************************************
 * @brief
 */
void systemTSK(void *pPrm){
	TickType_t 		xLastWakeTime = xTaskGetTickCount();
	selWindow_type 	selWindowPrev = noneWindow;
	BaseType_t 		Result = pdTRUE;

	loadParameters();
	pvd_setSuplyFaultCallBack(shutdown);

	while(1){
		if(selWindowPrev != fp.currentSelWindow){
			if(windowTskHandle != NULL){
				vTaskDelete(windowTskHandle);	//Удаляем текущее окно
			}

			switch(fp.currentSelWindow){
				case startupWindow:
					Result = xTaskCreate(startupTSK, "startupTSK", STARTUP_TSK_SZ_STACK, NULL, STARTUP_TSK_PRIO, &windowTskHandle);
					break;
				case settingWindow:
					Result = xTaskCreate(settingTSK, "settingTSK", SETT_TSK_SZ_STACK, NULL, SETT_TSK_PRIO, &windowTskHandle);
					break;
				case baseWindow:
					Result = xTaskCreate(baseTSK, "baseTSK", BASE_TSK_SZ_STACK, NULL, BASE_TSK_PRIO, &windowTskHandle);
					break;
				case chargerWindow:
					Result = xTaskCreate(chargeTSK, "chargeTSK", CHARG_TSK_SZ_STACK, NULL, CHARG_TSK_PRIO, &windowTskHandle);
					break;
				case cube3dWindow:
					Result = xTaskCreate(cube3dTSK, "cube3dTSK", CUBE_TSK_SZ_STACK, NULL, CUBE_TSK_PRIO, &windowTskHandle);
					break;
				default:
					while(1)
						;	//Fail windowTskHandle
			}

			if(Result != pdTRUE){
				while(1)
					;
			}

			selWindowPrev = fp.currentSelWindow;
		}

		/*********************
		 *
		 */
		if(wlan.wlanRxActive != 0){
			if(windowTskHandle != NULL){
				vTaskDelete(windowTskHandle);	//Удаляем текущее окно
			}

			lcd_setColor(black, white);
			lcd_fillScreen(black);

			while(1){
				if(wlan.wlanRxActive){
					if(wlan.task.request != setNone){
						sendCommand(wlan.task.request);
					}
					fp.tf.task.u = wlan.task.u;
					fp.tf.task.i = wlan.task.i;
					fp.tf.task.mode = wlan.task.mode;

					wlan.wlanRxActive = 0;
				}
				uint32_t 				measV;	//[uV]
				uint32_t 				measI;	//[uA]

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

				//Печать значения напряжения
				if(fp.tf.state.bit.switchIsON != 0){
					sprintf(str, "%02u.%03u", measV / 1000000, (measV / 1000) % 1000);
				}else{
					sprintf(str, "%02u.%03u", fp.tf.task.u / 1000000, (fp.tf.task.u / 1000) % 1000);
				}
				lcd_setColor(black, ui.color.voltage);
				lcd_putStr(16, 0, &dSegBold, 6, str);
				lcd_putChar(150, 18, &font8x12, 'V');

				//Печать значения тока
				lcd_setColor(black, ui.color.current);
				if(fp.tf.state.bit.switchIsON != 0){
					if(measI < 99000){
						sprintf(str, "%2u.%03u", measI / 1000, measI % 1000);
						lcd_putChar(150, 36, &font8x12, 'm');
						lcd_putChar(150, 49, &font8x12, 'A');
					}else{
						sprintf(str, "%2u.%03u", measI / 1000000, (measI / 1000) % 1000);
						lcd_putChar(150, 36, &font8x12, ' ');
						lcd_putChar(150, 49, &font8x12, 'A');
					}

				}else{
					strcpy(str, "--.---");
					lcd_putChar(150, 36, &font8x12, ' ');
					lcd_putChar(150, 49, &font8x12, ' ');
				}
				lcd_putStr(16, 36, &dSegBold, 6, str);

				sprintf(str, "u= %08u", wlan.task.u);
				lcd_putStr(0, 80, &font8x12, 0, str);
				sprintf(str, "i= %08u", wlan.task.i);
				lcd_putStr(0, 100, &font8x12, 0, str);

				vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(SYSTEM_TSK_PERIOD));
			}


			/*sprintf(str, "t= %08u", wlan.task.time);
			lcd_putStr(0, 30, &font8x12, 0, str);
			sprintf(str, "m= %08u", wlan.task.mode);
			lcd_putStr(0, 30, &font8x12, 0, str);*/
		}

		/**************************************
		 * Вызов периодических функций
		 */
		static uint8_t ledCount = 0;
		if(ledCount++ == 10){
			LED_ON();
			ledCount = 0;
		}
		if(ledCount == 1){
			LED_OFF();
		}
		if((ledCount == 2) && (uartTsk.state == uartConnect)){
			LED_ON();
		}
		if(ledCount == 3){
			LED_OFF();
		}

		/*************************************/
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(SYSTEM_TSK_PERIOD));
	}
}

/*!****************************************************************************
 * @brief	Load parameters from memory
 */
void loadParameters(void){
	nvMem_state_type nvMemState;

	prm_loadDefault();

	nvMem_init(&userConfRegion);
	nvMem_init(&userReservConfRegion);
	nvMem_init(&systemSettingRegion);

	nvMemState = nvMem_loadPrm(&systemSettingRegion);
	if(nvMemState != nvMem_ok){
		fp.state.sysSettingLoadDefault = 1;
	}
	nvMemState = nvMem_loadPrm(&userConfRegion);
	if(nvMemState != nvMem_ok){
		//Попробуем считать резервную копию
		nvMemState = nvMem_loadPrm(&userReservConfRegion);
		if(nvMemState != nvMem_ok){
			fp.state.userSettingLoadDefault = 1;
		}
	}else{
		nvMem_savePrm(&userReservConfRegion);
	}
}

/*!****************************************************************************
 * @brief	Select window task & wait selectable
 * 			This function need call from current GUI window
 * @param 	window	window task identifier
 */
void selWindow(selWindow_type window){
	fp.currentSelWindow = window;
	while(windowTskHandle != NULL){
		vTaskDelay(1000);
	}
}

/*!****************************************************************************
 * Выключение
 */
void shutdown(void){
	pvd_disable();
	setLcdBrightness(0);
	LED_OFF();
	nvMem_savePrm(&userConfRegion);
	spfd_disable();
	BeepTime(ui.beep.goodbye.time, ui.beep.goodbye.freq);
	LED_ON();
	delay_ms(10000);
	NVIC_SystemReset();
}

/*************** LGPL ************** END OF FILE *********** D_EL ************/

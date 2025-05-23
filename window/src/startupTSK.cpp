/*!****************************************************************************
 * @file		startupTSK.c
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		02.08.2017
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		This task create start screen
 */

/*!****************************************************************************
 * Include
 */
#include <inttypes.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include <display.h>
#include <ledpwm.h>
#include <key.h>
#include <board.h>
#include <beep.h>
#include <version.h>
#include <systemTSK.h>
#include <prmSystem.h>
#include <regulatorConnTSK.h>
#include "ui.h"

/*!****************************************************************************
 * @brief	GUI invitation program task
 */
void startupTSK(void *pPrm){
	(void)pPrm;

	while(1){
		Prm::startcnt.val++;

		//Start Beep
		BeepTime(ui.beep.welcome.time, ui.beep.welcome.freq);
		LED_ON();
		vTaskDelay(pdMS_TO_TICKS(60));
		LED_OFF();

		disp_setColor(black, white);
		disp_fillScreen(black);
		disp_PrintImageMonochrome((DISP_W - ImageLogo.w) / 2, 3, &ImageLogo);  //Logo

		char str[64];
		regVersion_t regVersion;
		reg_getVersion(&regVersion);
		sprintf(str, "P%s : R%" PRIu16 ".%" PRIu16 ".%" PRIu16,
				getVersion(),
				regVersion.major, regVersion.minor, regVersion.patch);
		disp_putStr(0, 70, &arial, 0, str);

		if(fp.state.sysSettingLoadDefault == 0){
			sprintf(str, "%" PRIu32, Prm::startcnt.val);
		}else{
			sprintf(str, "-");
		}
		disp_putStr(0, 90, &arial, 0, str);
		disp_putStr(0, 110, &arial, 0, "2012 - 2025");
		disp_flush();


		for(uint16_t i = 0; i < Prm::brightness.val * 10; i++){
			setLcdBrightness(i);
			vTaskDelay(pdMS_TO_TICKS(2));
		}
		vTaskDelay(pdMS_TO_TICKS(150));

		//Run key process
		for(uint32_t cnt = 0; cnt < KEY_SAMPLES * 2; cnt++){
			keyProc();
			if(keyState(kMode) != 0){
				break;
			}
		}

		//Show default settings load message
		if((fp.state.sysSettingLoadDefault != 0)||((fp.state.userSettingLoadDefault != 0))){
			disp_fillScreen(black);
			if(fp.state.sysSettingLoadDefault != 0){
				disp_putStr(00, 00, &arial, 0, "Load default system");
				disp_putStr(00, 20, &arial, 0, "setting");
			}
			if(fp.state.userSettingLoadDefault != 0){
				disp_putStr(00, 40, &arial, 0, "Load default user");
				disp_putStr(00, 60, &arial, 0, "setting");
			}
			disp_flush();
			while(keyProc() == 0){
				vTaskDelay(pdMS_TO_TICKS(10));
			}
			BeepTime(ui.beep.key.time, ui.beep.key.freq);
		}

		if(keyState(kMode)){
			selWindow(settingWindow);
		}else{
			selWindow(baseWindow);
		}
	}
}

/******************************** END OF FILE ********************************/

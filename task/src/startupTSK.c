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
#include "stdio.h"
#include "ledpwm.h"
#include "key.h"
#include "startupTSK.h"
#include "board.h"

/*!****************************************************************************
 * MEMORY
 */
uint32_t startCounter;	///< Device start counter

/*!****************************************************************************
 * @brief	GUI invitation program task
 */
void startupTSK(void *pPrm){
	char str[30];

	while(1){
		startCounter++;

		//Start Beep
		BeepTime(ui.beep.welcome.time, ui.beep.welcome.freq);
		LED_ON();
		vTaskDelay(pdMS_TO_TICKS(60));
		LED_OFF();

		disp_setColor(black, white);
		disp_fillScreen(black);
		disp_PrintImageMonochrome((DISP_W - ImageLogo.w) / 2, 3, black, white, &ImageLogo);  //Logo

		if(fp.state.sysSettingLoadDefault == 0){
			sprintf(str, "COUNT %u", startCounter);
		}else{
			sprintf(str, "COUNT -");
		}

		disp_putStr(00, 70, &arial, 0, str);
		disp_putStr(0, 90, &arial, 0, fwinfoVersion);
		disp_putStr(0, 110, &arial, 0, "2012 - 2018");

		if(fp.fpSet.lcdLight < 10){
			fp.fpSet.lcdLight = 10;
		}
		for(uint16_t i = 0; i < fp.fpSet.lcdLight; i++){
			setLcdBrightness(i);
			vTaskDelay(pdMS_TO_TICKS(3));
		}
		vTaskDelay(pdMS_TO_TICKS(150));
		setLcdBrightness(fp.fpSet.lcdLight);

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

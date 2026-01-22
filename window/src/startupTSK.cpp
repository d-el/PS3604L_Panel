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
#include <hal/ledpwm.h>
#include <hal/key.h>
#include <hal/board.h>
#include <hal/beep.h>
#include <version.h>
#include <systemTSK.h>
#include <prmSystem.h>
#include <regulatorConnTSK.h>
#include "ui.h"

/*!****************************************************************************
 * @brief	GUI invitation program task
 */
void startupTSK(void *pPrm){
	Disp& disp = *(Disp*)pPrm;

	while(1){
		Prm::startcnt.val++;

		//Start Beep
		BeepTime(Prm::bpWelcomeOnOff ? ui.beep.welcome.time : 0, ui.beep.welcome.freq);
		LED_ON();
		disp.setColor(black, white);
		disp.fillScreen(black);
		disp.printImageMonochrome((disp.getWith() - ImageLogo.w) / 2, 0, &ImageLogo);  //Logo

		char str[64];
		regVersion_t regVersion;
		reg_versionGet(&regVersion);
		sprintf(str, "P%s : R%" PRIu16 ".%" PRIu16 ".%" PRIu16,
				getVersion(),
				regVersion.major, regVersion.minor, regVersion.patch);
		disp.putStr(0, 70, &arial, str);

		if(fp.state.sysSettingLoadDefault == 0){
			sprintf(str, "%" PRIu32, Prm::startcnt.val);
		}else{
			sprintf(str, "-");
		}
		disp.putStr(0, 90, &arial, str);
		disp.putStr(0, 110, &arial, "2012 - 2026");
		disp.flush();


		for(uint16_t i = 0; i < Prm::brightness.val * 10; i++){
			disp.setBrightness(i);
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
			disp.fillScreen(black);
			if(fp.state.sysSettingLoadDefault != 0){
				disp.putStrCenter(00, &arial, "Load default system");
				disp.putStrCenter(16, &arial, "setting");
			}
			if(fp.state.userSettingLoadDefault != 0){
				disp.putStrCenter(40, &arial, "Load default user");
				disp.putStrCenter(56, &arial, "setting");
			}
			saveparametersSystem();
			disp.putStr(00, 112, &arial, "Press any key");
			disp.flush();
			while(keyProc() == 0){
				vTaskDelay(pdMS_TO_TICKS(10));
			}
			BeepTime(ui.beep.key.time, ui.beep.key.freq);
		}

		selWindow(baseWindow);
	}
}

/******************************** END OF FILE ********************************/

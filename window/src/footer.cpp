/*!****************************************************************************
 * @file		footer.c
 * @author		d_el
 * @version		V1.0
 * @date		Dec 20, 2023
 * @copyright	License (MIT). Copyright (c) 2023 Storozhenko Roman
 * @brief
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
#include "display.h"
#include "graphics.h"
#include "regulatorConnTSK.h"
#include "systemTSK.h"

/*!****************************************************************************
 * @brief
 */
void printFooter(void){
	static uint8_t modeIlimPrev = 0;
	static bool enablePrev = 0;
	static uint16_t disablecausePrev = v_none;
	static char str[30];

	//Print line
	grf_line(0, 107, 159, 107, halfLightGray);

	regState_t regmeas = {};
	bool regstate = reg_getState(&regmeas);
	bool enable;
	reg_getEnable(&enable);

	if(modeIlimPrev != regmeas.status.m_limitation){
		if(regmeas.status.m_limitation != 0){
			BeepTime(ui.beep.cvToCc.time, ui.beep.cvToCc.freq);
		}else{
			BeepTime(ui.beep.ccToCv.time, ui.beep.ccToCv.freq);
		}
		modeIlimPrev = regmeas.status.m_limitation;
	}

	if(disablecausePrev != regmeas.disablecause && regmeas.disablecause == v_overCurrent){
		BeepTime(ui.beep.ovfCurrent.time, ui.beep.ovfCurrent.freq);
	}
	disablecausePrev = regmeas.disablecause;

	if(regmeas.status.m_errorTemperatureSensor || regmeas.status.m_overheated || regmeas.status.m_reverseVoltage ||
			regmeas.status.m_errorExternalIAdc || regmeas.status.m_notCalibrated || !regstate){
		BeepTime(ui.beep.error.time, ui.beep.error.freq);
		disp_setColor(black, white);

		if(!regstate){
			disp_putStr(35, 112, &arial, 0, "No Connect");
		}
		else if(regmeas.status.m_errorTemperatureSensor){
			disp_putStr(16, 112, &arial, 0, "Err Temp Sensor");
		}
		else if(regmeas.status.m_overheated != 0){
			disp_putStr(40, 112, &arial, 0, "Overheat");
		}
		else if(regmeas.status.m_reverseVoltage != 0){
			disp_putStr(16, 112, &arial, 0, "Reverse Voltage");
		}
		else if(regmeas.status.m_errorExternalIAdc){
			disp_putStr(16, 112, &arial, 0, "Error External IAdc");
		}
		else if(regmeas.status.m_notCalibrated){
			disp_putStr(8, 112, &arial, 0, "Error Not Calibrated");
		}
	}else{	//Not failure
		disp_setColor(black, white);

		//Print output power
		snprintf(str, sizeof(str), "%02" PRIu32 ".%03" PRIu32 " W", regmeas.power / 1000, regmeas.power % 1000);
		disp_putStr(0, 110, &font6x8, 0, str);

		//Print load resistance
		if(regmeas.resistance != 999990000){
			if(regmeas.resistance < 9999){
				snprintf(str, sizeof(str), "%03" PRIu32 ".%01" PRIu32 " m\xB1", regmeas.resistance / 10, regmeas.resistance % 10);
			}else if(regmeas.resistance < 99'9999){
				auto resistance = regmeas.resistance / 10;
				snprintf(str, sizeof(str), "%02" PRIu32 ".%03" PRIu32 " \xB1", resistance / 1000, resistance % 1000);
			}else{
				snprintf(str, sizeof(str), "%06" PRIu32 " \xB1", (regmeas.resistance + 5000) / 10000);
			}
			disp_putStr(0, 120, &font6x8, 0, str);
		}else{
			disp_putStr(0, 120, &font6x8, 0, " ---   \xB1");
		}

		//Print regulator temperature
		snprintf(str, sizeof(str), "%02" PRIu16 ".%" PRIu16 "\xB0\x43", regmeas.temperature / 10, regmeas.temperature % 10);
		disp_putStr(60, 120, &font6x8, 0, str);

		//Print time
		struct tm tm;
		time_t unixTime = time(NULL);
		localtime_r(&unixTime, &tm);
		strftime(str, sizeof(str), "%H:%M:%S", &tm);
		disp_putStr(110, 110, &font6x8, 0, str);
		strftime(str, sizeof(str), "%d.%m.%y", &tm);
		disp_putStr(110, 120, &font6x8, 0, str);

		//Print LAN status
		static TickType_t xTime;
		static uint8_t ledon = 0;
		if(fp.state.lanLink != 0){
			if(fp.state.httpactiv != 0){
				xTime = xTaskGetTickCount();
				ledon = 1;
				fp.state.httpactiv = 0;
			}
			if((ledon != 0)&&((xTaskGetTickCount() - xTime) >= 200)){
				ledon = 0;
			}

			if(ledon == 0){
				disp_setColor(black, white);
			}else{
				disp_setColor(black, red);
			}

			snprintf(str, sizeof(str), "LAN");
			disp_putStr(60, 110, &font6x8, 0, str);
		}
		else{
			snprintf(str, sizeof(str), "    ");
			disp_putStr(69, 110, &font6x8, 0, str);
		}

		if(reg_getremote()){
			disp_setColor(black, white);
			snprintf(str, sizeof(str), "REM");
			disp_putStr(85, 110, &font6x8, 0, str);
		}
	}
}

/******************************** END OF FILE ********************************/

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
#include <stdlib.h>
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
			regmeas.status.m_errorExternalIAdc || regmeas.status.m_calibrationEmpty || !regstate){
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
			disp_putStr(2, 112, &arial, 0, "Error External ADC");
		}
		else if(regmeas.status.m_calibrationEmpty){
			disp_putStr(2, 112, &arial, 0, "Err Calibration Empty");
		}
	}else{	//Not failure
		disp_setColor(black, white);

		//Print output power
		if(regmeas.power < 100000){
			snprintf(str, sizeof(str), "%02" PRIu32 ".%03" PRIu32 " mW", regmeas.power / 1000, regmeas.power % 1000);
		}else{
			auto power = (regmeas.power + 500) / 1000;
			snprintf(str, sizeof(str), "%03" PRIu32 ".%03" PRIu32 " W", power / 1000, power % 1000);
		}
		disp_putStr(0, 110, &font6x8, 0, str);

		//Print load resistance
		if(regmeas.resistance > 0){
			if(regmeas.resistance < 9999){
				snprintf(str, sizeof(str), "%03" PRIi32 ".%01" PRIi32 "  m\xB1", regmeas.resistance / 10, regmeas.resistance % 10);
			}else if(regmeas.resistance < 999'9999){
				auto resistance = (regmeas.resistance + 5) / 10;
				snprintf(str, sizeof(str), "%02" PRIi32 ".%03" PRIi32 "  \xB1", resistance / 1000, resistance % 1000);
			}else{
				auto resistance = (regmeas.resistance + 5000) / 10000;
				snprintf(str, sizeof(str), "%02" PRIi32 ".%03" PRIi32 " k\xB1", resistance / 1000, resistance % 1000);
			}
			disp_putStr(0, 120, &font6x8, 0, str);
		}else{
			disp_putStr(0, 120, &font6x8, 0, " ---    \xB1");
		}

		//Print regulator temperature
		bool minus = false;
		int16_t abst = abs(regmeas.temp_heatsink);
		snprintf(str, sizeof(str), "%2" PRIi16 ".%" PRIu16 "\xB0\x43", regmeas.temp_heatsink / 10, abst % 10);
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
			snprintf(str, sizeof(str), "RMT");
			disp_putStr(85, 110, &font6x8, 0, str);
		}
	}
}

/******************************** END OF FILE ********************************/

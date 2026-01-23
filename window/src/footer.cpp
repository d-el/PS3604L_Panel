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
#include <plog.h>
#include "ui.h"
#include "hal/rtc.h"
#include "hal/beep.h"
#include "regulatorConnTSK.h"
#include "systemTSK.h"
#include <prmSystem.h>
#include <lwip/tcpip.h>

/*!****************************************************************************
 * @brief
 */
void printFooter(Disp& disp){
	static uint8_t modeIlimPrev = 0;
	static bool enablePrev = 0;
	static char str[30];

	//Print line
	disp.line(0, 107, 159, 107, halfLightGray);

	regState_t regmeas = {};
	bool regstate = reg_getState(&regmeas);

	if(modeIlimPrev != regmeas.status.m_limitation){
		if(regmeas.status.m_limitation != 0){
			BeepTime(Prm::bpCcCvOnOff ? ui.beep.cvToCc.time : 0, ui.beep.cvToCc.freq);
		}else{
			BeepTime(Prm::bpCcCvOnOff ? ui.beep.ccToCv.time : 0, ui.beep.ccToCv.freq);
		}
		modeIlimPrev = regmeas.status.m_limitation;
	}

	if(regmeas.status.m_errorTemperatureSensor || regmeas.status.m_overheated ||
			regmeas.status.m_errorExternalIAdc || regmeas.status.m_calibrationEmpty || !regstate){
		BeepTime(Prm::bpErrOnOff ? ui.beep.error.time : 0, ui.beep.error.freq);
		disp.setColor(black, white);

		if(!regstate){
			disp.putStr(35, 112, &arial, "No Connect");
		}
		else if(regmeas.status.m_errorTemperatureSensor){
			disp.putStr(16, 112, &arial, "Err Temp Sensor");
		}
		else if(regmeas.status.m_overheated != 0){
			disp.putStr(40, 112, &arial, "Overheat");
		}
		else if(regmeas.status.m_errorExternalIAdc){
			disp.putStr(2, 112, &arial, "Error External ADC");
		}
		else if(regmeas.status.m_calibrationEmpty){
			disp.putStr(2, 112, &arial, "Err Calibration Empty");
		}
	}else{	//Not failure
		disp.setColor(black, white);

		//Print output power
		if(regmeas.power < 100000){
			snprintf(str, sizeof(str), "%02" PRIu32 ".%03" PRIu32 " mW", regmeas.power / 1000, regmeas.power % 1000);
		}else{
			auto power = (regmeas.power + 500) / 1000;
			snprintf(str, sizeof(str), "%03" PRIu32 ".%03" PRIu32 " W", power / 1000, power % 1000);
		}
		disp.putStr(0, 110, &font6x8, str);

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
			disp.putStr(0, 120, &font6x8, str);
		}else{
			disp.putStr(0, 120, &font6x8, " ---    \xB1");
		}

		//Print regulator temperature
		bool minus = false;
		int16_t abst = abs(regmeas.temp_heatsink);
		snprintf(str, sizeof(str), "%2" PRIi16 ".%" PRIu16 "\xB0\x43", regmeas.temp_heatsink / 10, abst % 10);
		disp.putStr(60, 120, &font6x8, str);

		//Print time
		struct tm tm;
		time_t unixTime = time(NULL);
		localtime_r(&unixTime, &tm);
		strftime(str, sizeof(str), "%H:%M:%S", &tm);
		disp.putStr(110, 110, &font6x8, str);
		strftime(str, sizeof(str), "%d.%m.%y", &tm);
		disp.putStr(110, 120, &font6x8, str);

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
				disp.setColor(black, white);
			}else{
				disp.setColor(black, red);
			}
			disp.putStr(60, 110, &font6x8, "LAN");
		}

		if(reg_getremote()){
			disp.setColor(black, white);
			snprintf(str, sizeof(str), "RMT");
			disp.putStr(85, 110, &font6x8, str);
		}
	}
}

/******************************** END OF FILE ********************************/

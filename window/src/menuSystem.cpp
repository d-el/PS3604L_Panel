/*!****************************************************************************
 * @file		menuSystem.c
 * @author		Storozhenko Roman - D_EL
 * @version		V1.4
 * @date		04.09.2018
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		menu system
 */

/*!****************************************************************************
 * Include
 */
#include <time.h>
#include <inttypes.h>
#include <FreeRTOS.h>
#include <task.h>
#include <display.h>
#include <graphics.h>
#include <key.h>
#include <enco.h>
#include <beep.h>
#include "menuSystem.h"
#include "ui.h"

namespace Menu {

#define MENU_SCREEN_W       DISP_W
#define MENU_SCREEN_H       DISP_H
#define MENU_PATH_FONT      font6x8
#define MENU_PATH_H         9
#define MENU_PATH_CHAR_W    6
#define MENU_ITEM_FONT      font8x12
#define MENU_ITEM_H         13
#define MENU_ITEM_CHAR_W    8
#define MENU_MAXPATH        5
#define MENU_PERIOD			20

/*!****************************************************************************
 * @brief	Print message
 */
void printMessageWindow(const char *string){
	disp_fillScreen(black);
	disp_setColor(black, white);
	disp_putStr(0, MENU_ITEM_H, &MENU_ITEM_FONT, 0, string);
	disp_flushfill(&ui.color.background);
	while(keyProc() == 0){
		vTaskDelay(pdMS_TO_TICKS(MENU_PERIOD));
	}
	BeepTime(ui.beep.key.time, ui.beep.key.freq);
}

void callUnselect(const MenuItem *m){
	if(m->pUnselect && m->prm && !m->editor){
		ItemState res = m->pUnselect(m);
		if(!res.state){
			printMessageWindow(res.string);
		}
	}
}

void callSelect(const MenuItem *m){
	if(m->pSelect && m->prm && !m->editor){
		ItemState res = m->pSelect(m);
		if(!res.state){
			printMessageWindow(res.string);
		}
	}
}

void callExit(const MenuItem *m){
	if(m->pUnselect){
		ItemState res = m->pUnselect(m);
		if(!res.state){
			printMessageWindow(res.string);
		}
	}
}

/*!****************************************************************************
 * @brief	Call function on Enter
 */
void callEnter(const MenuItem *m){
	if(m->pSelect){
		ItemState res = m->pSelect(m);
		if(!res.state){
			printMessageWindow(res.string);
		}
	}
}

/*!****************************************************************************
 * @brief	Call function on Change parameter
 */
void callChanges(const MenuItem *m){
	if(m->pChange){
		ItemState res = m->pChange(m);
		if(!res.state){
			printMessageWindow(res.string);
		}
	}
}

typedef enum {
    menuItemUnselect,
    menuItemSelect,
    menuItemUnselectUnchangeable,
    menuItemSelectUnchangeable
} menuItemSelect_type;

static const disp_color_type colorLabel[] = {
    [menuItemUnselect] = white,
    [menuItemSelect] = red,
    [menuItemUnselectUnchangeable] = white,
    [menuItemSelectUnchangeable] = red,
};

static const disp_color_type colorValue[] = {
    [menuItemUnselect] = white,
    [menuItemSelect] = red,
    [menuItemUnselectUnchangeable] = white,
    [menuItemSelectUnchangeable] = white,
};

/*!****************************************************************************
 * @brief	Call function Periodic
 */
void callPeriodic(const MenuItem *m){
	if(m->pPeriod != NULL){
		ItemState res = m->pPeriod(m);
		if(!res.state){
			printMessageWindow(res.string);
		}
	}
}

/*!****************************************************************************
 */
void outItemString(const MenuItem *m, uint8_t itemNumber, bool select){
	char string[MENU_SCREEN_W / MENU_ITEM_CHAR_W + 1];
	char value[64];
	value[0] = '\0';
	if(m->prm) m->prm->tostring(value, sizeof(value));

	menuItemSelect_type sel = select ?
				m->change ? menuItemSelect: menuItemSelectUnchangeable
				:
				m->change ? menuItemUnselect: menuItemUnselectUnchangeable;

	// Delimiter
	grf_line(0, MENU_PATH_H + itemNumber * MENU_ITEM_H - 1,
	            MENU_SCREEN_W - 1, MENU_PATH_H + itemNumber * MENU_ITEM_H - 1, halfLightGray);

	// Label and spaces
	strcpy(string, m->label);
	memset(string + strlen(m->label), ' ', sizeof(string) - 1 - strlen(m->label));
	string[sizeof(string) -1 - strlen(value)] = 0;
    disp_setContentColor(colorLabel[sel]);
    disp_putStr(0, MENU_PATH_H + MENU_ITEM_H * itemNumber, &MENU_ITEM_FONT, 0, string);

    // Value
    if(!m->editor){
    	disp_setContentColor(colorValue[sel]);
		disp_putStr(MENU_ITEM_CHAR_W * strlen(string), MENU_PATH_H + MENU_ITEM_H * itemNumber, &MENU_ITEM_FONT, 0, value);
    }
}

/*!****************************************************************************
 */
void outItemString(const char *string, uint8_t itemNumber, uint8_t selectPosition, uint8_t unselectPosition){
	char s[MENU_SCREEN_W / MENU_ITEM_CHAR_W + 1];
	uint8_t offset = 0;

	grf_line(0, MENU_PATH_H + itemNumber * MENU_ITEM_H - 1,
		            MENU_SCREEN_W - 1, MENU_PATH_H + itemNumber * MENU_ITEM_H - 1, halfLightGray);

	memset(s, '\0', sizeof(s));
	strncpy(s, string, selectPosition);
	disp_setContentColor(colorValue[menuItemUnselect]);
	disp_putStr(0, MENU_PATH_H + MENU_ITEM_H * itemNumber, &MENU_ITEM_FONT, 0, s);
	offset = selectPosition;

	memset(s, '\0', sizeof(s) - offset);
	strncpy(s, &string[selectPosition], unselectPosition - selectPosition);
	disp_setContentColor(colorValue[menuItemSelect]);
	disp_putStr(offset * MENU_ITEM_CHAR_W, MENU_PATH_H + MENU_ITEM_H * itemNumber, &MENU_ITEM_FONT, 0, s);
	offset += unselectPosition - selectPosition;

	strncpy(s, &string[unselectPosition], sizeof(s) - offset);
	disp_setContentColor(colorValue[menuItemUnselect]);
	disp_putStr(offset * MENU_ITEM_CHAR_W, MENU_PATH_H + MENU_ITEM_H * itemNumber, &MENU_ITEM_FONT, 0, s);
}

/*!****************************************************************************
 */
void printHistory(const MenuItem* history[], uint8_t historyDepth){
	char s[MENU_SCREEN_W / MENU_ITEM_CHAR_W + 1] = "/";
	uint8_t offset = 0;
	for(size_t i = 0; i < historyDepth; i++){
		offset += snprintf(&s[offset], sizeof(s) - offset, "/%s", history[i]->label);
	}
	disp_setContentColor(green);
	disp_putStr(0, 0, &MENU_PATH_FONT, 0, s);
}

/*!****************************************************************************
 */
bool run(const MenuItem *m){
	const MenuItem* history[6] = { };
	uint8_t historyIndex = 0;
	const MenuItem *top = m;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	bool change = false;
	disp_setColor(black, white);
	disp_fillScreen(black);
	ksSet(15, 5, kUp | kDown);
	enco_settic(3);

	while(1){
		if(keyProc() != 0){
			BeepTime(ui.beep.key.time, ui.beep.key.freq);
			if(keyState(kUp)){
				if(m->prm) m->prm->bigstep(1);
				change = true;
			}
			else if(keyState(kDown)){
				if(m->prm) m->prm->bigstep(-1);
				change = true;
			}
			else if(keyState(kZero)){
				if(m->prm) m->prm->setdef();
				change = true;
			}

			//Previous
			if(keyState(kView)){
				if(m->previous){
					callUnselect(m);
					m = m->previous;
					callSelect(m);
				}
			}
			//Next
			if(keyState(kSet)){
				if(m->next){
					callUnselect(m);
					m = m->next;
					callSelect(m);
				}
			}
			//Parent
			if(keyState(kMode)){
				if(historyIndex){
					callExit(m);
					m = history[--historyIndex];
					for(top = m; top->previous; top = top->previous);
					//callEnter(m);
				}
				else{
					return true; //Exit
				}
			}
			//Child
			if(keyState(kOnOff)){
				if(m->editor){
					callEnter(m);
					history[historyIndex++] = m;
					m->editor(history, historyIndex);
					callExit(m);
					historyIndex--;
				}
				else if(m->child){
					//callExit(m);
					callEnter(m);
					history[historyIndex++] = m;
					m = m->child;
					top = m;
					callSelect(m);
				}
			}
		}

		if(m->change){
			auto step = enco_update();
			if(m->prm) m->prm->step(step);
		}

		if(change) callChanges(m);
		callPeriodic(m);

		printHistory(history, historyIndex);
		const MenuItem *w = top;
		uint8_t numItem = 0;
		while(w){
			outItemString(w, numItem++, w == m);
			w = w->next;
		}
		disp_flushfill(&ui.color.background);
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(MENU_PERIOD));
	}
}

/*!****************************************************************************
 */
ItemState clockEditor(const MenuItem* history[], uint8_t historyIndex){
	const MenuItem* m = history[historyIndex - 1];
	TickType_t xLastWakeTime = xTaskGetTickCount();
	enum { day, mon, yaer, hour, min, sec, number };
	uint8_t var = day;
	struct Limit{ int32_t min, max; };
	const Limit limits[] = {
			1,31, 1,12, 21,99, 0,23, 0,59, 0,59
	};

	struct tm t = {};
	time_t unixTime = static_cast<Prm::Val<uint32_t>*>(m->prm)->val;
	///localtime_r(&unixTime, &t);
	gmtime_r(&unixTime, &t);

	union Val{
		struct{ int32_t mday, mon, year, hour, min, sec; };
		int32_t v[6];
	};

	Val val = { t.tm_mday, t.tm_mon + 1, t.tm_year - 100, t.tm_hour, t.tm_min, t.tm_sec };

	while(1){
		if(keyProc() != 0){
			BeepTime(ui.beep.key.time, ui.beep.key.freq);

			if(keyState(kUp)){
				if(var < sec) var++;
			}
			else if(keyState(kDown)){
				if(var > day) var--;
			}

			//Parent
			if(keyState(kMode)){
				//callExit(m);
				break;
			}
		}

		auto isLeapYear = [](int y) -> bool{
		    return  !( y%4 ) && ( y%100 || !( y%400 ) );
		};

		auto daysInMonth = [isLeapYear](int month, int year) -> int {
			static const int daysInMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
			if (month == 2 && isLeapYear(year))
			        return 29;
			return daysInMonth[month - 1];
		};

		val.v[var] += enco_update();
		if(val.v[var] < limits[var].min) val.v[var] = limits[var].min;
		if(val.v[var] > limits[var].max) val.v[var] = limits[var].max;

		int monlimit = daysInMonth(val.mon, val.year + 2000);
		if(val.v[day] > monlimit) val.v[day] = monlimit;

		printHistory(history, historyIndex);

		char str[64];

		Limit hilightsDate[6] = { 5,7, 8,10, 11,13, 0,0, 0,0, 0,0 };
		snprintf(str, sizeof(str), "date %02" PRIi32 ".%02" PRIi32 ".%02" PRIi32, val.mday, val.mon, val.year);
		outItemString(str, 0, hilightsDate[var].min, hilightsDate[var].max);

		Limit hilightsTime[6] = { 0,0, 0,0, 0,0, 5,7, 8,10, 11,13 };
		snprintf(str, sizeof(str), "time %02" PRIi32 ":%02" PRIi32 ":%02" PRIi32, val.hour, val.min, val.sec);
		outItemString(str, 1, hilightsTime[var].min, hilightsTime[var].max);

		disp_flushfill(&ui.color.background);
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(MENU_PERIOD));
	}

	t.tm_mday = val.mday;
	t.tm_mon = val.mon - 1;
	t.tm_year = val.year + 100;
	t.tm_hour = val.hour;
	t.tm_min = val.min;
	t.tm_sec = val.sec;
	static_cast<Prm::Val<uint32_t>*>(m->prm)->val = mktime(&t);
	return ItemState{  true, "" };
}

/*!****************************************************************************
 */
ItemState ipAddressEditor(const MenuItem* history[], uint8_t historyIndex){
	const MenuItem* m = history[historyIndex - 1];
	enum { min = 0, max = 3 };
	uint8_t var = min;
	union { uint32_t ip; uint8_t v[4]; } ip = { static_cast<Prm::Val<uint32_t>*>(m->prm)->val };
	TickType_t xLastWakeTime = xTaskGetTickCount();
	while(1){
		if(keyProc() != 0){
			BeepTime(ui.beep.key.time, ui.beep.key.freq);

			if(keyState(kUp)){
				if(var < max) var++;
			}
			else if(keyState(kDown)){
				if(var > min) var--;
			}

			//Parent
			if(keyState(kMode)){
				callExit(m);
				break;
			}
		}

		ip.v[max - var] += enco_update();

		printHistory(history, historyIndex);

		char str[64];
		size_t select = 0, unselect = 0, offset = 0;
		for(size_t i = 0; i < 4; i++){
			if(var == i) select = offset + (i == 0 ? 0 : 1);
			offset += snprintf(&str[offset], sizeof(str) - offset, i == 0 ? "%u" : ":%u", ip.v[max - i]);
			if(var == i) unselect = offset;
		}

		outItemString(str, 0, select, unselect);
		disp_flushfill(&ui.color.background);
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(MENU_PERIOD));
	}
	static_cast<Prm::Val<uint32_t>*>(m->prm)->val = ip.ip;
	return ItemState{  true, "" };
}

}

/******************************** END OF FILE ********************************/

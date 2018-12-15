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
#include <inttypes.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "FreeRTOS.h"
#include "task.h"
#include "menuSystem.h"
#include "settingTSK.h"
#include "ui.h"
#include "rtc.h"
#include "display.h"
#include "graphics.h"
#include "key.h"
#include "prmEditor.h"
#include "beep.h"
#include "systemTSK.h"

/*!****************************************************************************
 * Define
 */
#define MENU_SCREEN_W       DISP_W
#define MENU_SCREEN_H       DISP_H
#define MENU_PATH_FONT      font6x8
#define MENU_PATH_H         9
#define MENU_PATH_CHAR_W    6
#define MENU_ITEM_FONT      font8x12
#define MENU_ITEM_H         13
#define MENU_ITEM_CHAR_W    8
#define MENU_MAXPATH        5

/*!****************************************************************************
 * MEMORY
 */
#define MENU_ITEM(_name, _label, _units, _prmHandle, _chmod, _pfPrm, _pfChanges, _pfSelect, _pfUnselect, _pfPeriod, _previous, _next, _parent, _child) \
{														\
	.label					= _label,					\
	.units					= _units,					\
	.prmHandle				= _prmHandle,				\
	.flags.bit.chmod		= _chmod,					\
	.flags.bit.pfParamert	= _pfPrm,					\
	.pfChanges				= _pfChanges,				\
	.pfSelect				= _pfSelect,				\
	.pfUnselect				= _pfUnselect,				\
	.pfPeriod				= _pfPeriod,				\
	.previous				= &menuTree[mN_##_previous],\
	.next					= &menuTree[mN_##_next],	\
	.parent					= &menuTree[mN_##_parent],	\
	.child					= &menuTree[mN_##_child],	\
},
const menuItem_type menuTree[] = {
	#include "menuTree.h"
};
#undef MENU_ITEM

const menuItem_type *selectedTopMenuPath[MENU_MAXPATH];
const menuItem_type *selectedMenuPath[MENU_MAXPATH];

/*!****************************************************************************
 * Local function declaration
 */
void callSelect(const struct menuItem* item);
void callUnselect(const struct menuItem* item);
void callEnter(const struct menuItem* item);
void callExit(const struct menuItem* item);
void callChanges(const struct menuItem *item);
void callPeriodic(const struct menuItem *item);
void setLimit(const menuItem_type *menuItem, uint8_t editSection);
void printMenuPath(const menuItem_type **menuPath, const menuItem_type *selectedItem);
void printItem(const menuItem_type *menuItem, uint8_t itemNumber, menuItemSelect_type select, uint8_t selectedSectionNumber);
void printUsigVar(char *string, const menuItem_type *menuItem, uint32_t var);
void printSigVar(char *string, const menuItem_type *menuItem, int32_t var);
void printFloatVar(char *string, const menuItem_type *menuItem);
void printIpVar(char *string, const uint32_t ip, uint8_t editSectionNumber, uint8_t *selectionPosition, uint8_t *selectionLength);
void printDateVar(char *string, const time_t unixTime, uint8_t editSectionNumber, uint8_t *selectionPosition, uint8_t *selectionLength);
void printTimeVar(char *string, const time_t unixTime, uint8_t editSectionNumber, uint8_t *selectionPosition, uint8_t *selectionLength);
void outItemString(char *label, char *value, uint8_t itemNumber, menuItemSelect_type select);
void outItemStringWithSelection(char *label, char *value, uint8_t itemNumber, menuItemSelect_type select, uint8_t selectionPosition, uint8_t selectionLength);

/*!****************************************************************************
 * @brief
 * @param	startMenuItem	- starting menu item
 */
void menuEngine(menuItemNumber_type menuItemNumber){
	TickType_t			xLastWakeTime; //Вемя ОС

	const menuItem_type **topMenu = selectedTopMenuPath;
	const menuItem_type **sMenu = selectedMenuPath;
	const menuItem_type *sMenuPrev = NULL;
	uint8_t			numItems = 1;
	uint8_t			startItem = 0;
	uint8_t			bigstepUp = 0;
	uint8_t			bigstepDown = 0;
	uint8_t			setDef = 0;
	uint8_t			editSection = 0;

	*topMenu = &menuTree[menuItemNumber];
	*sMenu = *topMenu;

	xLastWakeTime = xTaskGetTickCount();
	disp_setColor(black, white);
	disp_fillScreen(black);
	ksSet(15, 5, kUp | kDown);
	prmEditorSetNtic(3);

	numItems = 1;
	for(const menuItem_type *m = *topMenu; m != m->next; m = m->next){
		numItems++;
	}

	while(1){
		/**************************************
		 * Processing on press button
		 */
		if(keyProc() != 0){
			BeepTime(ui.beep.key.time, ui.beep.key.freq);

			if(keyState(kUp)){
				bigstepUp = 1;
			}
			else if(keyState(kDown)){
				bigstepDown = 1;
			}
			else if(keyState(kZero)){
				setDef = 1;
			}

			//Previous
			if(keyState(kView) != 0 && *sMenu != NULL){
				const menuItem_type *item = *sMenu;
				if(item != item->previous){ //Begin of list
					item = item->previous;
					callUnselect(*sMenu);
					*sMenu = item;
					callSelect(*sMenu);
				}
			}

			//Next
			if(keyState(kSet) != 0 && *sMenu != NULL){
				const menuItem_type *item = *sMenu;
				if(item != item->next){ //End of list
					item = item->next;
					callUnselect(*sMenu);
					*sMenu = item;
					callSelect(*sMenu);
				}
			}

			//Parent
			if(keyState(kMode)){
				if(sMenu > selectedMenuPath){
					*topMenu = NULL;
					*sMenu = NULL;
					topMenu--;
					sMenu--;
					numItems = 1;
					for(const menuItem_type *m = *topMenu; m != m->next; m = m->next){
						numItems++;
					}
					callExit(*sMenu);
					disp_fillScreen(black);
				}
				else{
					return; //Exit
				}
			}

			//Child
			if(keyState(kOnOff) && *sMenu != NULL && *sMenu != (*sMenu)->child){
				callEnter(*sMenu);
				topMenu++;
				*topMenu = (*sMenu)->child;
				sMenu++;
				*sMenu = NULL;
				const menuItem_type *item = *topMenu;

				while(1){
					if(item->flags.bit.chmod == chmodMenuAlways){
						if(item->prmHandle != NULL){
							if(item->prmHandle->chmod == chmodAlways){
								*sMenu = item;			//Available select
								callSelect(*sMenu);
							}
							break;
						}else{
							*sMenu = item;				//Available select
							callSelect(*sMenu);
							break;
						}
					}
					if(item == item->next){ //End of list
						break;
					}
					item = item->next;
				}
				numItems = 1;
				for(const menuItem_type *m = *topMenu; m != m->next; m = m->next){
					numItems++;
				}
				disp_fillScreen(black);
			}
		}

		//Detection of menu item switching
		if(*sMenu != NULL && sMenuPrev != *sMenu){
			editSection = 0;
			if((*sMenu)->prmHandle != NULL && (*sMenu)->prmHandle->limType == prmLimVariable){
				setLimit((*sMenu), editSection);
			}
			sMenuPrev = *sMenu;
		}

		/************************
		 * Editor
		 */
		prmEditorStatus_type status = enNoCharge;
		const menuItem_type *eitem = *sMenu;
		if(eitem->prmHandle != NULL){
			if(eitem->flags.bit.chmod == chmodMenuAlways && eitem->prmHandle->chmod == chmodAlways){
				if(bigstepUp != 0){
					if(eitem->prmHandle->limType == prmLimVariable){
						if(editSection < 3){
							editSection++;
							setLimit(eitem, editSection);
						}
					}
					else{
						status = prmEditorBigStepUp(eitem->prmHandle);
					}
					bigstepUp = 0;
				}
				else if(bigstepDown != 0){
					if(eitem->prmHandle->limType == prmLimVariable){
						if(editSection > 0){
							editSection--;
							setLimit(eitem, editSection);
						}
					}else{
						status = prmEditorBigStepDown(eitem->prmHandle);
					}
					bigstepDown = 0;
				}else if(setDef != 0){
					prmEditorWriteVal(eitem->prmHandle, &eitem->prmHandle->def);
					status = enCharge;
					setDef = 0;
				}else{
					status = prmEditorUpDate(eitem->prmHandle);
				}
				if(status != enNoCharge && eitem->pfChanges != NULL){
					callChanges(eitem);
				}
				if((status == enLimDown) || (status == enLimUp)){
					BeepTime(ui.beep.encoLim.time, ui.beep.encoLim.freq);
				}else if((status == enTransitionDown) || (status == enTransitionUp)){
					BeepTime(ui.beep.encoTransition.time, ui.beep.encoTransition.freq);
				}
			}
		}

		/******************************
		 * Call periodic handler
		 */
		const menuItem_type *pitem = *topMenu;
		while(1){
			if(pitem == pitem->next){
				break;
			}
			if(pitem->pfPeriod != NULL){
				pitem->pfPeriod(pitem);
			}
			callPeriodic(pitem);
			pitem = pitem->next;
		}

		/******************************
		 * Print menu path
		 */
		printMenuPath(selectedMenuPath, *sMenu);

		/******************************
		 * Print menu item in this tree
		 */
		uint8_t numItems = 0;
		uint8_t selectedItem = 0;
		const menuItem_type *item1 = *topMenu;
		while(1){
			if(item1 == *sMenu){
				selectedItem = numItems;
			}
			if(item1 == item1->next){
				break;
			}
			item1 = item1->next;
			numItems++;
		}

		// Calculate start item
		uint8_t maItems = (MENU_SCREEN_H - MENU_PATH_H) / MENU_ITEM_H;
		if(selectedItem - startItem > maItems - 1){
			startItem++;
		}
		if(selectedItem < startItem){
			startItem = selectedItem;
		}

		// Skip
		uint8_t itemNum = 0;
		const menuItem_type *item = *topMenu;
		while(1){
			if(item == item->next || itemNum == startItem){
				break;
			}
			item = item->next;
			itemNum++;
		}

		// Print
		itemNum = 0;
		while(1){
			if(itemNum >= maItems){
				break;
			}
			menuItemSelect_type select;
            if(item != *sMenu){
                if(item->prmHandle == NULL || (item->prmHandle != NULL && item->flags.bit.chmod == chmodMenuAlways && item->prmHandle->chmod == chmodAlways)){
                    select = menuItemUnselect;
                }else{
                    select = menuItemUnselectUnchangeable;
                }
            }
            else if(item->prmHandle == NULL || (item->prmHandle != NULL && item->flags.bit.chmod == chmodMenuAlways && item->prmHandle->chmod == chmodAlways)){
                select = menuItemSelect;
		    }else{
		        select = menuItemSelectUnchangeable;
		    }

			printItem(item, itemNum, select, editSection);
			if(item == item->next){
				break;
			}
			item = item->next;
			itemNum++;
		}

		/*************************************/
		//disp_flush();
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(MENU_PERIOD));
	}
}

/*!****************************************************************************
 * @brief	Print message
 */
void printMessageWindow(char *string){
	disp_fillScreen(black);
	disp_setColor(black, white);
	disp_putStr(0, MENU_ITEM_H, &MENU_ITEM_FONT, 0, string);
	while(keyProc() == 0){
		vTaskDelay(pdMS_TO_TICKS(MENU_PERIOD));
	}
	BeepTime(ui.beep.key.time, ui.beep.key.freq);
}

/*!****************************************************************************
 * @brief	Call function on Select
 */
void callSelect(const struct menuItem* item){
	if(item->pfSelect != NULL && item->prmHandle != NULL){
		itemState_type res = item->pfSelect(item);
		if(res.state != menuItemOk){
			printMessageWindow(res.string);
		}
	}
}

/*!****************************************************************************
 * @brief	Call function on Unselect
 */
void callUnselect(const struct menuItem* item){
	if(item->pfUnselect != NULL && item->prmHandle != NULL){
		itemState_type res = item->pfUnselect(item);
		if(res.state != menuItemOk){
			printMessageWindow(res.string);
		}
	}
}

/*!****************************************************************************
 * @brief	Call function on Enter
 */
void callEnter(const struct menuItem* item){
	if(item->pfSelect != NULL && item->prmHandle == NULL){
		itemState_type res = item->pfSelect(item);
		if(res.state != menuItemOk){
			printMessageWindow(res.string);
		}
	}
}

/*!****************************************************************************
 * @brief	Call function on Exit
 */
void callExit(const struct menuItem *item){
	if(item->pfUnselect != NULL && item->prmHandle == NULL){
		itemState_type res = item->pfUnselect(item);
		if(res.state != menuItemOk){
			printMessageWindow(res.string);
		}
	}
}

/*!****************************************************************************
 * @brief	Call function on Change parameter
 */
void callChanges(const struct menuItem *item){
	if(item->pfChanges != NULL){
		itemState_type res = item->pfChanges(item);
		if(res.state != menuItemOk){
			printMessageWindow(res.string);
		}
	}
}

/*!****************************************************************************
 * @brief	Call function Periodic
 */
void callPeriodic(const struct menuItem *item){
	if(item->pfPeriod != NULL){
		itemState_type res = item->pfPeriod(item);
		if(res.state != menuItemOk){
			printMessageWindow(res.string);
		}
	}
}

/*!****************************************************************************
 * @param menuItem
 */
void setLimit(const menuItem_type *menuItem, uint8_t editSection){
	uint32_t unixTime;

	switch(menuItem->prmHandle->type){
		case ipAdrFrmt:
			menuItem->prmHandle->step->t_ipAdrFrmt = 1UL << (3 - editSection) * 8;
			menuItem->prmHandle->min->t_ipAdrFrmt = menuItem->prmHandle->prm->t_ipAdrFrmt & ~(255UL << (3 - editSection) * 8);
			menuItem->prmHandle->max->t_ipAdrFrmt = menuItem->prmHandle->prm->t_ipAdrFrmt | 255UL << (3 - editSection) * 8;
			break;
		case unixTimeFrmt:
			unixTime = menuItem->prmHandle->prm->t_unixTimeFrmt % (24 * 60 * 60);
			switch(editSection){
				case 0:	// Hour
					menuItem->prmHandle->step->t_unixDateFrmt = 60 * 60;
					menuItem->prmHandle->min->t_unixDateFrmt = menuItem->prmHandle->prm->t_unixTimeFrmt - unixTime;
					menuItem->prmHandle->max->t_unixDateFrmt = menuItem->prmHandle->prm->t_unixTimeFrmt - unixTime + 24 * 60 * 60 - 1;
					break;
				case 1:	// Minute
					menuItem->prmHandle->step->t_unixDateFrmt = 60;
					menuItem->prmHandle->min->t_unixDateFrmt = menuItem->prmHandle->prm->t_unixTimeFrmt - unixTime;
					menuItem->prmHandle->max->t_unixDateFrmt = menuItem->prmHandle->prm->t_unixTimeFrmt - unixTime + 24 * 60 * 60 - 1;
					break;
				case 2: // Second
					menuItem->prmHandle->step->t_unixDateFrmt = 1;
					menuItem->prmHandle->min->t_unixDateFrmt = menuItem->prmHandle->prm->t_unixTimeFrmt - unixTime;
					menuItem->prmHandle->max->t_unixDateFrmt = menuItem->prmHandle->prm->t_unixTimeFrmt - unixTime + 24 * 60 * 60 - 1;
					break;
			}
			break;

		default:
			break;
	}
}

/*!****************************************************************************
 * @param menuPath: pointer to menu path
 */
void printMenuPath(const menuItem_type **menuPath, const menuItem_type *selectedItem){
	char string[MENU_SCREEN_W / MENU_PATH_CHAR_W + 1];

	strcpy(string, "/");

	while(*menuPath != selectedItem){
		strcpy(string + strlen(string), (*menuPath)->label);
		menuPath++;
		if(*menuPath != selectedItem){
			strcpy(string + strlen(string), "/");
		}
	}
	memset(string + strlen(string), ' ', MENU_SCREEN_W / MENU_PATH_CHAR_W - strlen(string));
	string[MENU_SCREEN_W / MENU_PATH_CHAR_W] = 0;

	disp_setColor(black, green);
	disp_putStr(0, 0, &MENU_PATH_FONT, 0, string);
}

/*!****************************************************************************
 * @param [in] item: menu item handler
 * @param [in] itemNumber: item number in screen
 * @param [in] isSelected: 0 - item is unselected, 1 - item is selected
 * @param [in] selectedSectionNumber: selected section number in composite type (e.g. IP, Time)
 */
void printItem(const menuItem_type *menuItem, uint8_t itemNumber, menuItemSelect_type select, uint8_t selectedSectionNumber){
	char string[MENU_SCREEN_W / MENU_ITEM_CHAR_W + 1];
	char vstring[MENU_SCREEN_W / MENU_ITEM_CHAR_W + 1];
	uint8_t selectionPosition, selectionLength;

	if(menuItem != menuItem->child){
		snprintf(string, sizeof(string), "/%s", menuItem->label);
	}else{
		snprintf(string, sizeof(string), "%s", menuItem->label);
	}

	if(menuItem->prmHandle == NULL){
		outItemString(string, "", itemNumber, select);
	}
	else{
		switch(menuItem->prmHandle->type){
			case u8Frmt:
				printUsigVar(vstring, menuItem, menuItem->prmHandle->prm->t_u8Frmt);
				outItemString(string, vstring, itemNumber, select);
				break;
			case s8Frmt:
				printSigVar(vstring, menuItem, menuItem->prmHandle->prm->t_s8Frmt);
				outItemString(string, vstring, itemNumber, select);
				break;
			case u16Frmt:
				printUsigVar(vstring, menuItem, menuItem->prmHandle->prm->t_u16Frmt);
				outItemString(string, vstring, itemNumber, select);
				break;
			case s16Frmt:
				printSigVar(vstring, menuItem, menuItem->prmHandle->prm->t_s16Frmt);
				outItemString(string, vstring, itemNumber, select);
				break;
			case u32Frmt:
				printUsigVar(vstring, menuItem, menuItem->prmHandle->prm->t_u32Frmt);
				outItemString(string, vstring, itemNumber, select);
				break;
			case s32Frmt:
				printSigVar(vstring, menuItem, menuItem->prmHandle->prm->t_s32Frmt);
				outItemString(string, vstring, itemNumber, select);
				break;
			case floatFrmt:
				printFloatVar(vstring, menuItem);
				outItemString(string, vstring, itemNumber, select);
				break;
			case unixDateFrmt:
				printDateVar(vstring, menuItem->prmHandle->prm->t_unixDateFrmt, selectedSectionNumber, &selectionPosition, &selectionLength);
				outItemString(string, vstring, itemNumber, select);
				break;
			case unixTimeFrmt:
				printTimeVar(vstring, menuItem->prmHandle->prm->t_unixTimeFrmt, selectedSectionNumber, &selectionPosition, &selectionLength);
				outItemStringWithSelection(string, vstring, itemNumber, select, selectionPosition, selectionLength);
				break;
			case ipAdrFrmt:
				printIpVar(vstring, menuItem->prmHandle->prm->t_ipAdrFrmt, selectedSectionNumber, &selectionPosition, &selectionLength);
				outItemStringWithSelection(string, vstring, itemNumber, select, selectionPosition, selectionLength);
				break;
			default:
				outItemString(string, "Error", itemNumber, select);
		}
	}
}

/*!****************************************************************************
 * @brief
 */
void printUsigVar(char *string, const menuItem_type *menuItem, uint32_t var){
	static const int32_t pows[] = { 1, 10, 100, 1000, 10000, 100000, 1000000 };

	if(menuItem->prmHandle->power == 0){
		sprintf(string, "%"PRIu32"%s", var, menuItem->units);
	}else{
		uint32_t a = var / pows[menuItem->prmHandle->power];
		uint32_t b = var % pows[menuItem->prmHandle->power];
		sprintf(string, "%"PRIu32".%0*"PRIu32"%s", a, menuItem->prmHandle->power, b, menuItem->units);
	}
}

/*!****************************************************************************
 * @brief
 */
void printSigVar(char *string, const menuItem_type *menuItem, int32_t var){
	static const int32_t pows[] = { 1, 10, 100, 1000, 10000, 100000, 1000000 };

	if(menuItem->prmHandle->power == 0){
		sprintf(string, "%"PRIi32"%s", var, menuItem->units);
	}else{
		uint32_t a = var / pows[menuItem->prmHandle->power];
		uint32_t b = abs(var) % pows[menuItem->prmHandle->power];
		sprintf(string, "%"PRIi32".%0*"PRIu32"%s", a, menuItem->prmHandle->power, b, menuItem->units);
	}
}

/*!****************************************************************************
 * @brief
 */
void printFloatVar(char *string, const menuItem_type *menuItem){
	sprintf(string, "%.*f%s", menuItem->prmHandle->power, menuItem->prmHandle->prm->t_floatFrmt, menuItem->units);
}

/*!****************************************************************************
 * @param [out] string: string to be printed
 * @param [in] ip: IP Address
 * @param [in] editSelectionNumber: edit selection number
 * @param [out] selectionPosition: selected char position
 * @param [out] selectionLength: selected char length
 */
void printIpVar(char *string, const uint32_t ip, uint8_t editSectionNumber, uint8_t *selectionPosition, uint8_t *selectionLength){
	uint32_t nchars = 0;

	// Section 0
	if(editSectionNumber == 0){
		*selectionPosition = 0;
	}
	nchars += sprintf(string, "%"PRIu32":", (ip >> 24) & 0xFF);
	if(editSectionNumber == 0){
		*selectionLength = nchars - *selectionPosition - 1;
	}

	// Section 1
	if(editSectionNumber == 1){
		*selectionPosition = nchars;
	}
	nchars += sprintf(string + nchars, "%"PRIu32":", (ip >> 16) & 0xFF);
	if(editSectionNumber == 1){
		*selectionLength = nchars - *selectionPosition - 1;
	}

	// Section 2
	if(editSectionNumber == 2){
		*selectionPosition = nchars;
	}
	nchars += sprintf(string + nchars, "%"PRIu32":", (ip >> 8) & 0xFF);
	if(editSectionNumber == 2){
		*selectionLength = nchars - *selectionPosition - 1;
	}

	// Section 3
	if(editSectionNumber == 3){
		*selectionPosition = nchars;
	}
	nchars += sprintf(string + nchars, "%"PRIu32"", (ip >> 0) & 0xFF);
	if(editSectionNumber == 3){
		*selectionLength = nchars - *selectionPosition;
	}
}

/*!****************************************************************************
 * @param [out] string: string to be printed
 * @param [in] unixTime: unix timestamp
 * @param [in] editSelectionNumber: edit selection number 0 - month day, 1 - month, 2 - year, other - not select,
 * @param [out] selectionPosition: selected char position
 * @param [out] selectionLength: selected char length
 */
void printDateVar(char *string, const time_t unixTime, uint8_t editSectionNumber, uint8_t *selectionPosition, uint8_t *selectionLength){
	struct tm	tm;
	localtime_r(&unixTime, &tm);
	strftime(string, 16, "%d.%m.%Y", &tm);

	switch(editSectionNumber){
		case 0:
			*selectionPosition = 0;
			*selectionLength = 2;
			break;
		case 1:
			*selectionPosition = 3;
			*selectionLength = 2;
			break;
		case 2:
			*selectionPosition = 6;
			*selectionLength = 4;
			break;
		default:
			*selectionPosition = 0;
			*selectionLength = 0;
	}
}

/*!****************************************************************************
 * @param [out] string: string to be printed
 * @param [in] unixTime: unix timestamp
 * @param [in] editSelectionNumber: edit selection number 0 - hour, 1 - minute, 2 - second, other - not select,
 * @param [out] selectionPosition: selected char position
 * @param [out] selectionLength: selected char length
 */
void printTimeVar(char *string, const time_t unixTime, uint8_t editSectionNumber, uint8_t *selectionPosition, uint8_t *selectionLength){
	struct tm	tm;
	gmtime_r(&unixTime, &tm);
	strftime(string, 16, "%H:%M:%S", &tm);

	switch(editSectionNumber){
		case 0:
			*selectionPosition = 0;
			*selectionLength = 2;
			break;
		case 1:
			*selectionPosition = 3;
			*selectionLength = 2;
			break;
		case 2:
			*selectionPosition = 6;
			*selectionLength = 2;
			break;
		default:
			*selectionPosition = 0;
			*selectionLength = 0;
	}
}

/*!****************************************************************************
 * @brief output label and value string to display
 * @param [in] label: item label string
 * @param [in] value: item value string
 * @param [in] itemNumber: item number in display
 * @param [in] isSelected: item selected flag
 */
void outItemString(char *label, char *value, uint8_t itemNumber, menuItemSelect_type select){
	char	string[MENU_SCREEN_W / MENU_ITEM_CHAR_W + 1];

	static const disp_color_type colorLabel[] = {
	    [menuItemUnselect] = white,
	    [menuItemSelect] = red,
	    [menuItemUnselectUnchangeable] = white,
	    [menuItemSelectUnchangeable] = red,
	};

	static const disp_color_type colorValue[] = {
        [menuItemUnselect] = white,
        [menuItemSelect] = red,
        [menuItemUnselectUnchangeable] = blue,
        [menuItemSelectUnchangeable] = blue,
    };

	// Delimiter
	grf_line(0, MENU_PATH_H + itemNumber * MENU_ITEM_H - 1,
	            MENU_SCREEN_W - 1, MENU_PATH_H + itemNumber * MENU_ITEM_H - 1, halfLightGray);

	// Label and spaces
	strncpy(string, label, sizeof(string));
	memset(string + strlen(label), ' ', sizeof(string) - 1 - strlen(label));
	string[sizeof(string) -1 - strlen(value)] = 0;
    disp_setContentColor(colorLabel[select]);
    disp_putStr(0, MENU_PATH_H + MENU_ITEM_H * itemNumber, &MENU_ITEM_FONT, 0, string);

    // Value
    disp_setContentColor(colorValue[select]);
	disp_putStr(MENU_ITEM_CHAR_W * strlen(string), MENU_PATH_H + MENU_ITEM_H * itemNumber, &MENU_ITEM_FONT, 0, value);
}

/*!****************************************************************************
 * @brief output label and value string to display with selection area
 * @param [in] label: item label string
 * @param [in] value: item value string
 * @param [in] itemNumber: item number in display
 * @param [in] isSelected: item selected flag
 * @param [in] selectionPosition
 * @param [in] selectionLength
 */
void outItemStringWithSelection(char *label, char *value, uint8_t itemNumber, menuItemSelect_type select, uint8_t selectionPosition, uint8_t selectionLength){
	char	string[MENU_SCREEN_W / MENU_ITEM_CHAR_W + 1];
	uint8_t poschars = 0;

	if(select != menuItemSelect){
	    outItemString(label, value, itemNumber, select);
	    return;
	}

	grf_line(0, MENU_PATH_H + itemNumber * MENU_ITEM_H - 1,
				MENU_SCREEN_W - 1, MENU_PATH_H + itemNumber * MENU_ITEM_H - 1, halfLightGray);

	// Print label
	if(select == menuItemUnselect){
	    disp_setContentColor(white);
	}else{
	    disp_setContentColor(red);
	}
	disp_putStr(poschars, MENU_PATH_H + MENU_ITEM_H * itemNumber, &MENU_ITEM_FONT, 0, label);
	poschars += strlen(label);

	// Print spaces
	memset(string, ' ', MENU_SCREEN_W / MENU_ITEM_CHAR_W - strlen(label) - strlen(value));
	string[MENU_SCREEN_W / MENU_ITEM_CHAR_W - strlen(label) - strlen(value)] = 0;
	disp_putStr(poschars * MENU_ITEM_CHAR_W, MENU_PATH_H + MENU_ITEM_H * itemNumber, &MENU_ITEM_FONT, 0, string);
	poschars += strlen(string);

	// Print unselect
	memcpy(string, value, selectionPosition);
	string[selectionPosition] = 0;
	disp_setContentColor(white);
	disp_putStr(poschars * MENU_ITEM_CHAR_W, MENU_PATH_H + MENU_ITEM_H * itemNumber, &MENU_ITEM_FONT, 0, string);
	poschars += strlen(string);

	// Print select
	memcpy(string, value + selectionPosition, selectionLength);
	string[selectionLength] = 0;
	if(select == menuItemUnselect){
	    disp_setContentColor(white);
	}else{
	    disp_setContentColor(red);
	}
	disp_putStr(poschars * MENU_ITEM_CHAR_W, MENU_PATH_H + MENU_ITEM_H * itemNumber, &MENU_ITEM_FONT, 0, string);
	poschars += strlen(string);

	// Print unselect
	strcpy(string, value + selectionPosition + selectionLength);
	disp_setContentColor(white);
	disp_putStr(poschars * MENU_ITEM_CHAR_W, MENU_PATH_H + MENU_ITEM_H * itemNumber, &MENU_ITEM_FONT, 0, string);
}

/******************************** END OF FILE ********************************/

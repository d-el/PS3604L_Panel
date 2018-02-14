/*!****************************************************************************
 * @file		menuSystem.c
 * @author		Storozhenko Roman - D_EL
 * @version		V1.3
 * @copyright	GNU Lesser General Public License v3
 * @date		06.061.2018
 * @brief		menu system
 */

/*!****************************************************************************
 * Include
 */
#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stddef.h"
#include "string.h"
#include "stdlib.h"
#include "time.h"
#include "FreeRTOS.h"
#include "menuSystem.h"
#include "settingTSK.h"
#include "ui.h"
#include "rtc.h"
#include "display.h"
#include "graphics.h"
#include "key.h"
#include "enco.h"
#include "beep.h"

/*!****************************************************************************
 * MEMORY
 */
#define MENU_ITEM(_name, _label, _units, _prmHandle, _chmod, _pfPrm, _pfChanges, _pfSelect, _pfUnselect, _pfPeriod, _previous, _next, _parent, _child) \
{														\
	.label 					= _label,					\
	.units 					= _units,					\
	.prmHandle 				= _prmHandle,				\
	.flags.bit.chmod 		= _chmod,					\
	.flags.bit.pfParamert	= _pfPrm,					\
	.pfChanges 				= _pfChanges,				\
	.pfSelect				= _pfSelect,				\
	.pfUnselect				= _pfUnselect, 				\
	.pfPeriod				= _pfPeriod, 				\
	.previous				= &menuTree[mN_##_previous],\
	.next 					= &menuTree[mN_##_next],	\
	.parent					= &menuTree[mN_##_parent],	\
	.child					= &menuTree[mN_##_child],	\
},
const menuItem_type menuTree[] = {
	#include "menuTree.h"
};
#undef MENU_ITEM

char mstring[22];
char vstring[22];
const menuItem_type *selectedTopMenuPath[5];
const menuItem_type *selectedMenuPath[5];

/*!****************************************************************************
 * Define
 */
#define menuFont			font8x12
#define MENU_SCREEN_W		160
#define MENU_Y_DISTANCE		13
#define MENU_CHAR_W			8

/*!****************************************************************************
 * Local function declaration
 */
/*!****************************************************************************
 * @brief	Call function on Select
 */
void callSelect(const struct menuItem* item);
void callUnselect(const struct menuItem* item);
void callEnter(const struct menuItem* item);
void callExit(const struct menuItem* item);
void callChanges(const struct menuItem *item);
void callPeriodic(const struct menuItem *item);
void setLimit(const menuItem_type *menuItem, uint8_t editSection);
void printMenuPath(const menuItem_type **menuPath, const menuItem_type *selectedItem);
void printItem(const menuItem_type *menuItem, uint8_t itemNumber, uint8_t isSelected, uint8_t selectedSectionNumber);
void printUsigVar(char *string, const menuItem_type *menuItem, uint32_t var);
void printSigVar(char *string, const menuItem_type *menuItem, int32_t var);
void printFloatVar(char *string, const menuItem_type *menuItem);
void printIpVar(char *string, const uint32_t ip, uint8_t editSectionNumber, uint8_t *selectionPosition, uint8_t *selectionLength);
void printDateVar(char *string, const time_t unixTime, uint8_t editSectionNumber, uint8_t *selectionPosition, uint8_t *selectionLength);
void printTimeVar(char *string, const time_t unixTime, uint8_t editSectionNumber, uint8_t *selectionPosition, uint8_t *selectionLength);
void outItemString(char *label, char *value, uint8_t itemNumber, uint8_t isSelected);
void outItemStringWithSelection(char *label, char *value, uint8_t itemNumber, uint8_t isSelected, uint8_t selectionPosition, uint8_t selectionLength);

/*!****************************************************************************
 * @brief
 * @param 	startMenuItem	- starting menu item
 */
void menuEngine(menuItemNumber_type menuItemNumber){
	TickType_t 			xLastWakeTime;              //Вемя ОС

	const menuItem_type **topMenu = selectedTopMenuPath;
	const menuItem_type **sMenu = selectedMenuPath;
	const menuItem_type *sMenuPrev = NULL;

	uint8_t 			bigstepUp = 0;
	uint8_t 			bigstepDown = 0;
	uint8_t 			setDef = 0;
	uint8_t 			editSection = 0;
	enStatus_type 		enstatus = enNoCharge;

	*topMenu = &menuTree[menuItemNumber];
	*sMenu = *topMenu;

	xLastWakeTime = xTaskGetTickCount();
	disp_setColor(black, white);
	disp_fillScreen(black);
	ksSet(15, 5, kUp | kDown);
	enSetNtic(3);

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
			if(keyState(kView)){
				if((*sMenu != NULL)&&(*sMenu != (*sMenu)->previous)){
					const menuItem_type *item = (*sMenu)->previous;

					while(1){
						if(item->flags.bit.chmod == chmodMenuAlways){
							if(item->prmHandle != NULL){
								if(item->prmHandle->chmod == chmodAlways){
									callUnselect(*sMenu);
									*sMenu = item;			//Available select
									callSelect(*sMenu);
								}
								break;
							}
							else{
								callUnselect(*sMenu);
								*sMenu = item;				//Available select
								callSelect(*sMenu);
								break;
							}
						}
						if(item == item->previous){			//Ending
							break;
						}
						item = item->previous;
					}
				}
			}

			//Next
			if(keyState(kSet)){
				if((*sMenu != NULL)&&(*sMenu != (*sMenu)->next)){
					const menuItem_type *item = (*sMenu)->next;

					while(1){
						if(item->flags.bit.chmod == chmodMenuAlways){
							if(item->prmHandle != NULL){
								if(item->prmHandle->chmod == chmodAlways){
									callUnselect(*sMenu);
									*sMenu = item;			//Available select
									callSelect(*sMenu);
								}
								break;
							}
							else{
								callUnselect(*sMenu);
								*sMenu = item;				//Available select
								callSelect(*sMenu);
								break;
							}
						}
						if(item == item->next){				//Ending
							break;
						}
						item = item->next;
					}
				}
			}

			//Parent
			if(keyState(kMode)){
				if(sMenu > selectedMenuPath){
					*topMenu = NULL;
					*sMenu = NULL;
					topMenu--;
					sMenu--;
					callExit(*sMenu);
					disp_fillScreen(black);
				}
				else{
					//Exit out menu
					return;
				}
			}

			//Child
			if(keyState(kOnOff)){
					if((*sMenu != NULL)&&(*sMenu != (*sMenu)->child)){
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
									*sMenu = item; 			//Available select
									callSelect(*sMenu);
								}
								break;
							}
							else{
								*sMenu = item; 				//Available select
								callSelect(*sMenu);
								break;
							}
						}
						if(item == item->next){				//Ending
							break;
						}
						item = item->next;
					}
					disp_fillScreen(black);
				}
			}
		}

		//Detection of menu item switching
		if((*sMenu != NULL)&&(sMenuPrev != *sMenu)){
			editSection = 0;
			if((*sMenu)->prmHandle->limType == prmLimVariable){
				setLimit((*sMenu), editSection);
			}
			sMenuPrev = *sMenu;
		}

		/************************
		 * Editor
		 */
		const menuItem_type *eitem = *sMenu;
		if(eitem->prmHandle != NULL){
			if((eitem->flags.bit.chmod == chmodMenuAlways)&&(eitem->prmHandle->chmod == chmodAlways)){
				if(bigstepUp != 0){
					if(eitem->prmHandle->limType == prmLimVariable){
						if(editSection < 3){
							editSection++;
							setLimit(eitem, editSection);
						}
					}
					else{
						enstatus = enBigStepUp(eitem->prmHandle);
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
						enstatus = enBigStepDown(eitem->prmHandle);
					}
					bigstepDown = 0;
				}else if(setDef != 0){
					enWriteVal(eitem->prmHandle, &eitem->prmHandle->def);
					enstatus = enCharge;
					setDef = 0;
				}else{
					enstatus = enUpDate(eitem->prmHandle);
				}
				if((enstatus != enNoCharge) && (eitem->pfChanges != NULL)){
					callChanges(eitem);
				}
				if((enstatus == enLimDown) || (enstatus == enLimUp)){
					BeepTime(ui.beep.encoLim.time, ui.beep.encoLim.freq);
				}else if((enstatus == enTransitionDown) || (enstatus == enTransitionUp)){
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
		 * Print all menu item in this tree
		 */
		uint8_t itemNum = 0;
		const menuItem_type *item = *topMenu;
		while(1){
			printItem(item, itemNum, item == *sMenu, editSection);
			if(item == item->next){
				break;
			}
			item = item->next;
			itemNum++;
		}

		/*************************************/
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(MENU_PERIOD));
	}
}

/*!****************************************************************************
 * @brief	Print message
 */
void printMessageWindow(char *string){
	disp_fillScreen(black);
	disp_putStr(0, MENU_Y_DISTANCE, &menuFont, 0, string);
	while(keyProc() == 0){
		vTaskDelay(pdMS_TO_TICKS(MENU_PERIOD));
	}
	BeepTime(ui.beep.key.time, ui.beep.key.freq);
}

/*!****************************************************************************
 * @brief	Call function on Select
 */
void callSelect(const struct menuItem* item){
	if((item->pfSelect != NULL)&&(item->prmHandle != NULL)){
		itemState_type res = item->pfSelect(item);
	}
}

/*!****************************************************************************
 * @brief	Call function on Unselect
 */
void callUnselect(const struct menuItem* item){
	if((item->pfUnselect != NULL)&&(item->prmHandle != NULL)){
		itemState_type res = item->pfUnselect(item);
	}
}

/*!****************************************************************************
 * @brief	Call function on Enter
 */
void callEnter(const struct menuItem* item){
	if((item->pfSelect != NULL)&&(item->prmHandle == NULL)){
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
	if((item->pfUnselect != NULL)&&(item->prmHandle == NULL)){
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
	}
}

/*!****************************************************************************
 * @brief	Call function Periodic
 */
void callPeriodic(const struct menuItem *item){
	if(item->pfPeriod != NULL){
		itemState_type res = item->pfPeriod(item);
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
	}
}

/*!****************************************************************************
 * @param menuPath: pointer to menu path
 */
void printMenuPath(const menuItem_type **menuPath, const menuItem_type *selectedItem){
	char string[MENU_SCREEN_W / MENU_CHAR_W + 1];

	strcpy(string, "/");

	while(*menuPath != selectedItem){
		strcpy(string + strlen(string), (*menuPath)->label);
		menuPath++;
		if(*menuPath != selectedItem){
			strcpy(string + strlen(string), "/");
		}
	}
	memset(string + strlen(string), ' ', MENU_SCREEN_W / MENU_CHAR_W - strlen(string));
	string[MENU_SCREEN_W / MENU_CHAR_W] = 0;

	disp_setColor(black, green);
	disp_putStr(0, 0, &menuFont, 0, string);	//Вывод на дисплей
}

/*!****************************************************************************
 * @param [in] item: menu item handler
 * @param [in] itemNumber: item number in screen
 * @param [in] isSelected: 0 - item is unselected, 1 - item is selected
 * @param [in] selectedSectionNumber: selected section number in composite type (e.g. IP, Time)
 */
void printItem(const menuItem_type *menuItem, uint8_t itemNumber, uint8_t isSelected, uint8_t selectedSectionNumber){
	struct tm	tm;
	uint8_t selectionPosition, selectionLength;

	if(menuItem != menuItem->child){
		sprintf(mstring, "%s/", menuItem->label);
	}else{
		sprintf(mstring, "%s", menuItem->label);
	}

	if(menuItem->prmHandle == NULL){
		outItemString(mstring, "", itemNumber, isSelected);
	}
	else{
		switch(menuItem->prmHandle->type){
			case u8Frmt:
				printUsigVar(vstring, menuItem, menuItem->prmHandle->prm->t_u8Frmt);
				outItemString(mstring, vstring, itemNumber, isSelected);
				break;
			case s8Frmt:
				printSigVar(vstring, menuItem, menuItem->prmHandle->prm->t_s8Frmt);
				outItemString(mstring, vstring, itemNumber, isSelected);
				break;
			case u16Frmt:
				printUsigVar(vstring, menuItem, menuItem->prmHandle->prm->t_u16Frmt);
				outItemString(mstring, vstring, itemNumber, isSelected);
				break;
			case s16Frmt:
				printSigVar(vstring, menuItem, menuItem->prmHandle->prm->t_s16Frmt);
				outItemString(mstring, vstring, itemNumber, isSelected);
				break;
			case u32Frmt:
				printUsigVar(vstring, menuItem, menuItem->prmHandle->prm->t_u32Frmt);
				outItemString(mstring, vstring, itemNumber, isSelected);
				break;
			case s32Frmt:
				printSigVar(vstring, menuItem, menuItem->prmHandle->prm->t_s32Frmt);
				outItemString(mstring, vstring, itemNumber, isSelected);
				break;
			case floatFrmt:
				printFloatVar(vstring, menuItem);
				outItemString(mstring, vstring, itemNumber, isSelected);
				break;
			case unixDateFrmt:
				printDateVar(vstring, menuItem->prmHandle->prm->t_unixDateFrmt, selectedSectionNumber, &selectionPosition, &selectionLength);
				//outItemStringWithSelection(mstring, vstring, itemNumber, isSelected, selectionPosition, selectionLength);
				outItemString(mstring, vstring, itemNumber, isSelected);
				break;
			case unixTimeFrmt:
				printTimeVar(vstring, menuItem->prmHandle->prm->t_unixTimeFrmt, selectedSectionNumber, &selectionPosition, &selectionLength);
				outItemStringWithSelection(mstring, vstring, itemNumber, isSelected, selectionPosition, selectionLength);
				break;
			case ipAdrFrmt:
				printIpVar(vstring, menuItem->prmHandle->prm->t_ipAdrFrmt, selectedSectionNumber, &selectionPosition, &selectionLength);
				outItemStringWithSelection(mstring, vstring, itemNumber, isSelected, selectionPosition, selectionLength);
				break;
			default:
				outItemString(mstring, "Error", itemNumber, isSelected);
		}
	}
}

/*!****************************************************************************
 * @brief
 */
void printUsigVar(char *string, const menuItem_type *menuItem, uint32_t var){
	switch(menuItem->prmHandle->power){
		case 0:
			sprintf(string, "%u%s", var, menuItem->units);
			break;
		case 1:
			sprintf(string, "%u.%01u%s", var / 10, var % 10, menuItem->units);
			break;
		case 2:
			sprintf(string, "%u.%02u%s", var / 100, var % 100, menuItem->units);
			break;
		case 3:
			sprintf(string, "%u.%03u%s", var / 1000, var % 1000, menuItem->units);
			break;
		case 4:
			sprintf(string, "%u.%04u%s", var / 10000, var % 10000, menuItem->units);
			break;
		case 5:
			sprintf(string, "%u.%05u%s", var / 100000, var % 100000, menuItem->units);
			break;
		case 6:
			sprintf(string, "%u.%06u%s", var / 1000000, var % 1000000, menuItem->units);
			break;
		default:
			sprintf(string, "");
	}
}

/*!****************************************************************************
 * @brief
 */
void printSigVar(char *string, const menuItem_type *menuItem, int32_t var){
	switch(menuItem->prmHandle->power){
		case 0:
			sprintf(string, "%i%s", var, menuItem->units);
			break;
		case 1:
			sprintf(string, "%i.%01i%s", var / 10, abs(var) % 10, menuItem->units);
			break;
		case 2:
			sprintf(string, "%i.%02i%s", var / 100, abs(var) % 100, menuItem->units);
			break;
		case 3:
			sprintf(string, "%i.%03i%s", var / 1000, abs(var) % 1000, menuItem->units);
			break;
		case 4:
			sprintf(string, "%i.%04i%s", var / 10000, abs(var) % 10000, menuItem->units);
			break;
		case 5:
			sprintf(string, "%i.%05i%s", var / 100000, abs(var) % 100000, menuItem->units);
			break;
		case 6:
			sprintf(string, "%i.%06i%s", var / 1000000, abs(var) % 1000000, menuItem->units);
			break;
		default:
			sprintf(string, "");
	}
}

/*!****************************************************************************
 * @brief
 */
void printFloatVar(char *string, const menuItem_type *menuItem){
	switch(menuItem->prmHandle->power){
		case 0:
			sprintf(string, "%.0f%s", menuItem->prmHandle->prm->t_floatFrmt, menuItem->units);
			break;
		case 1:
			sprintf(string, "%.1f%s", menuItem->prmHandle->prm->t_floatFrmt, menuItem->units);
			break;
		case 2:
			sprintf(string, "%.2f%s", menuItem->prmHandle->prm->t_floatFrmt, menuItem->units);
			break;
		case 3:
			sprintf(string, "%.3f%s", menuItem->prmHandle->prm->t_floatFrmt, menuItem->units);
			break;
		case 4:
			sprintf(string, "%.4f%s", menuItem->prmHandle->prm->t_floatFrmt, menuItem->units);
			break;
		case 5:
			sprintf(string, "%.5f%s", menuItem->prmHandle->prm->t_floatFrmt, menuItem->units);
			break;
		case 6:
			sprintf(string, "%.6f%s", menuItem->prmHandle->prm->t_floatFrmt, menuItem->units);
			break;
		default:
			sprintf(string, "Error");
	}
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
	nchars += sprintf(string, "%u:", (ip >> 24) & 0xFF);
	if(editSectionNumber == 0){
		*selectionLength = nchars - *selectionPosition - 1;
	}

	// Section 1
	if(editSectionNumber == 1){
		*selectionPosition = nchars;
	}
	nchars += sprintf(string + nchars, "%u:", (ip >> 16) & 0xFF);
	if(editSectionNumber == 1){
		*selectionLength = nchars - *selectionPosition - 1;
	}

	// Section 2
	if(editSectionNumber == 2){
		*selectionPosition = nchars;
	}
	nchars += sprintf(string + nchars, "%u:", (ip >> 8) & 0xFF);
	if(editSectionNumber == 2){
		*selectionLength = nchars - *selectionPosition - 1;
	}

	// Section 3
	if(editSectionNumber == 3){
		*selectionPosition = nchars;
	}
	nchars += sprintf(string + nchars, "%u", (ip >> 0) & 0xFF);
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
	uint32_t	nchars = 0;
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
	uint32_t	nchars = 0;
	struct tm	tm;
	localtime_r(&unixTime, &tm);
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
void outItemString(char *label, char *value, uint8_t itemNumber, uint8_t isSelected){
	char 	string[MENU_SCREEN_W / MENU_CHAR_W + 1];
	uint8_t	labelLen = strlen(label);
	uint8_t	valLen = strlen(value);

	memcpy(string, label, labelLen);
	memset(string + labelLen, ' ', sizeof(string) - labelLen);
	memcpy(string + MENU_SCREEN_W / MENU_CHAR_W - valLen, value, valLen + 1);

	grf_line(0, itemNumber * MENU_Y_DISTANCE + MENU_Y_DISTANCE - 1,
			MENU_SCREEN_W - 1, itemNumber * MENU_Y_DISTANCE +  MENU_Y_DISTANCE - 1, halfLightGray);

	if(isSelected != 0){
		disp_setContentColor(red);
	}else{
		disp_setContentColor(white);
	}
	disp_putStr(0, MENU_Y_DISTANCE + MENU_Y_DISTANCE * itemNumber, &menuFont, 0, string);
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
void outItemStringWithSelection(char *label, char *value, uint8_t itemNumber, uint8_t isSelected, uint8_t selectionPosition, uint8_t selectionLength){
	char 	string[MENU_SCREEN_W / MENU_CHAR_W + 1];
	uint8_t poschars = 0;

	grf_line(0, itemNumber * MENU_Y_DISTANCE + MENU_Y_DISTANCE - 1,
				MENU_SCREEN_W - 1, itemNumber * MENU_Y_DISTANCE +  MENU_Y_DISTANCE - 1, halfLightGray);

	// Print label
	if(isSelected != 0){
		disp_setContentColor(red);
	}else{
		disp_setContentColor(white);
	}
	disp_putStr(poschars, MENU_Y_DISTANCE + MENU_Y_DISTANCE * itemNumber, &menuFont, 0, label);
	poschars += strlen(label);

	// Print spaces
	memset(string, ' ', MENU_SCREEN_W / MENU_CHAR_W - strlen(label) - strlen(value));
	string[MENU_SCREEN_W / MENU_CHAR_W - strlen(label) - strlen(value)] = 0;
	disp_putStr(poschars * MENU_CHAR_W, MENU_Y_DISTANCE + MENU_Y_DISTANCE * itemNumber, &menuFont, 0, string);
	poschars += strlen(string);

	// Print unselect
	memcpy(string, value, selectionPosition);
	string[selectionPosition] = 0;
	disp_setContentColor(white);
	disp_putStr(poschars * MENU_CHAR_W, MENU_Y_DISTANCE + MENU_Y_DISTANCE * itemNumber, &menuFont, 0, string);
	poschars += strlen(string);

	// Print select
	memcpy(string, value + selectionPosition, selectionLength);
	string[selectionLength] = 0;
	if(isSelected != 0){
		disp_setContentColor(red);
	}else{
		disp_setContentColor(white);
	}
	disp_putStr(poschars * MENU_CHAR_W, MENU_Y_DISTANCE + MENU_Y_DISTANCE * itemNumber, &menuFont, 0, string);
	poschars += strlen(string);

	// Print unselect
	strcpy(string, value + selectionPosition + selectionLength);
	disp_setContentColor(white);
	disp_putStr(poschars * MENU_CHAR_W, MENU_Y_DISTANCE + MENU_Y_DISTANCE * itemNumber, &menuFont, 0, string);
}

/*************** LGPL ************** END OF FILE *********** D_EL ************/

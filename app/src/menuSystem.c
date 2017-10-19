/*!****************************************************************************
 * @file		menuSystem.c
 * @author		Storozhenko Roman - D_EL
 * @version		V1.1
 * @copyright	GNU Lesser General Public License v3
 * @date		10.02.2016
 * @brief		menu system
 */

/*!****************************************************************************
 * Include
 */
#include "menuSystem.h"
#include "settingTSK.h"

/*!****************************************************************************
 * MEMORY
 */
#define MENU_ITEM(_name, _label, _units, _prmHandle, _chmod, _pfPrm, _pfChanges, _pfSelect, _pfUnselect, _pfPeriod, _previous, _next, _parent , _child) \
const menuItem_type item_##_name = {				\
	.label 					= _label,					\
	.units 					= _units,					\
	.prmHandle 				= _prmHandle,				\
	.flags.bit.chmod 		= _chmod,					\
	.flags.bit.pfParamert	= _pfPrm,					\
	.pfChanges 				= _pfChanges,				\
	.pfSelect				= _pfSelect,				\
	.pfUnselect				= _pfUnselect, 				\
	.pfPeriod				= _pfPeriod, 				\
	.previous				= &item_##_previous,		\
	.next 					= &item_##_next,			\
	.parent					= &item_##_parent,			\
	.child					= &item_##_child,			\
};
#include "menuTree.h"
#undef MENU_ITEM

char mstring[22];
char vstring[22];
const menuItem_type *pathMenu[5];
const menuItem_type *selectPathMenu[5];
uint32_t ipMul;

/*!****************************************************************************
 * Define
 */
#define MENU_SCREEN_W		160
#define MENU_Y_DISTANCE		13
#define MENU_CHAR_W			8

/*!****************************************************************************
 * Local function declaration
 */
void printMenuPath(const menuItem_type **menuPath);
void printItem(const menuItem_type *menuItem, uint8_t itemNumber, uint8_t isSelected, uint8_t selectedSectionNumber);

void printUsigVar(char *string, const menuItem_type *menuItem, uint32_t var);
void printSigVar(char *string, const menuItem_type *menuItem, int32_t var);
void printFloatVar(char *string, const menuItem_type *menuItem);
void printIpVar(char *string, const uint32_t ip, uint8_t editSectionNumber, uint8_t *selectionPosition, uint8_t *selectionLength);

void outItemString(char *label, char *value, uint8_t itemNumber, uint8_t isSelected);
void outItemStringWithSelection(char *label, char *value, uint8_t itemNumber, uint8_t isSelected, uint8_t selectionPosition, uint8_t selectionLength);

/*!****************************************************************************
 * @brief
 * @param 	startMenuItem	- starting menu item
 */
void menuEngine(const menuItem_type *startMenuItem){
	TickType_t 			xLastWakeTime;              //Вемя ОС

	const menuItem_type **topMenu = pathMenu;
	const menuItem_type **sMenu = selectPathMenu;
	const menuItem_type *vMenu;

	uint8_t 			bigstepUp = 0;
	uint8_t 			bigstepDown = 0;
	uint8_t 			setDef = 0;
	uint8_t 			editSection = 0;
	enStatus_type 		enstatus;

	pathMenu[0] = startMenuItem;
	*sMenu = *topMenu;

	xLastWakeTime = xTaskGetTickCount();      //Инициализируем xLastWakeTime текущим временем
	lcd_setColor(black, white);
	lcd_fillScreen(black);
	ksSet(15, 5, kUp | kDown);
	enSetNtic(5);

	while(1){
		/**************************************
		 * Обработка кнопок
		 */
		if(keyProc() != 0){
			if(keyState(kUp)){
				bigstepUp = 1;
			}else if(keyState(kDown)){
				bigstepDown = 1;
			}else if(keyState(kZero)){
				setDef = 1;
			}

			BeepTime(ui.beep.key.time, ui.beep.key.freq);

			//Previous
			if(keyState(kView)){
				if((*sMenu) != NULL){
					if((*sMenu == (*sMenu)->child) && ((*sMenu)->pfUnselect != NULL)){
						(*sMenu)->pfUnselect(*sMenu);				//Call pfUnselect
					}
				}
				vMenu = (*sMenu)->previous;
				while(1){
					if(vMenu->flags.bit.chmod == chmodMenuAlways){
						if(vMenu->prmHandle != NULL){
							if(vMenu->prmHandle->chmod == chmodAlways){
								*sMenu = vMenu; 					//Available select
								if((*sMenu == (*sMenu)->child) && ((*sMenu)->pfSelect != NULL)){
									(*sMenu)->pfSelect(*sMenu);		//Call pfSelect
								}
								break;
							}
						}else{
							*sMenu = vMenu; 						//Available select
							if((*sMenu == (*sMenu)->child) && ((*sMenu)->pfSelect != NULL)){
								(*sMenu)->pfSelect(*sMenu);			//Call pfSelect
							}
							break;
						}
						editSection = 0;
					}
					if(vMenu == vMenu->previous){					//Ending
						break;
					}
					vMenu = vMenu->previous;
				}
			}

			//Next
			if(keyState(kNext)){
				if((*sMenu) != NULL){
					if((*sMenu == (*sMenu)->child) && ((*sMenu)->pfUnselect != NULL)){
						(*sMenu)->pfUnselect(*sMenu);				//Call pfUnselect
					}
				}
				vMenu = (*sMenu)->next;
				while(1){
					if(vMenu->flags.bit.chmod == chmodMenuAlways){
						if(vMenu->prmHandle != NULL){
							if(vMenu->prmHandle->chmod == chmodAlways){
								*sMenu = vMenu; 					//Available select
								if((*sMenu == (*sMenu)->child) && ((*sMenu)->pfSelect != NULL)){
									(*sMenu)->pfSelect(*sMenu);		//Call pfSelect
								}
								break;
							}
						}else{
							*sMenu = vMenu; 						//Available select
							if((*sMenu == (*sMenu)->child) && ((*sMenu)->pfSelect != NULL)){
								(*sMenu)->pfSelect(*sMenu);			//Call pfSelect
							}
							break;
						}
						editSection = 0;
					}
					if(vMenu == vMenu->next){						//Ending
						break;
					}
					vMenu = vMenu->next;
				}
			}

			//Parent
			if(keyState(kMode)){
				if((*sMenu) != NULL){
					if((*sMenu == (*sMenu)->child) && ((*sMenu)->pfSelect != NULL)){
						(*sMenu)->pfSelect(*sMenu);		//Call pfSelect
					}
				}
				if(sMenu > selectPathMenu){
					*topMenu = NULL;
					*sMenu = NULL;
					topMenu--;
					sMenu--;
					if((*sMenu)->pfUnselect != NULL){
						(*sMenu)->pfUnselect(*sMenu);			//Call pfUnselect
					}
					lcd_fillScreen(black);
				}else{	//Выходим из меню
					return;
				}
			}

			//Child
			if((keyState(kOnOff)) && (*sMenu != (*sMenu)->child)){
				if((*sMenu) != NULL){
					if((*sMenu)->pfSelect != NULL){
						(*sMenu)->pfSelect(*sMenu);			//Call pfEnter
					}
				}
				lcd_fillScreen(black);
				topMenu++;
				*topMenu = (*sMenu)->child;
				sMenu++;

				*sMenu = NULL;
				vMenu = *topMenu;
				while(1){
					if(vMenu->flags.bit.chmod == chmodMenuAlways){
						if(vMenu->prmHandle != NULL){
							if(vMenu->prmHandle->chmod == chmodAlways){
								*sMenu = vMenu; 					//Available select
								if((*sMenu == (*sMenu)->child) && ((*sMenu)->pfSelect != NULL)){
									(*sMenu)->pfSelect(*sMenu);		//Call pfSelect
								}
								break;
							}
						}else{
							*sMenu = vMenu; 						//Available select
							if((*sMenu == (*sMenu)->child) && ((*sMenu)->pfSelect != NULL)){
								(*sMenu)->pfSelect(*sMenu);			//Call pfSelect
							}
							break;
						}
					}
					if(vMenu == vMenu->next){						//Ending
						break;
					}
					vMenu = vMenu->next;
				}
			}
		}

		/************************
		 * Редактор
		 */
		vMenu = *sMenu;
		if(vMenu->prmHandle != NULL){
			if((vMenu->flags.bit.chmod == chmodMenuAlways) && (vMenu->prmHandle->chmod == chmodAlways)){
				if(bigstepUp != 0){
					if(vMenu->prmHandle->type != ipAdrFrmt){
						enstatus = enBigStepUp(vMenu->prmHandle);
					}else{
						if(editSection < 3){	//Изменяем редактируемый разряд
							editSection++;
							vMenu->prmHandle->step->t_ipAdrFrmt = 1UL << (3 - editSection) * 8;
							vMenu->prmHandle->min->t_ipAdrFrmt = vMenu->prmHandle->prm->t_ipAdrFrmt & ~(255UL << (3 - editSection) * 8);
							vMenu->prmHandle->max->t_ipAdrFrmt = vMenu->prmHandle->prm->t_ipAdrFrmt | 255UL << (3 - editSection) * 8;
						}
					}
					bigstepUp = 0;
				}
				else if(bigstepDown != 0){
					if(vMenu->prmHandle->type != ipAdrFrmt){
						enstatus = enBigStepDown(vMenu->prmHandle);
					}else{
						if(editSection > 0){	//Изменяем редактируемый разряд
							editSection--;
							vMenu->prmHandle->step->t_ipAdrFrmt = 1UL << (3 - editSection) * 8;
							vMenu->prmHandle->min->t_ipAdrFrmt = vMenu->prmHandle->prm->t_ipAdrFrmt & ~(255UL << (3 - editSection) * 8);
							vMenu->prmHandle->max->t_ipAdrFrmt = vMenu->prmHandle->prm->t_ipAdrFrmt | 255UL << (3 - editSection) * 8;
						}
					}
					bigstepDown = 0;
				}
				else if(setDef != 0){
					enWriteVal(vMenu->prmHandle, &vMenu->prmHandle->def);
					enstatus = enCharge;
					setDef = 0;
				}
				else{
					enstatus = enUpDate(vMenu->prmHandle);
				}

				if((enstatus != enNoCharge) && (vMenu->pfChanges != NULL)){
					vMenu->pfChanges(vMenu);
				}
				if((enstatus == enLimDown) || (enstatus == enLimUp)){
					BeepTime(ui.beep.encoLim.time, ui.beep.encoLim.freq);
				}else if((enstatus == enTransitionDown) || (enstatus == enTransitionUp)){
					BeepTime(ui.beep.encoTransition.time, ui.beep.encoTransition.freq);
				}
			}
		}

		/******************************
		 * Периодический вызов
		 */
		vMenu = *topMenu;
		while(1){
			if(vMenu == vMenu->next){
				break;
			}
			if(vMenu->pfPeriod != NULL){
				vMenu->pfPeriod(vMenu);
			}
			vMenu = vMenu->next;
		}

		/******************************
		 * Print menu path
		 */
		printMenuPath(selectPathMenu);

		/******************************
		 * Print all menu item in this tree
		 */
		uint8_t itemNum = 0;
		vMenu = *topMenu;
		while(1){
			printItem(vMenu, itemNum, vMenu == *sMenu, editSection);
			if(vMenu == vMenu->next){
				break;
			}
			vMenu = vMenu->next;
			itemNum++;
		}

		/*************************************/
		//vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(MENU_PERIOD));
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
	}
}

/*!****************************************************************************
 * @param menuPath: pointer to menu path
 */
void printMenuPath(const menuItem_type **menuPath){
	char string[MENU_SCREEN_W / MENU_CHAR_W + 1];

	string[0] = '/';
	string[1] = 0;
	while(*menuPath != NULL){
		if(*(menuPath + 1) == NULL){
			break;
		}
		sprintf(string + strlen(string), "%s/", (*menuPath)->label);
		menuPath++;
	}
	memset(string + strlen(string), ' ', MENU_SCREEN_W / MENU_CHAR_W - strlen(string));
	string[MENU_SCREEN_W / MENU_CHAR_W] = 0;

	lcd_setColor(black, green);
	lcd_putStr(0, 0, &font8x12, 0, string);	//Вывод на дисплей
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
				gmtime_r(&menuItem->prmHandle->prm->t_unixDateFrmt, &tm);
				strftime(vstring, sizeof(vstring), "%Y.%m.%d", &tm);
				outItemString(mstring, vstring, itemNumber, isSelected);
				break;
			case unixTimeFrmt:
				gmtime_r(&menuItem->prmHandle->prm->t_unixTimeFrmt, &tm);
				strftime(vstring, sizeof(vstring), "%H:%M:%S", &tm);
				outItemString(mstring, vstring, itemNumber, isSelected);
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
			sprintf(string, "%i.%01u%s", var / 10, abs(var) % 10, menuItem->units);
			break;
		case 2:
			sprintf(string, "%i.%02u%s", var / 100, abs(var) % 100, menuItem->units);
			break;
		case 3:
			sprintf(string, "%i.%03u%s", var / 1000, abs(var) % 1000, menuItem->units);
			break;
		case 4:
			sprintf(string, "%i.%04u%s", var / 10000, abs(var) % 10000, menuItem->units);
			break;
		case 5:
			sprintf(string, "%i.%05u%s", var / 100000, abs(var) % 100000, menuItem->units);
			break;
		case 6:
			sprintf(string, "%i.%06u%s", var / 1000000, abs(var) % 1000000, menuItem->units);
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
		lcd_setContentColor(red);
	}else{
		lcd_setContentColor(white);
	}
	lcd_putStr(0, MENU_Y_DISTANCE + MENU_Y_DISTANCE * itemNumber, &font8x12, 0, string);
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
		lcd_setContentColor(red);
	}else{
		lcd_setContentColor(white);
	}
	lcd_putStr(poschars, MENU_Y_DISTANCE + MENU_Y_DISTANCE * itemNumber, &font8x12, 0, label);
	poschars += strlen(label);

	// Print spaces
	memset(string, ' ', MENU_SCREEN_W / MENU_CHAR_W - strlen(label) - strlen(value));
	string[MENU_SCREEN_W / MENU_CHAR_W - strlen(label) - strlen(value)] = 0;
	lcd_putStr(poschars * MENU_CHAR_W, MENU_Y_DISTANCE + MENU_Y_DISTANCE * itemNumber, &font8x12, 0, string);
	poschars += strlen(string);

	// Print unselect
	memcpy(string, value, selectionPosition);
	string[selectionPosition] = 0;
	lcd_setContentColor(white);
	lcd_putStr(poschars * MENU_CHAR_W, MENU_Y_DISTANCE + MENU_Y_DISTANCE * itemNumber, &font8x12, 0, string);
	poschars += strlen(string);

	// Print select
	memcpy(string, value + selectionPosition, selectionLength);
	string[selectionLength] = 0;
	if(isSelected != 0){
		lcd_setContentColor(red);
	}else{
		lcd_setContentColor(white);
	}
	lcd_putStr(poschars * MENU_CHAR_W, MENU_Y_DISTANCE + MENU_Y_DISTANCE * itemNumber, &font8x12, 0, string);
	poschars += strlen(string);

	// Print unselect
	strcpy(string, value + selectionPosition + selectionLength);
	lcd_setContentColor(white);
	lcd_putStr(poschars * MENU_CHAR_W, MENU_Y_DISTANCE + MENU_Y_DISTANCE * itemNumber, &font8x12, 0, string);
}

/*************** LGPL ************** END OF FILE *********** D_EL ************/

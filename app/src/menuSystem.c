/*!****************************************************************************
 * @file    menuSystem.c
 * @author  Storozhenko Roman - D_EL
 * @version V1.0
 * @date    10.02.2016
 * @brief   parametres system
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
char pstring[22];
const menuItem_type *pathMenu[5];
const menuItem_type *selectPathMenu[5];

/*!****************************************************************************
 * Define
 */
#define MENU_SCREEN_W		160
#define MENU_POS_LABEL		0
#define MENU_POS_VAR		64
#define MENU_Y_DISTANCE		14

/*!****************************************************************************
 * @brief
 * @param 	startMenuItem	- starting menu item
 */
void menuEngine(const menuItem_type *startMenuItem){
	TickType_t 			xLastWakeTime;              //Вемя ОС

	const menuItem_type **topMenu = pathMenu;
	const menuItem_type **sMenu = selectPathMenu;
	const menuItem_type *vMenu;

	struct tm 			tm;
	uint8_t 			bigstepUp = 0;
	uint8_t 			bigstepDown = 0;
	uint8_t 			setDef = 0;
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
					if((*sMenu)->pfUnselect != NULL){
						(*sMenu)->pfUnselect(*sMenu);		//Call pfExit
					}

				}
				if(sMenu > selectPathMenu){
					lcd_fillScreen(black);
					*topMenu = NULL;
					*sMenu = NULL;
					topMenu--;
					sMenu--;
					if((*sMenu == (*sMenu)->child) && ((*sMenu)->pfSelect != NULL)){
						(*sMenu)->pfSelect(*sMenu);			//Call pfSelect
					}
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

					/*if(vMenu != vMenu->child){
					 *sMenu = vMenu; 			//Available select as is parent
					 if((*sMenu)->pfSelect != NULL){
					 (*sMenu)->pfSelect(*sMenu);		//Call pfSelect
					 }
					 break;
					 }*/
					/*if(vMenu->prmHandle != NULL){	//Is parametr handle
					 if((vMenu->flags.bit.chmod == chmodMenuAlways)&&(vMenu->prmHandle->chmod == chmodAlways)){
					 *sMenu = vMenu; 		//Available select
					 if((*sMenu)->pfSelect != NULL){
					 (*sMenu)->pfSelect(*sMenu);
					 }
					 break;
					 }
					 }
					 if(vMenu == vMenu->next){
					 break;
					 }
					 vMenu = vMenu->next;*/
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
					enstatus = enBigStepUp(vMenu->prmHandle);
					bigstepUp = 0;
				}else if(bigstepDown != 0){
					enstatus = enBigStepDown(vMenu->prmHandle);
					bigstepDown = 0;
				}else if(setDef != 0){
					enWriteVal(vMenu->prmHandle, &vMenu->prmHandle->def);
					enstatus = enCharge;
					setDef = 0;
				}else{
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
		 * Вывод путь меню
		 */
		const menuItem_type **ppathMenu = selectPathMenu;
		mstring[0] = '/';
		mstring[1] = 0;
		while(*ppathMenu != NULL){
			if(*(ppathMenu + 1) == NULL){
				break;
			}
			sprintf(mstring + strlen(mstring), "%s/", (*ppathMenu)->label);
			ppathMenu++;
		}
		lcd_setColor(black, green);
		memset(mstring + strlen(mstring), ' ', (MENU_SCREEN_W) / 8 - strlen(mstring));
		lcd_putStr(0, 0, &font8x12, 0, mstring);	//Вывод на дисплей

		/******************************
		 * Выводим все строки текущего уровня меню
		 */
		uint8_t itemNum = 0;
		vMenu = *topMenu;
		while(1){
			if(vMenu == *sMenu){
				lcd_setColor(black, red);
			}else{
				lcd_setColor(black, white);
			}

			if(vMenu != vMenu->child){
				sprintf(mstring, "%s/", vMenu->label);
			}else{
				sprintf(mstring, "%s", vMenu->label);
			}

			lcd_putStr(0, MENU_Y_DISTANCE + itemNum * MENU_Y_DISTANCE, &font8x12, 0, mstring);	//Вывод на дисплей

			if(vMenu->prmHandle != NULL){
				switch(vMenu->prmHandle->type){
					case u8Frmt:
						printUsigVar(mstring, vMenu, vMenu->prmHandle->prm->_u8Frmt);
						break;
					case s8Frmt:
						printSigVar(mstring, vMenu, vMenu->prmHandle->prm->_s8Frmt);
						break;
					case u16Frmt:
						printUsigVar(mstring, vMenu, vMenu->prmHandle->prm->_u16Frmt);
						break;
					case s16Frmt:
						printSigVar(mstring, vMenu, vMenu->prmHandle->prm->_s16Frmt);
						break;
					case u32Frmt:
						printUsigVar(mstring, vMenu, vMenu->prmHandle->prm->_u32Frmt);
						break;
					case s32Frmt:
						printSigVar(mstring, vMenu, vMenu->prmHandle->prm->_s32Frmt);
						break;
					case floatFrmt:
						//printFloatVar(mstring, vMenu);
						break;
					case unixDateFrmt:
						gmtime_r(&vMenu->prmHandle->prm->_unixDateFrmt, &tm);
						strftime(mstring, sizeof(mstring), "%Y.%m.%d", &tm);
						break;
					case unixTimeFrmt:
						gmtime_r(&vMenu->prmHandle->prm->_unixTimeFrmt, &tm);
						strftime(mstring, sizeof(mstring), "%H:%M:%S", &tm);
						break;
					default:
						strcpy(mstring, "err type");
				}

				uint8_t slen = strlen(mstring);
				memset(mstring + slen, ' ', (MENU_SCREEN_W - MENU_POS_VAR) / 8 - slen);
				mstring[(MENU_SCREEN_W - MENU_POS_VAR) / 8] = '\0';
				lcd_putStr(MENU_POS_VAR, MENU_Y_DISTANCE + itemNum * MENU_Y_DISTANCE, &font8x12, 0, mstring);		//Вывод на дисплей
			}

			if(vMenu == vMenu->next){
				break;
			}
			vMenu = vMenu->next;
			itemNum++;
		}

		/*************************************/
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(MENU_PERIOD));
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
			sprintf(string, "%.0f%s", menuItem->prmHandle->prm->_floatFrmt, menuItem->units);
			break;
		case 1:
			sprintf(string, "%.1f%s", menuItem->prmHandle->prm->_floatFrmt, menuItem->units);
			break;
		case 2:
			sprintf(string, "%.2f%s", menuItem->prmHandle->prm->_floatFrmt, menuItem->units);
			break;
		case 3:
			sprintf(string, "%.3f%s", menuItem->prmHandle->prm->_floatFrmt, menuItem->units);
			break;
		case 4:
			sprintf(string, "%.41f%s", menuItem->prmHandle->prm->_floatFrmt, menuItem->units);
			break;
		case 5:
			sprintf(string, "%.5f%s", menuItem->prmHandle->prm->_floatFrmt, menuItem->units);
			break;
		case 6:
			sprintf(string, "%.6f%s", menuItem->prmHandle->prm->_floatFrmt, menuItem->units);
			break;
		default:
			sprintf(string, "");
	}
}

/******************* (C) COPYRIGHT ***************** END OF FILE ********* D_EL *****/

/*!****************************************************************************
 * @file    menuSystem.h
 * @author  Storozhenko Roman - D_EL
 * @version V1.0
 * @date    10.02.2016
 * @brief   parametres system
 */
#ifndef MENUSYSTEM_H
#define MENUSYSTEM_H

/*!****************************************************************************
 * Include
 */
#include "stdint.h"
#include "stddef.h"
#include "prmSystem.h"
#include "rtc.h"
#include "time.h"
#include "FreeRTOS.h"
#include "spfd54124b.h"
#include "key.h"
#include "ui.h"

/*!****************************************************************************
 * Define
 */
#define MENU_PERIOD   20	//[ms]

/*!****************************************************************************
 * User enum
 */

/*!****************************************************************************
 * User typedef
 */
typedef enum {
	chmodMenuNone, chmodMenuAlways,
} chmodMenu_type;

typedef union {
	struct {
		uint16_t istext :1;
		chmodMenu_type chmod :2;
		uint16_t pfParamert :4;
	} bit;
	uint16_t all;
} menuFlags_type;

typedef struct menuItem{
	char *label;
	char *units;
	const prmHandle_type *prmHandle;
	uint32_t (*pfChanges)(const struct menuItem*);
	uint32_t (*pfSelect)(const struct menuItem*); 	/**< Pointer to the optional menu-specific select callback of this menu item */
	uint32_t (*pfUnselect)(const struct menuItem*); /**< Pointer to the optional menu-specific unselect callback of this menu item */
	uint32_t (*pfPeriod)(const struct menuItem*); 	/**< Pointer to the optional menu-specific periodic callback of this menu item */

	const struct menuItem *next; 		/**< Pointer to the next menu item of this menu item */
	const struct menuItem *previous; 	/**< Pointer to the previous menu item of this menu item */
	const struct menuItem *parent; 		/**< Pointer to the parent menu item of this menu item */
	const struct menuItem *child; 		/**< Pointer to the child menu item of this menu item */

	menuFlags_type flags;
} menuItem_type;

/*!****************************************************************************
 * Extern viriables
 */
#define MENU_ITEM(_name, _label, _units, _prmHandle, _chmod, _pfPrm, _pfChanges, _pfSelect, _pfUnselect, _pfPeriod, _previous, _next, _parent , _child) \
	extern const menuItem_type item_##_name;
#include "menuTree.h"
#undef MENU_ITEM

/*!****************************************************************************
 * Macro functions
 */

/*!****************************************************************************
 * Prototypes for the functions
 */
void menuEngine(const menuItem_type *startMenuItem);
void printUsigVar(char *string, const menuItem_type *menuItem, uint32_t var);
void printSigVar(char *string, const menuItem_type *menuItem, int32_t var);
void printFloatVar(char *string, const menuItem_type *menuItem);

#endif /* MENUSYSTEM_H */
/******************* (C) COPYRIGHT ***************** END OF FILE ********* D_EL *****/

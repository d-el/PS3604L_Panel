/*!****************************************************************************
 * @file		menuSystem.h
 * @author		Storozhenko Roman - D_EL
 * @version		V1.3
 * @copyright	GNU Lesser General Public License v3
 * @date		06.061.2018
 * @brief		menu system
 */
#ifndef MENUSYSTEM_H
#define MENUSYSTEM_H

/*!****************************************************************************
 * Include
 */
#include "stdint.h"
#include "prmSystem.h"

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
#define MENU_ITEM(_name, _label, _units, _prmHandle, _chmod, _pfPrm, _pfChanges, _pfSelect, _pfUnselect, _pfPeriod, _previous, _next, _parent , _child) \
	mN_##_name,
typedef enum{
	#include "menuTree.h"
}menuItemNumber_type;
#undef MENU_ITEM

typedef enum {
	chmodMenuNone,
	chmodMenuAlways,
} chmodMenu_type;

typedef enum {
	menuItemOk,
	menuItemMessage,
	menuItemWarning,
	menuItemError
} menuItemState_type;

typedef union {
	struct {
		uint16_t istext :1;
		chmodMenu_type chmod :2;
		uint16_t pfParamert :4;
	} bit;
	uint16_t all;
} menuFlags_type;

typedef struct itemState{
	uint8_t state;
	char *string;
}itemState_type;

typedef struct menuItem{
	char *label;
	char *units;
	const prmHandle_type *prmHandle;
	itemState_type (*pfChanges)(const struct menuItem*);
	itemState_type (*pfSelect)(const struct menuItem*);		/**< Pointer to the optional menu-specific select callback of this menu item */
	itemState_type (*pfUnselect)(const struct menuItem*);	/**< Pointer to the optional menu-specific unselect callback of this menu item */
	itemState_type (*pfPeriod)(const struct menuItem*);		/**< Pointer to the optional menu-specific periodic callback of this menu item */

	const struct menuItem *next; 		/**< Pointer to the next menu item of this menu item */
	const struct menuItem *previous; 	/**< Pointer to the previous menu item of this menu item */
	const struct menuItem *parent; 		/**< Pointer to the parent menu item of this menu item */
	const struct menuItem *child; 		/**< Pointer to the child menu item of this menu item */

	menuFlags_type flags;
} menuItem_type;

/*!****************************************************************************
 * External variables
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
void menuEngine(menuItemNumber_type menuItemNumber);

#endif /* MENUSYSTEM_H */
/*************** LGPL ************** END OF FILE *********** D_EL ************/

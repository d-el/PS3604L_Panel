/*!****************************************************************************
 * @file		menuSystem.h
 * @author		Storozhenko Roman - D_EL
 * @version		V2.0
 * @date		17.02.2021
 * @copyright	The MIT License (MIT). Copyright (c) 2021 Storozhenko Roman
 * @brief		menu system
 */
#ifndef MENUSYSTEM_H
#define MENUSYSTEM_H

/*!****************************************************************************
 * Include
 */
#include "stdint.h"
#include "prmSystem.h"

namespace Menu {

//typedef enum {
//	menuItemOk,
//	menuItemMessage,
//	menuItemWarning,
//	menuItemError
//} menuItemState_type;
//
//typedef enum {
//    menuItemUnselect,
//    menuItemSelect,
//    menuItemUnselectUnchangeable,
//    menuItemSelectUnchangeable
//} menuItemSelect_type;
//
//typedef union {
//	struct {
//		int16_t istext :1;
//		chmodMenu_type chmod :2;
//		int16_t pfParamert :4;
//	} bit;
//	int16_t all;
//} menuFlags_type;

struct ItemState{
	bool state;
	char *string;
};

class MenuItem{
public:
	using Callcack = ItemState (*)(const MenuItem*);

public:
	constexpr MenuItem(const char *_label, Prm::IVal *_prm, bool _change, uint16_t _arg,
			Callcack _pChange, Callcack _pSelect, Callcack _pUnselect, Callcack _pPeriod,
			const MenuItem* _next, const MenuItem* _previous, const MenuItem* _parent, const MenuItem* _child
			):
	label(_label),
	prm(_prm),
	change(_change),
	arg(_arg),
	pChange(_pChange),
	pSelect(_pSelect),
	pUnselect(_pUnselect),
	pPeriod(_pPeriod),
	next(_next),
	previous(_previous),
	parent(_parent),
	child(_child)
	{};

public:
	const char *label;
	Prm::IVal *prm;
	bool change;
	uint16_t arg;

	Callcack pChange;
	Callcack pSelect;
	Callcack pUnselect;
	Callcack pPeriod;

	const MenuItem* next;
	const MenuItem* previous;
	const MenuItem* parent;
	const MenuItem* child;
};

bool run(const MenuItem *m);

};

#endif /* MENUSYSTEM_H */
/******************************** END OF FILE ********************************/

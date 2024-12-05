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

struct ItemState{
	bool state;
	const char *string;
};

class MenuItem{
public:
	using Callcack = ItemState (*)(const MenuItem*);
	using Editor = ItemState (*)(const MenuItem* history[], uint8_t historyIndex);

public:
	constexpr MenuItem(const char *_label, Prm::IVal*_prm, bool _change, uint16_t _arg,
			Callcack _pChange, Callcack _pSelect, Callcack _pUnselect, Callcack _pPeriod,
			const MenuItem* _next, const MenuItem* _previous, const MenuItem* _child = nullptr,
			Editor _editor = nullptr
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
	child(_child),
	editor(_editor)
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
	const MenuItem* child;

	const Editor editor;
};

bool run(const MenuItem *m);
ItemState clockEditor(const MenuItem* history[], uint8_t historyIndex);
ItemState ipAddressEditor(const MenuItem* history[], uint8_t historyIndex);
ItemState ipMacEditor(const MenuItem* history[], uint8_t historyIndex);

};

#endif /* MENUSYSTEM_H */
/******************************** END OF FILE ********************************/

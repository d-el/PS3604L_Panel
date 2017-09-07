/*!****************************************************************************
 * @file    ui.h
 * @author  d_el
 * @version V1.0
 * @date    20.12.2015, by d_el
 * @brief   user interface
 */
#ifndef ui_H
#define ui_H

/*!****************************************************************************
 * Include
 */
#include "stdint.h"
#include "spfd54124b.h"

/*!****************************************************************************
 * User define
 */

/*!****************************************************************************
 * User enum
 */

/*!****************************************************************************
 * User typedef
 */
typedef struct {
	uint16_t cursor;
	uint16_t voltage;
	uint16_t current;
	uint16_t capacity;
	uint16_t imax;
	uint16_t mode;
} uicolor_type;

typedef struct {
	uint16_t freq;
	uint16_t time;
} beepmem_type;

typedef struct {
	beepmem_type welcome;
	beepmem_type goodbye;
	beepmem_type key;
	beepmem_type vallim;
	beepmem_type cvToCc;
	beepmem_type ccToCv;
	beepmem_type ovfCurrent;
	beepmem_type chargeReady;
	beepmem_type error;
	beepmem_type encoLim;
	beepmem_type encoTransition;
} uibeep_type;

typedef struct {
	uicolor_type color;
	uibeep_type beep;
} ui_type;

/*!****************************************************************************
 * Extern viriables
 */
extern ui_type ui;

/*!****************************************************************************
 * Macro functions
 */

/*!****************************************************************************
 * Prototypes for the functions
 */

#endif //ui_H
/***************** (C) COPYRIGHT ************** END OF FILE ******** d_el ****/

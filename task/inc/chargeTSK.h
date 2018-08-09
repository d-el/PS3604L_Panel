/*!****************************************************************************
 * @file		charge.h
 * @author		d_el
 * @version		V1.0
 * @date		31.01.20187
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 */
#ifndef CHARGE_H
#define CHARGE_H

/*!****************************************************************************
 * Include
 */
#include "stdio.h"

/******************************************************************************
 * Define
 */
#define CH_TSK_PERIOD   10      //[ms]

/*!****************************************************************************
 * Enumeration
 */
enum {
	C_VOLT = 0,
	C_CURR = 1,
	C_TIME = 2,
	C_MODE = 3,
};

enum {
	ch_modeTime = 0,
	ch_modeCurrent = 1,
};

/*!****************************************************************************
 * Typedef
 */
typedef struct {
	uint16_t u;		//[mV]
	uint16_t i;		//[mA]
	uint16_t t;		//[minute]
	uint16_t mode;
} charge_type;

/******************************************************************************
 * External variables
 */
extern charge_type ch;

/******************************************************************************
 * Macro functions
 */

/*!****************************************************************************
 * Function declaration
 */
void chargeTSK(void *pPrm);

#endif //CHARGE_H
/******************************** END OF FILE ********************************/

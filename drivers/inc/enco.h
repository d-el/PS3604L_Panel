/*!****************************************************************************
 * @file		enco.h
 * @author		Storozhenko Roman - D_EL
 * @version		V2.3
 * @date		13.05.2015
 * @date		14.11.2016 fix "temp > enco.top"
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		Средства работы с энкодером
 */
#ifndef ENCO_H
#define ENCO_H

/*!****************************************************************************
 * Include
 */
#include "stdint.h"
#include "prmSystem.h"

/*!****************************************************************************
 * User define
 */

/*!****************************************************************************
 * User typedef
 */
typedef enum {
	enNoCharge,
	enCharge,
	enNoLim,
	enLimDown,
	enLimUp,
	enTransitionDown,
	enTransitionUp,
} enStatus_type;

typedef struct {
	uint16_t ntic;
} enco_type;

/*!****************************************************************************
 * External variables
 */
extern enco_type enco;

/*!****************************************************************************
 * Macro functions
 */

/*!****************************************************************************
 * Prototypes for the functions
 */
void enco_init(void);
uint16_t enGeReg(void);
void enSetReg(uint16_t val);
void enWriteVal(const prmHandle_type *prmHandle, const prmval_type *prmval);
enStatus_type enAdd(const prmHandle_type *prmHandle, const prmval_type *prmval, int32_t multiply);
enStatus_type enBigStepUp(const prmHandle_type *prmHandle);
enStatus_type enBigStepDown(const prmHandle_type *prmHandle);
enStatus_type enUpDate(const prmHandle_type *prmHandle);
void enSetNtic(uint16_t n);

#endif //ENCO_H
/******************************** END OF FILE ********************************/

/*!****************************************************************************
 * @file    enco.h
 * @author  Storozhenko Roman - D_EL
 * @version V2.3
 * @date    13.05.2015
 * @date    14.11.2016   fix "temp > enco.top"
 * @brief   Средства работы с энкодером
 * @copyright GNU Public License
 */
#ifndef ENCO_H
#define ENCO_H

/*!****************************************************************************
 * Include
 */
#include "stm32f4xx.h"
#include "gpio.h"
#include "prmSystem.h"

/*!****************************************************************************
 * User define
 */

/*!****************************************************************************
 * User typedef
 */
typedef enum {
	enNONCIRC, enCIRC
} enMode_type;

typedef enum {
	enU8, enS8, enU16, enS16, enU32, enS32, enFloat
} enMemType_type;

typedef enum {
	enNoCharge, enCharge, enNoLim, enLimDown, enLimUp, enTransitionDown, enTransitionUp,
} enStatus_type;

typedef union {
	uint8_t u8v;
	int8_t s8v;
	uint16_t u16v;
	int16_t s16v;
	uint32_t u32v;
	int32_t s32v;
	float fv;
} encoVal_type;

typedef struct {
	//void            *pval;
	//encoVal_type    min;
	//encoVal_type    max;
	//encoVal_type    step;
	//encoVal_type    bigStep;
	//enMemType_type  memtype;
	//enMode_type     mode;
	uint16_t ntic;
} enco_type;

/*!****************************************************************************
 * Extern viriables
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
/*************** GNU GPL ************** END OF FILE ********* D_EL ***********/

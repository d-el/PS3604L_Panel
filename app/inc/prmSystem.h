/*!****************************************************************************
 * @file		prmSystem.h
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		08.02.2017
 * @copyright	GNU Lesser General Public License v3
 * @brief		Parameters system
 */
#ifndef PRMSYSTEM_H
#define PRMSYSTEM_H

/*!****************************************************************************
 * Include
 */
#include "stdint.h"

/*!****************************************************************************
 * Define
 */
#define typeu8Frmt		uint8_t
#define	types8Frmt		int8_t
#define	typeu16Frmt		uint16_t
#define	types16Frmt		int16_t
#define	typeu32Frmt		uint32_t
#define	types32Frmt		int32_t
#define	typesfloatFrmt	float

#define	power0			1U
#define	power1			10U
#define	power2			100U
#define	power3			1000U
#define	power4			10000U
#define	power5			100000U
#define	power6			1000000U

/*!****************************************************************************
 * Enumeration
 */
#define parametres(_label, _units, _prm, _type, _chmod, _def, _min, _max, _step, _bigstep, _power, _save)	\
	N##_label,

typedef enum {
#include "parametres.h"
	endOfNumberPrm,
} parametresNum_type;

#undef parametres

/*!****************************************************************************
 * Ttypedef
 */
typedef union {
	uint8_t 	_u8Frmt;
	int8_t 		_s8Frmt;
	uint16_t 	_u16Frmt;
	int16_t 	_s16Frmt;
	uint32_t 	_u32Frmt;
	int32_t 	_s32Frmt;
	float 		_floatFrmt;
	uint32_t 	_unixTimeFrmt;
	uint32_t 	_unixDateFrmt;
} prmval_type;

typedef enum {
	u8Frmt,
	s8Frmt,
	u16Frmt,
	s16Frmt,
	u32Frmt,
	s32Frmt,
	floatFrmt,
	unixTimeFrmt,
	unixDateFrmt,
} typePrm_type;

typedef enum {
	chmodNone,
	chmodAlways,
} chmodPrm_type;

typedef enum {
	prmNotSave,
	prmEeprom,
} nvSavePrm_type;

typedef struct {
	prmval_type 	*prm;			//Pointer to parametr
	prmval_type 	def;
	prmval_type 	min;
	prmval_type 	max;
	prmval_type 	step;
	prmval_type 	bigstep;
	typePrm_type 	type 	:4;
	chmodPrm_type 	chmod 	:2;
	uint8_t 		power 	:4;
	nvSavePrm_type 	save 	:2;
} prmHandle_type;

/*!****************************************************************************
 * Exported variables
 */
extern const prmHandle_type prmh[];
extern const uint16_t prmHandleLen;

/*!****************************************************************************
 * Macro functions
 */

/*!****************************************************************************
 * Function declaration
 */
void prm_setVal(const prmHandle_type *const prmHandle, const prmval_type *const prmval);
void prm_loadDefault(void);

#endif /* PRMSYSTEM_H */
/*************** LGPL ************** END OF FILE *********** D_EL ************/

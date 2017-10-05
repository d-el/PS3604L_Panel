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

//! Number parameter handler
#define parametres(m_label, m_units, m_prm, m_type, m_chmod, m_def, m_min, m_max, m_step, m_bigstep, m_power, m_limtype, m_save)	\
	N##m_label,
typedef enum {
#include "parametres.h"
	endOfNumberPrm
} parametresNum_type;
#undef parametres

/*!****************************************************************************
 * Typedef
 */
typedef uint8_t u8;

typedef union {
	uint8_t 	_u8Frmt;
	uint8_t		u8;
	uint8_t 	u8Frmt;
	int8_t 		_s8Frmt;
	uint16_t 	_u16Frmt;
	int16_t 	_s16Frmt;
	uint32_t 	_u32Frmt;
	int32_t 	_s32Frmt;
	float 		_floatFrmt;
	uint32_t 	_unixTimeFrmt;
	uint32_t 	_unixDateFrmt;
	uint32_t	_ipAdrFrmt;

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
	ipAdrFrmt,
} typePrm_type;

typedef enum {
	chmodNone,
	chmodAlways,
} chmodPrm_type;

typedef enum {
	prmNotSave,
	prmEeprom,
} nvSavePrm_type;

typedef enum {
	prmLimConst,
	prmLimVariable,
} prmLim_type;

typedef struct {
	prmval_type 	*prm;			//Pointer to parametr
	prmval_type 	def;
	prmval_type 	min;
	prmval_type 	max;
	prmval_type 	step;
	prmval_type 	*pstep;
	prmval_type 	bigstep;
	typePrm_type 	type 	:4;
	chmodPrm_type 	chmod 	:2;
	uint8_t 		power 	:4;
	nvSavePrm_type 	save 	:2;
	prmLim_type		prmLim	:1;
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

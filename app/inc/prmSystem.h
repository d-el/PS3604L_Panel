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
/*
 * - 0000 -----------------------------------
 * ------------------------------------------
 * ------ Пользовательские настройки
 * ------------------------------------------
 * ------------------------------------------
 * - 0512 -----------------------------------
 * ------------------------------------------
 * ------ Системные данные
 * ------------------------------------------
 * ------------------------------------------
 * - 1023 -----------------------------------
 */
#define USEREEPADR		((void*)0)
#define SYSEEPADR		((void*)512)

#define typeu8Frmt		uint8_t
#define	types8Frmt		int8_t
#define	typeu16Frmt		uint16_t
#define	types16Frmt		int16_t
#define	typeu32Frmt		uint32_t
#define	types32Frmt		int32_t
#define	typesfloatFrmt	float

#define	power0			1
#define	power1			10
#define	power2			100
#define	power3			1000U
#define	power4			10000U
#define	power5			100000U
#define	power6			1000000U

/*!****************************************************************************
 * Enumeration
 */

//! Number parameter handler
#define parametres(m_label, m_units, m_prm, m_type, m_chmod, m_def, m_min, m_max, m_step, m_bigstep, m_power, m_limtype, m_steptype, m_save)	\
	N##m_label,
typedef enum {
	#include "parametres.h"
	endOfNumberPrm
} parametresNum_type;
#undef parametres

/*!****************************************************************************
 * Typedef
 */
typedef union {
	uint8_t 	t_u8Frmt;
	int8_t 		t_s8Frmt;
	uint16_t 	t_u16Frmt;
	int16_t 	t_s16Frmt;
	uint32_t 	t_u32Frmt;
	int32_t 	t_s32Frmt;
	float 		t_floatFrmt;
	uint32_t 	t_unixTimeFrmt;
	uint32_t 	t_unixDateFrmt;
	uint32_t	t_ipAdrFrmt;
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
} prmType_type;

typedef enum {
	chmodNone,
	chmodAlways,
} prmChmod_type;

typedef enum {
	prmNotSave,
	prmEepSys,
	prmEep,
} prmNvSave_type;

typedef enum {
	prmLimConst,
	prmLimVariable,
} prmLim_type;

typedef enum {
	prmStepConst,
	prmStepVariable,
} prmStep_type;

typedef enum {
	prm_ok,
	prm_addrIsNull,
	prm_signatureError,
	prm_crcError,
	prm_errorSizeMem,
	prm_writeError,
	prm_readError,
	prm_error
} prm_state_type;

typedef struct {
	prmval_type		*prm;			//Pointer to parameter
	prmval_type		def;
	prmval_type		*min;
	prmval_type		*max;
	prmval_type		*step;
	prmval_type		bigstep;
	prmType_type	type 	:4;
	prmChmod_type	chmod 	:2;
	uint8_t			power 	:4;
	prmNvSave_type	save 	:2;
	prmLim_type		limType :1;
	prmStep_type	stepType:1;
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
void prm_loadDefault(prmNvSave_type prmNvSave);
prm_state_type prm_store(void *pMemory, prmNvSave_type prmNvSave);
prm_state_type prm_load(void *pMemory, prmNvSave_type prmNvSave);

#endif /* PRMSYSTEM_H */
/*************** LGPL ************** END OF FILE *********** D_EL ************/

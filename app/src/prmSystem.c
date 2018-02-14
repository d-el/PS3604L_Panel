/*!****************************************************************************
 * @file		prmSystem.c
 * @author		d_el - Storozhenko Roman
 * @version		V1.1
 * @date		18.10.2017
 * @copyright	GNU Lesser General Public License v3
 * @brief		Parameters system
 */

/*!****************************************************************************
 * Include
 */
#include "string.h"
#include "crc.h"
#include "24AAxx.h"
#include "prmSystem.h"
#include "systemTSK.h"
#include "baseTSK.h"
#include "settingTSK.h"
#include "chargeTSK.h"
#include "startupTSK.h"

prmval_type abc = { .t_u32Frmt = 10000 };

/*!****************************************************************************
 * MEMORY
 */
//! prmLimConstMin, prmLimVariableMin
#define parametres_prmLimConst(m_label, m_type, m_min, m_max, m_step, m_bigstep, m_power)	\
	static const prmval_type prmLimConstMin_##m_label = { .t_##m_type = (m_min * power##m_power) };
#define parametres_prmLimVariable(m_label, m_type, m_min, m_max, m_step, m_bigstep, m_power) \
	static prmval_type prmLimVariableMin_##m_label = { .t_##m_type = (m_min * power##m_power) };
#define parametres(m_label, m_units, m_prm, m_type, m_chmod, m_def, m_min, m_max, m_step, m_bigstep, m_power, m_limType, m_stepType, m_save)	\
	parametres_##m_limType(m_label, m_type, m_min, m_max, m_step, m_bigstep, m_power)
#include "parametres.h"
#undef parametres_prmLimConst
#undef parametres_prmLimVariable
#undef parametres

//! prmLimConstMax, prmLimVariableMax
#define parametres_prmLimConst(m_label, m_type, m_min, m_max, m_step, m_bigstep, m_power)	\
	static const prmval_type prmLimConstMax_##m_label = { .t_##m_type = (m_max * power##m_power) };
#define parametres_prmLimVariable(m_label, m_type, m_min, m_max, m_step, m_bigstep, m_power) \
	static prmval_type prmLimVariableMax_##m_label = { .t_##m_type = (m_max * power##m_power) };
#define parametres(m_label, m_units, m_prm, m_type, m_chmod, m_def, m_min, m_max, m_step, m_bigstep, m_power, m_limType, m_stepType, m_save)	\
	parametres_##m_limType(m_label, m_type, m_min, m_max, m_step, m_bigstep, m_power)
#include "parametres.h"
#undef parametres_prmLimConst
#undef parametres_prmLimVariable
#undef parametres

//! prmStep
#define parametres_prmStepConst(m_label, m_type, m_min, m_max, m_step, m_bigstep, m_power)	\
	static const prmval_type prmStepConst_##m_label = { .t_##m_type = (m_step * power##m_power) };
#define parametres_prmStepVariable(m_label, m_type, m_min, m_max, m_step, m_bigstep, m_power) \
	static prmval_type prmStepVariable_##m_label = {.t_##m_type = (m_step * power##m_power) };
#define parametres(m_label, m_units, m_prm, m_type, m_chmod, vm_def, m_min, m_max, m_step, m_bigstep, m_power, m_limType, m_stepType, m_save)	\
	parametres_##m_stepType(m_label, m_type, m_min, m_max, m_step, m_bigstep, m_power)
#include "parametres.h"
#undef parametres_prmStepConst
#undef parametres_prmStepVariable
#undef parametres

//! Parameter handler macros
#define parametres(m_label, m_units, m_prm, m_type, m_chmod, m_def, m_min, m_max, m_step, m_bigstep, m_power, m_limType, m_stepType, m_save)	\
{																		\
	.prm 				= (prmval_type*)&m_prm,							\
	.def.t_##m_type 	= (m_def * power##m_power),						\
	.min 				= (prmval_type*)&m_limType##Min_##m_label,		\
	.max 				= (prmval_type*)&m_limType##Max_##m_label,		\
	.step				= (prmval_type*)&m_stepType##_##m_label,		\
	.bigstep.t_##m_type = (m_bigstep * power##m_power),					\
	.type 				= m_type,										\
	.chmod 				= m_chmod,										\
	.power				= m_power,										\
	.limType			= m_limType,									\
	.stepType			= m_stepType,									\
	.save				= m_save,										\
},
//! Parameters handlers
const prmHandle_type prmh[] = {
	#include "parametres.h"
};
//! Parameters handlers length
const uint16_t prmHandleLen = sizeof(prmh) / sizeof(prmHandle_type);
#undef parametres

//! Type size table
static const uint8_t sizePrm[] = {
	1,	//u8Frmt
	1,	//s8Frmt
	2,	//u16Frmt
	2,	//s16Frmt
	4,	//u32Frmt
	4,	//s32Frmt
	4,	//floatFrmt
	4,	//unixTimeFrmt
	4,	//unixDateFrmt
	4,	//ipAdrFrmt
};

static uint8_t prmLoadBff[1024];

/*!****************************************************************************
 * @brief	get parameter size
 * @param[in]	parameter type
 * @retval		size in bytes
 */
uint8_t prm_getSize(prmType_type type){
	return sizePrm[type];
}

/*!****************************************************************************
 * @brief	Write value to parameter
 */
void prm_setVal(const prmHandle_type *const prmHandle, const prmval_type *const prmval){
	switch(prmHandle->type){
		case u8Frmt:
			prmHandle->prm->t_u8Frmt = prmval->t_u8Frmt;
			break;
		case s8Frmt:
			prmHandle->prm->t_s8Frmt = prmval->t_s8Frmt;
			break;
		case u16Frmt:
			prmHandle->prm->t_u16Frmt = prmval->t_u16Frmt;
			break;
		case s16Frmt:
			prmHandle->prm->t_s16Frmt = prmval->t_s16Frmt;
			break;
		case u32Frmt:
		case unixTimeFrmt:
		case unixDateFrmt:
		case ipAdrFrmt:
			prmHandle->prm->t_u32Frmt = prmval->t_u32Frmt;
			break;
		case s32Frmt:
			prmHandle->prm->t_s32Frmt = prmval->t_s32Frmt;
			break;
		case floatFrmt:
			prmHandle->prm->t_floatFrmt = prmval->t_floatFrmt;
			break;
	}
}

/*!****************************************************************************
 * @brief	Load to all parameters default value
 */
void prm_loadDefault(prmNvSave_type prmNvSave){
	for(uint32_t iterator = 0; iterator < prmHandleLen; iterator++){
		if(prmh[iterator].save == prmNvSave){
			prm_setVal(&prmh[iterator], &prmh[iterator].def);
		}
	}
}

/*!****************************************************************************
 * @brief	Store all parameters
 */
prm_state_type prm_store(void *pMemory, prmNvSave_type prmNvSave){
	uint16_t signature = 0x2805;
	uint16_t crc;
	uint32_t size;
	uint32_t prmSize;
	uint8_t *pbuf = &prmLoadBff[0];

	// Copy data to signature
	memcpy(pbuf, &signature, sizeof(signature));
	size = sizeof(signature);

	// Copy data to buffer
	for(uint32_t iterator = 0; iterator < prmHandleLen; iterator++){
		if(prmh[iterator].save == prmNvSave){
			memcpy(pbuf + size, prmh[iterator].prm, prm_getSize(prmh[iterator].type));
			size += prm_getSize(prmh[iterator].type);
		}
	}

	// Calculate CRC
	crc = crc16Calc(&crcModBus, pbuf, size);

	// Copy CRC
	memcpy(pbuf + size, &crc, sizeof(crc));
	size += sizeof(crc);

	// Write to memory
	uint8_t memState = eep_write((uint32_t)pMemory, pbuf, size);
	if(memState != 0){
		return prm_writeError;
	}

	return prm_ok;
}

/*!****************************************************************************
 * @brief	Load all parameters
 */
prm_state_type prm_load(void *pMemory, prmNvSave_type prmNvSave){
	uint16_t signature;
	uint16_t crc;
	uint16_t size;
	uint8_t *pbuf = &prmLoadBff[0];

	// Calculate size packet
	size = sizeof(signature);
	for(uint32_t iterator = 0; iterator < prmHandleLen; iterator++){
		if(prmh[iterator].save == prmNvSave){
			size += prm_getSize(prmh[iterator].type);
		}
	}
	size += sizeof(crc);

	// Read from memory
	uint8_t memState = eep_read(pbuf, (uint32_t)pMemory, size);
	if(memState != 0){
		return prm_readError;
	}

	// Check signature
	memcpy(&signature, pbuf, sizeof(signature));
	if(signature != 0x2805){
		return prm_signatureError;
	}

	// Check CRC
	crc = crc16Calc(&crcModBus, pbuf, size);
	if(crc != 0){
		return prm_crcError;
	}

	// Copy data
	pbuf += sizeof(signature);
	for(uint32_t iterator = 0; iterator < prmHandleLen; iterator++){
		if(prmh[iterator].save == prmNvSave){
			prm_setVal(&prmh[iterator], (prmval_type*)pbuf);
			pbuf += prm_getSize(prmh[iterator].type);
		}
	}

	return prm_ok;
}

/*************** LGPL ************** END OF FILE *********** D_EL ************/

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
#include "prmSystem.h"
#include "systemTSK.h"
#include "rtc.h"

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
void prm_loadDefault(void){
	for(uint32_t iterator = 0; iterator < prmHandleLen; iterator++){
		prm_setVal(&prmh[iterator], &prmh[iterator].def);
	}
}

/*************** LGPL ************** END OF FILE *********** D_EL ************/

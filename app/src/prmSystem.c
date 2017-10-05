/*!****************************************************************************
 * @file		prmSystem.c
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		08.02.2017
 * @copyright	GNU Lesser General Public License v3
 * @brief		Parameters system
 */

/*!****************************************************************************
 * Include
 */
#include "prmSystem.h"
#include "systemTSK.h"
#include "rtc.h"

/*!****************************************************************************
 * MEMORY
 */
////! Parameter constant step
//#define parametres_prmLimConst(_type, _min, _max, _step, _bigstep, _power, _limtype)	\
//	_step * power##_power,
//#define parametres_prmLimVariable(_type, _min, _max, _step, _bigstep, _power, _limtype)
//#define parametres(_label, _units, _prm, _type, _chmod, _def, _min, _max, _step, _bigstep, _power, _limtype, _save)	\
//	parametres_##_limtype(_type, _min, _max, _step, _bigstep, _power, _limtype)
//const prmval_type prmLimConstStep[] = {
//	#include "parametres.h"
//};
//#undef parametres_prmLimConst
//#undef parametres_prmLimVariable
//#undef parametres
//
////! Parameter variable step
//#define parametres_prmLimConst(_type, _min, _max, _step, _bigstep, _power, _limtype)
//#define parametres_prmLimVariable(_type, _min, _max, _step, _bigstep, _power, _limtype) \
//	_step * power##_power,
//#define parametres(_label, _units, _prm, _type, _chmod, _def, _min, _max, _step, _bigstep, _power, _limtype, _save)	\
//	parametres_##_limtype(_type, _min, _max, _step, _bigstep, _power, _limtype)
//prmval_type prmLimVariableStep[] = {
//	#include "parametres.h"
//};
//#undef parametres_prmLimConst
//#undef parametres_prmLimVariable
//#undef parametres


//! prmLim Step
#define parametres_prmLimConst(m_label, m_type, m_min, m_max, m_step, m_bigstep, m_power)	\
	static const prmval_type prmLimConstStep##_##m_label = { ._##ipAdrFrmt = m_step * m_power };
#define parametres_prmLimVariable(m_label, m_type, m_min, m_max, m_step, m_bigstep, m_power) \
	static prmval_type prmLimVariableStep##_##m_label = { m_step * m_power };
#define parametres(m_label, m_units, m_prm, m_type, m_chmod, m_def, m_min, m_max, m_step, m_bigstep, m_power, m_limtype, m_save)	\
	parametres_##m_limtype(m_label, m_type, m_min, m_max, m_step, m_bigstep, m_power)
#include "parametres.h"
#undef parametres_prmLimConst
#undef parametres_prmLimVariable
#undef parametres


//! Parameter handler macros
#define parametres(m_label, m_units, m_prm, m_type, m_chmod, m_def, m_min, m_max, m_step, m_bigstep, m_power, m_limtype, m_save)	\
{														\
	.prm 				= (prmval_type*)&m_prm,			\
	.def._##m_type 		= (m_def * power##m_power),		\
	.min._##m_type 		= (m_min * power##m_power),		\
	.max._##m_type 		= (m_max * power##m_power),		\
	.step._##m_type 		= (m_step * power##m_power),		\
	.pstep				= (prmval_type*)&m_limtype##Step_##m_label,			\
	.bigstep._##m_type 	= (m_bigstep * power##m_power),	\
	.type 				= m_type,						\
	.chmod 				= m_chmod,						\
	.power				= m_power,						\
	.prmLim				= m_limtype,						\
	.save				= m_save,						\
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
			prmHandle->prm->_u8Frmt = prmval->_u8Frmt;
			break;
		case s8Frmt:
			prmHandle->prm->_s8Frmt = prmval->_s8Frmt;
			break;
		case u16Frmt:
			prmHandle->prm->_u16Frmt = prmval->_u16Frmt;
			break;
		case s16Frmt:
			prmHandle->prm->_s16Frmt = prmval->_s16Frmt;
			break;
		case u32Frmt: unixTimeFrmt: unixDateFrmt:
			prmHandle->prm->_u32Frmt = prmval->_u32Frmt;
			break;
		case s32Frmt:
			prmHandle->prm->_s32Frmt = prmval->_s32Frmt;
			break;
		case floatFrmt:
			prmHandle->prm->_floatFrmt = prmval->_floatFrmt;
			break;
		case ipAdrFrmt:
			prmHandle->prm->_ipAdrFrmt = prmval->_ipAdrFrmt;
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

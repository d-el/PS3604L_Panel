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
#define parametres(_label, _units, _prm, _type, _chmod, _def, _min, _max, _step, _bigstep, _power, _save)	\
{														\
	.prm 				= (prmval_type*)&_prm,			\
	.def._##_type 		= (_def * power##_power),		\
	.min._##_type 		= (_min * power##_power),		\
	.max._##_type 		= (_max * power##_power),		\
	.step._##_type 		= (_step * power##_power),		\
	.bigstep._##_type 	= (_bigstep * power##_power),	\
	.type 				= _type,						\
	.chmod 				= _chmod,						\
	.power				= _power,						\
	.save				= _save,						\
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
		case u32Frmt:
			unixTimeFrmt: unixDateFrmt: prmHandle->prm->_u32Frmt = prmval->_u32Frmt;
			break;
		case s32Frmt:
			prmHandle->prm->_s32Frmt = prmval->_s32Frmt;
			break;
		case floatFrmt:
			prmHandle->prm->_floatFrmt = prmval->_floatFrmt;
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

/*!****************************************************************************
 * @file		prmEditor.c
 * @author		d_el
 * @version		V1.0
 * @date		16.12.2018
 * @copyright	The MIT License (MIT). Copyright (c) 2018 Storozhenko Roman
 * @brief		--
 */

/*!****************************************************************************
 * Include
 */
#include "enco.h"
#include "prmEditor.h"

/*!****************************************************************************
 * MEMORY
 */
static uint16_t prmEditorNtic;

/*!****************************************************************************
 * @brief
 */
void prmEditorWriteVal(const prmHandle_type *prmHandle, const prmval_type *prmval){
	prm_setVal(prmHandle, prmval);
}

/*!****************************************************************************
 * @brief
 * @retval enNoLim, enLimDown, enLimUp
 */
prmEditorStatus_type prmEditorAdd(const prmHandle_type *prmHandle, const prmval_type *prmval, int32_t multiply){
	int32_t s32t;
	int64_t s64t;
	float	ftemp;

	switch(prmHandle->type){
		case u8Frmt:
			s32t = prmHandle->prm->t_u8Frmt + (prmval->t_u8Frmt * multiply);
			if(s32t > prmHandle->max->t_u8Frmt){
				prmHandle->prm->t_u8Frmt = prmHandle->max->t_u8Frmt;
				return enLimUp;
			}else if(s32t < prmHandle->min->t_u8Frmt){
				prmHandle->prm->t_u8Frmt = prmHandle->min->t_u8Frmt;
				return enLimDown;
			}else{
				prmHandle->prm->t_u8Frmt = s32t;
			}
			break;

		case s8Frmt:
			s32t = prmHandle->prm->t_s8Frmt + (prmval->t_s8Frmt * multiply);
			if(s32t > prmHandle->max->t_s8Frmt){
				prmHandle->prm->t_s8Frmt = prmHandle->max->t_s8Frmt;
				return enLimUp;
			}else if(s32t < prmHandle->min->t_s8Frmt){
				prmHandle->prm->t_s8Frmt = prmHandle->min->t_s8Frmt;
				return enLimDown;
			}else{
				prmHandle->prm->t_s8Frmt = s32t;
			}
			break;

		case u16Frmt:
			s32t = prmHandle->prm->t_u16Frmt + (prmval->t_u16Frmt * multiply);
			if(s32t > prmHandle->max->t_u16Frmt){
				prmHandle->prm->t_u16Frmt = prmHandle->max->t_u16Frmt;
				return enLimUp;
			}else if(s32t < prmHandle->min->t_u16Frmt){
				prmHandle->prm->t_u16Frmt = prmHandle->min->t_u16Frmt;
				return enLimDown;
			}else{
				prmHandle->prm->t_u16Frmt = s32t;
			}
			break;

		case s16Frmt:
			s32t = prmHandle->prm->t_s16Frmt + (prmval->t_s16Frmt * multiply);
			if(s32t > prmHandle->max->t_s16Frmt){
				prmHandle->prm->t_s16Frmt = prmHandle->max->t_s16Frmt;
				return enLimUp;
			}else if(s32t < prmHandle->min->t_s16Frmt){
				prmHandle->prm->t_s16Frmt = prmHandle->min->t_s16Frmt;
				return enLimDown;
			}else{
				prmHandle->prm->t_s16Frmt = s32t;
			}
			break;

		case u32Frmt:
		case ipAdrFrmt:
		case unixTimeFrmt:
		case unixDateFrmt:
			s64t = prmHandle->prm->t_u32Frmt + ((int64_t) prmval->t_u32Frmt * multiply);
			if(s64t > prmHandle->max->t_u32Frmt){
				prmHandle->prm->t_u32Frmt = prmHandle->max->t_u32Frmt;
				return enLimUp;
			}else if(s64t < prmHandle->min->t_u32Frmt){
				prmHandle->prm->t_u32Frmt = prmHandle->min->t_u32Frmt;
				return enLimDown;
			}else{
				prmHandle->prm->t_u32Frmt = s64t;
			}
			break;

		case s32Frmt:
			s64t = prmHandle->prm->t_s32Frmt + ((int64_t) prmval->t_s32Frmt * multiply);
			if(s64t > prmHandle->max->t_s32Frmt){
				prmHandle->prm->t_s32Frmt = prmHandle->max->t_s32Frmt;
				return enLimUp;
			}else if(s64t < prmHandle->min->t_s32Frmt){
				prmHandle->prm->t_s32Frmt = prmHandle->min->t_s32Frmt;
				return enLimDown;
			}else{
				prmHandle->prm->t_s32Frmt = s64t;
			}
			break;

		case floatFrmt:
			ftemp = prmHandle->prm->t_floatFrmt + (prmval->t_floatFrmt * multiply);
			if(ftemp > prmHandle->max->t_floatFrmt){
				prmHandle->prm->t_floatFrmt = prmHandle->max->t_floatFrmt;
				return enLimUp;
			}else if(ftemp < prmHandle->min->t_floatFrmt){
				prmHandle->prm->t_floatFrmt = prmHandle->min->t_floatFrmt;
				return enLimDown;
			}else{
				prmHandle->prm->t_floatFrmt = ftemp;
			}
			break;
		default:
			break;
	}

	return enNoLim;
}

/*!****************************************************************************
 * @brief
 * @retval enNoLim, enLimDown, enLimUp
 */
prmEditorStatus_type prmEditorBigStepUp(const prmHandle_type *prmHandle){
	return prmEditorAdd(prmHandle, &prmHandle->bigstep, 1);
}

/*!****************************************************************************
 * @brief
 * @retval enNoLim, enLimDown, enLimUp
 */
prmEditorStatus_type prmEditorBigStepDown(const prmHandle_type *prmHandle){
	return prmEditorAdd(prmHandle, &prmHandle->bigstep, -1);
}

/*!****************************************************************************
 * @brief  update value
 * @retval enNoCharge, enCharge, enLimDown, enLimUp, enTransitionDown, enTransitionUp
 */
prmEditorStatus_type prmEditorUpDate(const prmHandle_type *prmHandle){
	static uint16_t ntic = 0;
	prmEditorStatus_type status = enNoCharge;

	if(ntic >= prmEditorNtic){
		int32_t step = enco_read();

		//Accelerator
		if((step > 2) || (step < -2)){
			step = step * 5;
		}

		status = prmEditorAdd(prmHandle, prmHandle->step, step);

		ntic = 0;
	}else{
		ntic++;
	}

	return status;
}

/*!****************************************************************************
 */
void prmEditorSetNtic(uint16_t n){
	prmEditorNtic = n;
}

/******************************** END OF FILE ********************************/

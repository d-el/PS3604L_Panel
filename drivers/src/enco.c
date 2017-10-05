/*!****************************************************************************
 * @file    enco.c
 * @author  Storozhenko Roman - D_EL
 * @version V2.3
 * @date    13.05.2015
 * @date    14.11.2016   fix "temp > enco.top"
 * @brief   encoder driver, stm32f1
 * @copyright GNU Public License
 */

/*!****************************************************************************
 * Include
 */
#include "enco.h"

/******************************************************************************
 * MEMORY
 */
enco_type enco;
prmHandle_type *encoLink;

/*!****************************************************************************
 * @brief  Настройка периферии МК для работы с инкрементальным энкодером
 */
void enco_init(void){
	gppin_init(GPIOD, 12, alternateFunctionPushPull, pullUp, 0, 2);  				//CH1
	gppin_init(GPIOD, 13, alternateFunctionPushPull, pullUp, 0, 2);  				//CH2

	//Настройка таймера
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;                       	//Clock enable

	TIM4->CCMR1 = TIM_CCMR1_CC2S_0 | TIM_CCMR1_CC1S_0;         	//Capture/Compare  selection T2, T1
	TIM4->CCMR1 |= (TIM_CCMR1_IC2PSC_1 | TIM_CCMR1_IC1PSC_1); 	//Capture is done once every 4 events
	TIM4->CCMR1 |= TIM_CCMR1_IC1F | TIM_CCMR1_IC2F;         	//Input capture 1 filter fSAMPLING=fDTS/32, N=8

	TIM4->CCER |= TIM_CCER_CC1P;								//Capture/Compare output polarity - inverted/falling edge
	TIM4->CCER |= TIM_CCER_CC2P;								//Capture/Compare output polarity - inverted/falling edge

	TIM4->CCER |= TIM_CCER_CC1E;								//Capture/Compare 1 output enable
	TIM4->CCER |= TIM_CCER_CC2E;								//Capture/Compare 2 output enable

	TIM4->SMCR |= TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1;				//Encoder mode 3 - Counter counts up/down on both TI1FP1 and TI2FP2 edges

	TIM4->ARR = 0xFFFF;                                       	//Значение, до которого считает
	TIM4->SMCR |= TIM_SMCR_ETPS_1;								//ETRP frequency divided by 4
	TIM4->CNT = 0xFFFF / 4;
	TIM4->CR1 |= TIM_CR1_CEN;                                  	//Timer enable
}

/*!****************************************************************************
 * @brief
 */
uint16_t enGeReg(void){
	return TIM4->CNT;
}

/*!****************************************************************************
 * @brief
 */
void enSetReg(uint16_t val){
	TIM4->CNT = val;
}

/*!****************************************************************************
 * @brief
 */
void enWriteVal(const prmHandle_type *prmHandle, const prmval_type *prmval){
	prm_setVal(prmHandle, prmval);
}

/*!****************************************************************************
 * @brief
 * @retval enNoLim, enLimDown, enLimUp
 */
enStatus_type enAdd(const prmHandle_type *prmHandle, const prmval_type *prmval, int32_t multiply){
	int32_t s32t;
	int64_t s64t;
	float 	ftemp;
	extern uint32_t ipMul;

	switch(prmHandle->type){
		case frmt_u8:
			s32t = prmHandle->prm->_u8Frmt + (prmval->_u8Frmt * multiply);
			if(s32t > prmHandle->max._u8Frmt){
				prmHandle->prm->_u8Frmt = prmHandle->max._u8Frmt;
				return enLimUp;
			}else if(s32t < prmHandle->min._u8Frmt){
				prmHandle->prm->_u8Frmt = prmHandle->min._u8Frmt;
				return enLimDown;
			}else{
				prmHandle->prm->_u8Frmt = s32t;
			}
			break;

		case s8Frmt:
			s32t = prmHandle->prm->_s8Frmt + (prmval->_s8Frmt * multiply);
			if(s32t > prmHandle->max._s8Frmt){
				prmHandle->prm->_s8Frmt = prmHandle->max._s8Frmt;
				return enLimUp;
			}else if(s32t < prmHandle->min._s8Frmt){
				prmHandle->prm->_s8Frmt = prmHandle->min._s8Frmt;
				return enLimDown;
			}else{
				prmHandle->prm->_s8Frmt = s32t;
			}
			break;

		case u16Frmt:
			s32t = prmHandle->prm->_u16Frmt + (prmval->_u16Frmt * multiply);
			if(s32t > prmHandle->max._u16Frmt){
				prmHandle->prm->_u16Frmt = prmHandle->max._u16Frmt;
				return enLimUp;
			}else if(s32t < prmHandle->min._u16Frmt){
				prmHandle->prm->_u16Frmt = prmHandle->min._u16Frmt;
				return enLimDown;
			}else{
				prmHandle->prm->_u16Frmt = s32t;
			}
			break;

		case s16Frmt:
			s32t = prmHandle->prm->_s16Frmt + (prmval->_s16Frmt * multiply);
			if(s32t > prmHandle->max._s16Frmt){
				prmHandle->prm->_s16Frmt = prmHandle->max._s16Frmt;
				return enLimUp;
			}else if(s32t < prmHandle->min._s16Frmt){
				prmHandle->prm->_s16Frmt = prmHandle->min._s16Frmt;
				return enLimDown;
			}else{
				prmHandle->prm->_s16Frmt = s32t;
			}
			break;

		case u32Frmt:
			s64t = prmHandle->prm->_u32Frmt + ((int64_t) prmval->_u32Frmt * multiply);
			if(s64t > prmHandle->max._u32Frmt){
				prmHandle->prm->_u32Frmt = prmHandle->max._u32Frmt;
				return enLimUp;
			}else if(s64t < prmHandle->min._u32Frmt){
				prmHandle->prm->_u32Frmt = prmHandle->min._u32Frmt;
				return enLimDown;
			}else{
				prmHandle->prm->_u32Frmt = s64t;
			}
			break;

		case s32Frmt:
			s64t = prmHandle->prm->_s32Frmt + ((int64_t) prmval->_s32Frmt * multiply);
			if(s64t > prmHandle->max._s32Frmt){
				prmHandle->prm->_s32Frmt = prmHandle->max._s32Frmt;
				return enLimUp;
			}else if(s64t < prmHandle->min._s32Frmt){
				prmHandle->prm->_s32Frmt = prmHandle->min._s32Frmt;
				return enLimDown;
			}else{
				prmHandle->prm->_s32Frmt = s64t;
			}
			break;

		case floatFrmt:
			ftemp = prmHandle->prm->_floatFrmt + (prmval->_floatFrmt * multiply);
			if(ftemp > prmHandle->max._floatFrmt){
				prmHandle->prm->_floatFrmt = prmHandle->max._floatFrmt;
				return enLimUp;
			}else if(ftemp < prmHandle->min._floatFrmt){
				prmHandle->prm->_floatFrmt = prmHandle->min._floatFrmt;
				return enLimDown;
			}else{
				prmHandle->prm->_floatFrmt = ftemp;
			}
			break;

		case ipAdrFrmt:
			s64t = prmHandle->prm->_ipAdrFrmt + ((int64_t) prmval->_ipAdrFrmt * multiply);
			if(s64t > prmHandle->max._ipAdrFrmt){
				prmHandle->prm->_ipAdrFrmt = prmHandle->max._ipAdrFrmt;
				return enLimUp;
			}else if(s64t < prmHandle->min._u32Frmt){
				prmHandle->prm->_ipAdrFrmt = prmHandle->min._ipAdrFrmt;
				return enLimDown;
			}else{
				prmHandle->prm->_ipAdrFrmt = s64t;
			}
			break;
	}

	return enNoLim;
}

/*!****************************************************************************
 * @brief
 * @retval enNoLim, enLimDown, enLimUp
 */
enStatus_type enBigStepUp(const prmHandle_type *prmHandle){
	return enAdd(prmHandle, &prmHandle->bigstep, 1);
}

/*!****************************************************************************
 * @brief
 * @retval enNoLim, enLimDown, enLimUp
 */
enStatus_type enBigStepDown(const prmHandle_type *prmHandle){
	return enAdd(prmHandle, &prmHandle->bigstep, -1);
}

/*!****************************************************************************
 * @brief  Прочитать значние
 * @retval enNoCharge, enCharge, enLimDown, enLimUp, enTransitionDown, enTransitionUp
 */
enStatus_type enUpDate(const prmHandle_type *prmHandle){
	int32_t step = 0;
	static uint16_t ntic = 0;
	enStatus_type status = enNoCharge;

	if(ntic >= enco.ntic){
		//Вынимаем с регистра значение
		step = enGeReg() - 0xFFFF / 4;
		enSetReg(0xFFFF / 4);
		//Фильтруем
		if(step > 0){
			step = (step + 2) / 4;
		}else if(step < 0){
			step = (step - 2) / 4;
		}else if(step == 0){
			return enNoCharge;
		}

		//Ускоритель
		if((step > 2) || (step < -2)){
			step = step * 5;
		}

		status = enAdd(prmHandle, prmHandle->pstep, step);

		ntic = 0;
	}else{
		ntic++;
	}

	return status;
}

/*!****************************************************************************
 * @brief  Прочитать значние
 * @retval enNoCharge, enCharge, enLimDown, enLimUp, enTransitionDown, enTransitionUp
 */
void enSetNtic(uint16_t n){
	enco.ntic = n;
}

/*************** GNU GPL ************** END OF FILE ********* D_EL ***********/

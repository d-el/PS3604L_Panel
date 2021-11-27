/*!****************************************************************************
 * @file		enco.c
 * @author		d_el
 * @version		V2.4
 * @date		16.12.2018
 * @copyright	The MIT License (MIT). Copyright (c) 2018 Storozhenko Roman
 * @brief		encoder driver
 */

/*!****************************************************************************
 * Include
 */
#include "stm32f4xx.h"
#include "gpio.h"
#include "enco.h"

/******************************************************************************
 * MEMORY
 */
static uint16_t enco_tic;

/*!****************************************************************************
 * @brief  Initialize peripheral for incremental encoder
 */
void enco_init(void){
	gppin_init(GPIOD, 12, alternateFunctionPushPull, pullUp, 0, 2);	//CH1
	gppin_init(GPIOD, 13, alternateFunctionPushPull, pullUp, 0, 2);	//CH2

	//Timer init
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;							//Clock enable
	RCC->APB1RSTR |= RCC_APB1RSTR_TIM4RST;
	RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM4RST;

	TIM4->CCMR1 = TIM_CCMR1_CC2S_0 | TIM_CCMR1_CC1S_0;			//Capture/Compare  selection T2, T1
	TIM4->CCMR1 |= TIM_CCMR1_IC2PSC_1 | TIM_CCMR1_IC1PSC_1;		//Capture is done once every 4 events
	TIM4->CCMR1 |= TIM_CCMR1_IC1F | TIM_CCMR1_IC2F;				//Input capture 1 and 2 filter fSAMPLING=fDTS/32, N=8

	TIM4->CCER |= TIM_CCER_CC1P;								//Capture/Compare output polarity - inverted/falling edge
	TIM4->CCER |= TIM_CCER_CC2P;								//Capture/Compare output polarity - inverted/falling edge

	TIM4->CCER |= TIM_CCER_CC1E;								//Capture/Compare 1 output enable
	TIM4->CCER |= TIM_CCER_CC2E;								//Capture/Compare 2 output enable

	TIM4->SMCR |= TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1;				//Encoder mode 3 - Counter counts up/down on both TI1FP1 and TI2FP2 edges

	TIM4->ARR = 0xFFFF;											//Top value
	TIM4->SMCR |= TIM_SMCR_ETPS_1;								//ETRP frequency divided by 4
	TIM4->CNT = 0xFFFF / 4;
	TIM4->CR1 |= TIM_CR1_CEN;									//Timer enable
}

/*!****************************************************************************
 * @brief
 */
int16_t enco_read(void){
	//Get value from register
	int16_t step = TIM4->CNT - 0xFFFF / 4;
	TIM4->CNT = 0xFFFF / 4;

	//Filterer
	if(step > 0){
		step = (step + 2) / 4;
	}else if(step < 0){
		step = (step - 2) / 4;
	}

	return step;
}

/*!****************************************************************************
 */
int32_t enco_update(void){
	static uint16_t ntic = 0;
	if(ntic >= enco_tic){
		int32_t step = enco_read();
		if((step > 2) || (step < -2)){
			step = step * 5; //Accelerator
		}
		ntic = 0;
		return step;
	}else{
		ntic++;
	}
	return 0;
}

/*!****************************************************************************
 */
void enco_settic(uint16_t n){
	TIM4->CNT = 0xFFFF / 4;
	enco_tic = n;
}

/******************************** END OF FILE ********************************/

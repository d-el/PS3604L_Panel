/*!****************************************************************************
 * @file		ledpwm.c
 * @author		d_el
 * @version		V1.0
 * @date		27.12.2015
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		pwm for lcd led
 */

/*!****************************************************************************
 * Include
 */
#include "gpio.h"
#include "ledpwm.h"

/*!****************************************************************************
 * MEMORY
 */

/*!****************************************************************************
 * @brief Initialize timer
 */
void ledPwm_init(void){
	//TIM8_CH2 - PC7
	gppin_init(GPIOA, 0, alternateFunctionPushPull, pullDisable, 0, 2);

	RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;						//Clock Enable
	RCC->APB1RSTR |= RCC_APB1RSTR_TIM5RST;					//Reset
	RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM5RST;

	TIM5->PSC	= 1 - 1;									//Set prescaler
	TIM5->CCER	|= TIM_CCER_CC1E;							//Chennel enable
	TIM5->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;		//PWM mode
	TIM5->ARR	= 1000;										//Auto-reload value
	TIM5->CCR1	= 0;										//Compare value
	TIM5->CR1	|= TIM_CR1_ARPE;							//TIMx_ARR register is buffered
	TIM5->BDTR	|= TIM_BDTR_MOE;							//Main output enable
	TIM5->CR1	|= TIM_CR1_CEN;								//Count enable
}

/******************************** END OF FILE ********************************/

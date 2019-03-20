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
	gppin_init(GPIOC, 7, alternateFunctionPushPull, pullDisable, 0, 3);

	RCC->APB2ENR |= RCC_APB2ENR_TIM8EN;						//Clock Enable
	RCC->APB2RSTR |= RCC_APB2RSTR_TIM8RST;					//Reset
	RCC->APB2RSTR &= ~RCC_APB2RSTR_TIM8RST;

	TIM8->PSC	= 1 - 1;									//Set prescaler
	TIM8->CCER	|= TIM_CCER_CC2E;							//Chennel enable
	TIM8->CCMR1 |= TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1;		//PWM mode
	TIM8->ARR	= 1000;										//Auto-reload value
	TIM8->CCR2	= 0;										//Compare value
	TIM8->CR1	|= TIM_CR1_ARPE;							//TIMx_ARR register is buffered
	TIM8->BDTR	|= TIM_BDTR_MOE;							//Main output enable
	TIM8->CR1	|= TIM_CR1_CEN;								//Count enable
}

/*!****************************************************************************
 *
 */
void setLcdBrightness(uint16_t percent){
	TIM8->CCR2	= percent;
}

/******************************** END OF FILE ********************************/

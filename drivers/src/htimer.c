/*!****************************************************************************
 * @file		htimer.c
 * @author		d_el
 * @version		V1.0
 * @date		Jul 16, 2025
 * @copyright	License (MIT). Copyright (c) 2025 Storozhenko Roman
 * @brief
 */

/*!****************************************************************************
 * Include
 */
#include <stm32f4xx.h>
#include "board.h"

/*!****************************************************************************
 * @brief
 */
void htimer_init(void){
	RCC->APB2ENR |= RCC_APB2ENR_TIM9EN;						// Clock Enable
	RCC->APB2RSTR |= RCC_APB2RSTR_TIM9RST;					// Reset
	RCC->APB2RSTR &= ~RCC_APB2RSTR_TIM9RST;

	TIM9->PSC	= APB2_TIM_FREQ / 1000000 - 1;				// Set prescaler
	TIM9->ARR	= 65535;									// Auto-reload value
	TIM9->CR1	|= TIM_CR1_ARPE;							// TIMx_ARR register is buffered
	TIM9->CR1	|= TIM_CR1_CEN;								// Count enable
}

/*!****************************************************************************
 * @brief
 */
void htimer_delay(uint16_t us){
	uint16_t val = TIM9->CNT + us;
	while((val - TIM9->CNT) > 0)
	{}
}

/******************************** END OF FILE ********************************/

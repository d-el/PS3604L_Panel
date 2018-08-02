/*!****************************************************************************
 * @file		beep.c
 * @author		d_el
 * @version		V1.0
 * @date		19.12.2014
 * @copyright	GNU Lesser General Public License v3
 * @brief		Driver beep
 */

/*!****************************************************************************
 * Include
 */
#include "stm32f4xx.h"
#include "gpio.h"
#include "board.h"
#include "beep.h"

/*!****************************************************************************
 * @brief
 */
void TIM7_IRQHandler(void){
	if((TIME_B_TIM->SR & TIM_SR_UIF) != 0){
		TIME_B_TIM->ARR = 0;
		BEEP_TIM->CR1 &= ~TIM_CR1_CEN;
		TIME_B_TIM->SR = 0/*~TIM_SR_UIF*/;							//Clear UIF flag
	}
}

/*!****************************************************************************
 * @brief
 * @param time - duration time (1 - 32767) [ms]
 *		  freq - frequency (1 - 32767) [Hz]
 */
void BeepTime(uint16_t time, uint16_t freq){
	if((TIME_B_TIM->ARR - TIME_B_TIM->CNT) < time){
		TIME_B_TIM->CNT = 0;
		BEEP_TIM->ARR = APB2_TIM_FREQ / (freq) / 2;
		TIME_B_TIM->ARR = time * 2;
		BEEP_TIM->CR1 |= TIM_CR1_CEN;
		TIME_B_TIM->CR1 |= TIM_CR1_CEN;								//Timer enable
	}
}

/*!****************************************************************************
 * @brief
 */
void beep_init(void){
	// TIM12_CH1 - PB14
	gppin_init(GPIOB, 14, alternateFunctionPushPull, pullDisable, 0, 9);
	// TIM12_CH2 - PB15
	gppin_init(GPIOB, 15, alternateFunctionPushPull, pullDisable, 0, 9);

	/*!***********************
	 * PWM timer
	 */
	RCC->APB1ENR	|= RCC_APB1ENR_TIM12EN;						//Clock enable
	RCC->APB1RSTR	|= RCC_APB1RSTR_TIM12RST;					//Reset
	RCC->APB1RSTR	&= ~RCC_APB1RSTR_TIM12RST;

	BEEP_TIM->PSC	= 1 - 1;									//Prescaler
	BEEP_TIM->CR1	|= TIM_CR1_ARPE;							//TIMx_ARR register is buffered
	BEEP_TIM->CCER	|= TIM_CCER_CC1E;							//Enable CH1
	BEEP_TIM->CCMR1	|= (TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0);	//Toggle - OC1REF toggles when TIMx_CNT=TIMx_CCR1

	BEEP_TIM->CCER	|= TIM_CCER_CC2E;							//Enable CH2
	BEEP_TIM->CCER	|= TIM_CCER_CC2P;							//CH2 Polarity
	BEEP_TIM->CCMR1	|= (TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_0);	//Toggle - OC1REF toggles when TIMx_CNT=TIMx_CCR1

	BEEP_TIM->BDTR	|= TIM_BDTR_MOE;							//Main output enable
	BEEP_TIM->ARR	= APB1_TIM_FREQ / F_BEEP / 2;				//Auto-reload value

	/*!***********************
	 * Time timer
	 */
	RCC->APB1ENR	|= RCC_APB1ENR_TIM7EN;						//Clock enable
	RCC->APB1RSTR	|= RCC_APB1RSTR_TIM7RST;					//Reset
	RCC->APB1RSTR	&= ~RCC_APB1RSTR_TIM7RST;

	TIME_B_TIM->PSC = APB1_TIM_FREQ / 2 / 1000 - 1;				//Prescaler - 1 cycle = 1/2 ms
	TIME_B_TIM->ARR = 0;										//Auto-reload value
	TIME_B_TIM->CR1 |= TIM_CR1_OPM;								//One pulse mode
	TIME_B_TIM->CR1 |= TIM_CR1_CKD_1;							//Clock division tDTS=4*tCK_INT
	TIME_B_TIM->DIER |= TIM_DIER_UIE;							//Update interrupt enable
	TIME_B_TIM->EGR = TIM_EGR_UG;								//Update event generate
	TIME_B_TIM->SR	= ~TIM_SR_UIF;								//Clear UIF flag

	#define TIM_BEEP_InterruptPrior	15
	NVIC_EnableIRQ(TIM7_IRQn);
	NVIC_SetPriority(TIM7_IRQn, TIM_BEEP_InterruptPrior);
}

/*************** LGPL ************** END OF FILE *********** D_EL ************/

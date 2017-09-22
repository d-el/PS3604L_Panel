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
#include "beep.h"

/*!****************************************************************************
 * @brief
 */
void TIM6_DAC_IRQHandler(void){
	/*if((TIME_B_TIM->SR & TIM_SR_UIF) != 0){
		TIME_B_TIM->ARR = 0;
		beepOFF();
		TIME_B_TIM->SR = ~TIM_SR_UIF;							//Clear UIF flag
	}*/
}

/*!****************************************************************************
 * @brief
 * @param time - duration time (1 - 32767) [ms]
 *        freq - frequency (1 - 32767) [Hz]
 */
void BeepTime(uint16_t time, uint16_t freq){
	/*beepON();
	if((TIME_B_TIM->ARR - TIME_B_TIM->CNT) < time){
		TIME_B_TIM->CNT = 0;
		BEEP_TIM->ARR = SYSTEM_FREQ / (freq) / 2;
		TIME_B_TIM->ARR = time * 2;
	}
	TIME_B_TIM->CR1 |= TIM_CR1_CEN;								//Timer enable*/
}

/*!****************************************************************************
 * @brief
 */
void beep_init(void){
//	/*//TIM15_CH2 - PB15
//	gppin_init(GPIOB, 15, alternateFunctionPushPull, pullDisable, 0, 14);
//
//	/*!***********************
//	 * PWM timer
//	 */
//	RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;						//Clock enable
//	RCC->APB2RSTR |= RCC_APB2RSTR_TIM15RST;						//Reset
//	RCC->APB2RSTR &= ~RCC_APB2RSTR_TIM15RST;
//
//	BEEP_TIM->PSC = 1 - 1;										//Prescaler
//	BEEP_TIM->CR1 |= TIM_CR1_ARPE;                            	//TIMx_ARR register is buffered
//	BEEP_TIM->CCER |= TIM_CCER_CC2E;                           	//Enabke CH2
//	BEEP_TIM->CCMR1 |= (TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_0);   //Toggle - OC1REF toggles when TIMx_CNT=TIMx_CCR1
//	BEEP_TIM->BDTR |= TIM_BDTR_MOE;                            	//Main output enable
//	BEEP_TIM->ARR = SYSTEM_FREQ / F_BEEP / 2;            		//Auto-reload value
//
//	/*!***********************
//	 * Time timer
//	 */
//	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM6EN;						//Clock enable
//	RCC->APB1RSTR1 |= RCC_APB1RSTR1_TIM6RST;					//Reset
//	RCC->APB1RSTR1 &= ~RCC_APB1RSTR1_TIM6RST;
//
//	TIME_B_TIM->PSC = SYSTEM_FREQ / 2 / 1000 - 1;            	//Prescaler - 1 cycle = 1/2 ms
//	TIME_B_TIM->ARR = 0;                                        //Auto-reload value
//	TIME_B_TIM->CR1 |= TIM_CR1_OPM;                             //One pulse mode
//	TIME_B_TIM->CR1 |= TIM_CR1_CKD_1;							//Clock division tDTS=4*tCK_INT
//	TIME_B_TIM->DIER |= TIM_DIER_UIE;                           //Update interrupt enable
//	TIME_B_TIM->EGR = TIM_EGR_UG;								//Update event generate
//	TIME_B_TIM->SR = ~TIM_SR_UIF;								//Clear UIF flag
//	NVIC_EnableIRQ(TIM6_DAC_IRQn);
//	NVIC_SetPriority(TIM6_DAC_IRQn, 15);
}

/*************** LGPL ************** END OF FILE *********** D_EL ************/

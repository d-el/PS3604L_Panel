/*!****************************************************************************
 * @file		pvd.h
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		01.08.2016
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		Driver power voltage detector
 */

/*!****************************************************************************
* Include
*/
#include "stm32f4xx.h"
#include "stddef.h"
#include "pvd.h"

/*!****************************************************************************
* MEMORY
*/
static suplyFaultCallBack_type suplyFaultCallBack;

/*!****************************************************************************
* @brief	PVD initialization
*/
void pvd_init(void){
	RCC->APB1ENR	|= RCC_APB1ENR_PWREN;				//Power interface clock enable
	PWR->CR			&= ~PWR_CR_PVDE;					//Power voltage detector disable
	PWR->CR			&= ~PWR_CR_PLS;
	PWR->CR			|= PWR_CR_PLS_LEV7;					//Power voltage detector level selection VPVD6 around 2.9 V
	PWR->CR			|= PWR_CR_PVDE;						//Power voltage detector enable				//Restore value

	EXTI->IMR		|= EXTI_IMR_IM16;					//Interrupt request from Line 16 is not masked
	EXTI->EMR		|= EXTI_EMR_EM16;					//Event request from Line 16 is not masked
	EXTI->RTSR		|= EXTI_RTSR_TR16;					//Rising trigger event configuration bit of line 16
	EXTI->PR		|= EXTI_PR_PR16;					//Pending

	NVIC_SetPriority(PVD_IRQn, PVD_IRQ_Priority);
	NVIC_EnableIRQ(PVD_IRQn);							//PVD through EXTI Line detection Interrupt
}

/*!****************************************************************************
* @brief	PVD disable
*/
void pvd_disable(void){
	PWR->CR			&= ~PWR_CR_PVDE;					//Power voltage detector disable
	EXTI->IMR		&= ~EXTI_IMR_IM16;					//Interrupt request from Line 16 is masked
	EXTI->EMR		&= ~EXTI_EMR_EM16;					//Event request from Line 16 is masked
	EXTI->RTSR		&= ~EXTI_RTSR_TR16;					//Rising trigger event configuration bit of line 16
	NVIC_DisableIRQ(PVD_IRQn);
}

/*!****************************************************************************
 *
 */
void pvd_setSupplyFaultCallBack(suplyFaultCallBack_type callBack){
	suplyFaultCallBack = callBack;
}

/*!****************************************************************************
* @brief	PVD IRQ handler
*/
void PVD_IRQHandler(void){
	if(suplyFaultCallBack != NULL){
		suplyFaultCallBack();
	}
	EXTI->PR = EXTI_PR_PR16;	//Pending bit
}

/******************************** END OF FILE ********************************/

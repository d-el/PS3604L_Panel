/*!****************************************************************************
 * @file		rng.c
 * @author		d_el
 * @version		V1.0
 * @date		24.04.2018
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		Random number generator driver
 */

/*!****************************************************************************
 * Include
 */
#include "stm32f4xx.h"

/*!****************************************************************************
 * MEMORY
 */

/*!****************************************************************************
 * @brief
 */
void rng_init(void){
	RCC->AHB2ENR |= RCC_AHB2ENR_RNGEN;		//Clock enable
	RCC->AHB2RSTR |= RCC_AHB2RSTR_RNGRST;	//Reset
	RCC->AHB2RSTR &= ~RCC_AHB2RSTR_RNGRST;
	RNG->CR |= RNG_CR_RNGEN; 				//Random number generator enable
}

/*!****************************************************************************
 * @brief
 */
int32_t rng_get(void){
	return RNG->DR;
}

/******************************** END OF FILE ********************************/

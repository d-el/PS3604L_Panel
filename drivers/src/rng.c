/*!****************************************************************************
 * @file		rng.c
 * @author		d_el
 * @version		V1.0
 * @date		24.04.2018
 * @brief		Random number generator driver
 * @copyright	Copyright (C) 2017 Storozhenko Roman
 *				All rights reserved
 *				This software may be modified and distributed under the terms
 *				of the BSD license.	 See the LICENSE file for details
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

/***************** Copyright (C) Storozhenko Roman ******* END OF FILE *******/

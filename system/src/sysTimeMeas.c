/*!****************************************************************************
 * @file		sysTimeMeas.c
 * @author		d_el
 * @version		V1.0
 * @date		01.08.2017
 * @brief		Time meter on DWT cycle counter
 * @copyright	Copyright (C) 2017 Storozhenko Roman
 *				All rights reserved
 *				This software may be modified and distributed under the terms
 *				of the BSD license.	 See the LICENSE file for details
 */

/*!****************************************************************************
 * Include
 */
#include "sysTimeMeas.h"

/*!****************************************************************************
 * MEMORY
 */
sysTimeStruct_type sysTimeStruct;

/*!****************************************************************************
 * @brief		Enable DWT cycle counter
 */
void sysTimeMeasEnable(void){
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;	//Enable the CYCCNT
	DWT->CYCCNT = 0;
}

/*!****************************************************************************
 * @brief		Disable DWT cycle counter
 */
void sysTimeMeasDisable(void){
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;	//Enable the CYCCNT
}

/*!****************************************************************************
 * @brief		Get time in cycles
 * @param[in] 	n - number measurement
 * @return		number cycles
 */
uint32_t sysTimeMeasGet_cycles(sysTimeNumber_type n){
	uint32_t delta = sysTimeStruct.stop[n] - sysTimeStruct.start[n];
	return delta;
}

/*!****************************************************************************
 * @brief		Convert cycles to ms
 * @param[in] 	cycles - time in counter cycles
 * @return		time in milliseconds
 */
uint64_t sysTimeMeasTo_ms(uint64_t cycles){
	return  (cycles * 1000ULL) / CORE_FREQUENCY;
}

/*!****************************************************************************
 * @brief		Convert cycles to us
 * @param[in] 	cycles - time in counter cycles
 * @return		time in microseconds
 */
uint64_t sysTimeMeasTo_us(uint64_t cycles){
	return  (cycles * 1000000ULL) / CORE_FREQUENCY;
}

/*************** LGPL ************** END OF FILE *********** D_EL ************/

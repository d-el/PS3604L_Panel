/*!****************************************************************************
 * @file		sysTimeMeas.c
 * @author		d_el
 * @version		V1.0
 * @date		01.08.2017
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		Time meter on DWT cycle counter
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
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
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
	return  (cycles * 1000000) / CORE_FREQUENCY;
}

/******************************** END OF FILE ********************************/

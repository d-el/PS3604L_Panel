/*!****************************************************************************
 * @file		sysTimeMeas.c
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		01.08.2017
 * @copyright	GNU Lesser General Public License v3
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
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;	//Enable the CYCCNT
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
 * @brief		Get time in ms
 * @param[in] 	n - number measurement
 * @return		time in milliseconds
 */
uint32_t sysTimeMeasGet_ms(sysTimeNumber_type n){
	uint32_t delta = sysTimeStruct.stop[n] - sysTimeStruct.start[n];
	uint32_t time = (delta * 1000ULL) / CORE_FREQUENCY;
	return time;
}

/*!****************************************************************************
 * @brief		Get time in us
 * @param[in] 	n - number measurement
 * @return		time in microseconds
 */
uint64_t sysTimeMeasGet_us(sysTimeNumber_type n){
	uint32_t delta = sysTimeStruct.stop[n] - sysTimeStruct.start[n];
	uint64_t time = (delta * 1000000ULL) / CORE_FREQUENCY;
	return time;
}

/*!****************************************************************************
 * @brief		Get time in ns
 * @param[in] 	n - number measurement
 * @return		time in nanoseconds
 */
uint64_t sysTimeMeasGet_ns(sysTimeNumber_type n){
	uint32_t delta = sysTimeStruct.stop[n] - sysTimeStruct.start[n];
	uint64_t time = (delta * 1000000000ULL) / CORE_FREQUENCY;
	return time;
}

/*************** LGPL ************** END OF FILE *********** D_EL ************/

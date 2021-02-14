/*!****************************************************************************
 * @file		sysTimeMeas.h
 * @author		d_el
 * @version		V1.0
 * @date		01.08.2017
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		Time meter on DWT cycle counter
 */
#ifndef sysTimeMeas_H
#define sysTimeMeas_H

#ifdef __cplusplus
extern "C" {
#endif

/*!****************************************************************************
 * Include
 */
#include "stdint.h"
#include "stm32f4xx.h"

/*!****************************************************************************
 * Define
 */
#define NUMBER_MEASURES		(8)
#define CORE_FREQUENCY		(168000000)

/*!****************************************************************************
 * Enumeration
 */

/*!****************************************************************************
 * Typedef
 */
typedef enum {
	sysTimeBs = 0,
	sysTime1,
	sysTime2,
} sysTimeNumber_type;

typedef struct {
	uint32_t start[NUMBER_MEASURES];
	uint32_t stop[NUMBER_MEASURES];
	uint32_t timDelta[NUMBER_MEASURES];
} sysTimeStruct_type;

/*!****************************************************************************
 * Exported variables
 */
extern sysTimeStruct_type sysTimeStruct;

/*!****************************************************************************
 * Macro functions
 */
/*!****************************************************************************
 * @brief		Get counter value
 */
static inline uint32_t sysTimeMeasGetCnt(void){
	return DWT->CYCCNT;
}

/*!****************************************************************************
 * @brief		Start capture
 * @param[in] 	n - number measurement
 */
static inline void sysTimeMeasStart(sysTimeNumber_type n){
	sysTimeStruct.start[n] = sysTimeMeasGetCnt();
}

/*!****************************************************************************
 * @brief		Stop capture
 * @param[in] 	n - number measurement
 */
static inline void sysTimeMeasStop(sysTimeNumber_type n){
	sysTimeStruct.stop[n] = sysTimeMeasGetCnt();
}

/*!****************************************************************************
 * Function declaration
 */
void sysTimeMeasEnable(void);
void sysTimeMeasDisable(void);
uint32_t sysTimeMeasGet_cycles(sysTimeNumber_type n);
uint64_t sysTimeMeasTo_ms(uint64_t cycles);
uint64_t sysTimeMeasTo_us(uint64_t cycles);

#ifdef __cplusplus
}
#endif

#endif //sysTimeMeas_H
/******************************** END OF FILE ********************************/

/*!****************************************************************************
 * @file		sysTimeMeas.h
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		01.08.2017
 * @copyright	GNU Lesser General Public License v3
 * @brief		Time meter on DWT cycle counter
 */
#ifndef sysTimeMeas_H
#define sysTimeMeas_H

/*!****************************************************************************
 * Include
 */
#include "stm32f4xx.h"
#include "board.h"

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
	sysTime3,
	sysTime4,
	sysTime5,
	sysTime6,
	sysTime7,
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
 * @brief		Start capture
 * @param[in] 	n - number measurement
 */
static inline void sysTimeMeasStart(sysTimeNumber_type n){
	sysTimeStruct.start[n] = DWT->CYCCNT;
}

/*!****************************************************************************
 * @brief		Stop capture
 * @param[in] 	n - number measurement
 */
static inline void sysTimeMeasStop(sysTimeNumber_type n){
	sysTimeStruct.stop[n] = DWT->CYCCNT;
}

/*!****************************************************************************
 * Function declaration
 */
void sysTimeMeasEnable(void);
void sysTimeMeasDisable(void);
void sysTimeMeasStart(sysTimeNumber_type n);
void sysTimeMeasStop(sysTimeNumber_type n);
uint32_t sysTimeMeasGet_cycles(sysTimeNumber_type n);
uint32_t sysTimeMeasGet_ms(sysTimeNumber_type n);
uint64_t sysTimeMeasGet_us(sysTimeNumber_type n);
uint64_t sysTimeMeasGet_ns(sysTimeNumber_type n);

#endif //sysTimeMeas_H
/*************** LGPL ************** END OF FILE *********** D_EL ************/

/*!****************************************************************************
 * @file		base.h
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		01.01.2015
 * @copyright	The MIT License (MIT). Copyright (c) 2020 Storozhenko Roman
 * @brief		This task is base GUI
 */
#ifndef BASE_H
#define BASE_H

/*!****************************************************************************
 * Include
 */
#include <stdint.h>

/******************************************************************************
 * Define
 */
#define NPRESET             (3)
#define IDLE_TIME           (180000)       		//Время до заставки [ms]
#define BASE_TSK_PERIOD     (20)                //[ms]

/*!****************************************************************************
 * Enumeration
 */

/******************************************************************************
 * Typedef
 */
typedef enum {
	VAR_VOLT = 0,
	VAR_CURR,
	VAR_MODE,
} baseVar_type;

typedef enum {
	baseImax = 0,
	baseILimitation,
	baseUnprotected,
} baseMode_type;

typedef struct {
	uint16_t u;
	uint16_t i;
	/*baseMode_type*/uint8_t mode;
} baseSet_type;

typedef struct {
	baseSet_type set[NPRESET];
	uint8_t curPreSet;
} base_type;

/*!****************************************************************************
 * Exported variables
 */
extern base_type bs;

/******************************************************************************
 * Macro functions
 */

/*!****************************************************************************
 * Function declaration
 */
void baseTSK(void *pPrm);
void printStatusBar(void);

#endif //BASE_H
/******************************** END OF FILE ********************************/

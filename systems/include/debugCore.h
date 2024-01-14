/*!****************************************************************************
 * @file		debugCore.c
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		15.01.2016
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		Debug utils, HardFault_Handler tracer
 */
#ifndef debugCore_H
#define debugCore_H

/*!****************************************************************************
 * Include
 */
#include "stdint.h"

/*!****************************************************************************
 * Function declaration
 */
uint32_t coreIsInDebugMode(void);

#endif //debugCore_H
/******************************** END OF FILE ********************************/

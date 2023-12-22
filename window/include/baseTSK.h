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
#define BASE_TSK_PERIOD		(20) //[ms]

/*!****************************************************************************
 * Function declaration
 */
void baseTSK(void *pPrm);
void printFooter(void);

#endif //BASE_H
/******************************** END OF FILE ********************************/

/*!****************************************************************************
 * @file		debugCore.c
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		15.01.2016
 * @copyright	GNU Lesser General Public License v3
 * @brief		Debug utils, HardFault_Handler tracer
 */
#ifndef debugCore_H
#define debugCore_H

/*!****************************************************************************
 * Include
 */
#include "stm32f4xx.h"
#include "stdint.h"
#include "semihosting.h"

/*!****************************************************************************
 * Define
 */

/*!****************************************************************************
 * Enumeration
 */

/*!****************************************************************************
 * Typedef
 */

/*!****************************************************************************
 * Exported variables
 */
/*!****************************************************************************
 * Macro functions
 */

/*!****************************************************************************
 * Function declaration
 */
uint32_t coreIsInDebugMode(void);

#endif //debugCore_H
/*************** LGPL ************** END OF FILE *********** D_EL ************/

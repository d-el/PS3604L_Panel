/*!****************************************************************************
 * @file		st7735.h
 * @author		d_el
 * @version		V1.0
 * @date		05.07.2013
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 */
#ifndef DRIVERS_H
#define DRIVERS_H

#ifdef __cplusplus
extern "C" {
#endif

/*!****************************************************************************
 * Include
 */
#include "stdint.h"

/*!****************************************************************************
 * Exported variables
 */
extern uint8_t macAddress[6];

/*!****************************************************************************
 * Function declaration
 */
void hardInit(void);

#ifdef __cplusplus
}
#endif

#endif //DRIVERS_H
/******************************** END OF FILE ********************************/

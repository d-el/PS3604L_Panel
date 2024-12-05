/*!****************************************************************************
 * @file		uniqueDeviceId.h
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		26.06.2017
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 */
#ifndef uniqueDeviceId_H
#define uniqueDeviceId_H

#ifdef __cplusplus
extern "C" {
#endif

/*!****************************************************************************
 * Include
 */
#include "stdint.h"

/*!****************************************************************************
 * Function declaration
 */
uint32_t uid_get(void);

#ifdef __cplusplus
}
#endif

#endif //uniqueDeviceId_H
/******************************** END OF FILE ********************************/

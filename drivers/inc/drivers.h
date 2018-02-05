/*!****************************************************************************
 * @file		st7735.h
 * @author		d_el
 * @version		V1.0
 * @date		05.07.2013
 * @brief
 * @copyright	Copyright (C) 2013 Storozhenko Roman
 *				All rights reserved
 *				This software may be modified and distributed under the terms
 *				of the BSD license.	 See the LICENSE file for details
 */
#ifndef DRIVERS_H
#define DRIVERS_H

/*!****************************************************************************
 * Include
 */
#include "stdint.h"

/*!****************************************************************************
 * Define
 */

/*!****************************************************************************
 * Enumeration
 */

/*!****************************************************************************
 * Exported variables
 */
extern uint8_t macAddress[6];

/*!****************************************************************************
 * Macro functions
 */

/*!****************************************************************************
 * Function declaration
 */
void hardInit(void);

#endif //DRIVERS_H
/***************** Copyright (C) Storozhenko Roman ******* END OF FILE *******/

/*!****************************************************************************
 * @file		drivers.h
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		05-07-2013
 * @copyright	GNU Lesser General Public License v3
 * @brief		MCU peripheral initialization
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
/*************** LGPL ************** END OF FILE *********** D_EL ************/

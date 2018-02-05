/*!****************************************************************************
 * @file		clock.h
 * @author		d_el
 * @version		V1.0
 * @date		17.09.2017
 * @brief		MCU peripheral initialization
 * @copyright	Copyright (C) 2017 Storozhenko Roman
 *				All rights reserved
 *				This software may be modified and distributed under the terms
 *				of the BSD license.	 See the LICENSE file for details
 */
#ifndef clock_H
#define clock_H

/*!****************************************************************************
 * Include
 */

/*!****************************************************************************
 * Define
 */

/*!****************************************************************************
 * Enumeration
 */

/*!****************************************************************************
 * Typedef
 */
typedef enum {
	clockOk,
	clockErorHse
}clockState_type;

/*!****************************************************************************
 * Exported variables
 */

/*!****************************************************************************
 * Macro functions
 */

/*!****************************************************************************
 * Function declaration
 */
uint8_t clock_init(void);

#endif //clock_H
/***************** Copyright (C) Storozhenko Roman ******* END OF FILE *******/

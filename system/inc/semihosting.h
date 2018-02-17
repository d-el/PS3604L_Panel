/*!****************************************************************************
 * @file		semihosting.h
 * @author		d_el
 * @version		V1.0
 * @date		17.02.2018
 * @brief		Semihosting driver
 * @copyright	Copyright (C) 2017 Storozhenko Roman
 *				All rights reserved
 *				This software may be modified and distributed under the terms
 *				of the BSD license.	 See the LICENSE file for details
 */
#ifndef SEMIHOSTTING
#define SEMIHOSTTING

/*!****************************************************************************
 * Include
 */
#include "stdint.h"

/*!****************************************************************************
 * Function declaration
 */
void sh_sendString(const char *str);
char sh_getChar(void);


#endif //SEMIHOSTTING
/***************** Copyright (C) Storozhenko Roman ******* END OF FILE *******/

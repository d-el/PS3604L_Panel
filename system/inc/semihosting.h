/*!****************************************************************************
 * @file		semihosting.h
 * @author		d_el
 * @version		V1.0
 * @date		17.02.2018
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		Semihosting driver
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
uint32_t sh_getTime(void);

#endif //SEMIHOSTTING
/******************************** END OF FILE ********************************/

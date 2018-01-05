/*!****************************************************************************
 * @file		semihosting.h
 * @author		Coocox
 * @version		V1.0
 * @date		09.10.2011
 * @copyright	GNU Lesser General Public License v3
 * @brief		Semihosting LowLayer GetChar/SendChar Implement
 */
#ifndef __SIMIHOSTTING_IMPL
#define __SIMIHOSTTING_IMPL

#include "stdint.h"

/*!****************************************************************************
 * Function declaration
 */
void SH_SendString(const char *str);
char SH_GetChar(void);


#endif
/*************** LGPL ************** END OF FILE *********** D_EL ************/

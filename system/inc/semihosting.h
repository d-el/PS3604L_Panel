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
extern uint32_t SH_DoCommand(uint32_t n32In_R0, uint32_t n32In_R1, uint32_t *pn32Out_R0);
void SH_SendString(const char *str);
char SH_GetChar(void);


#endif
/*************** LGPL ************** END OF FILE *********** D_EL ************/

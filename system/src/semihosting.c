/*!****************************************************************************
 * @file		semihosting.c
 * @author		Coocox
 * @version		V1.0
 * @date		09.10.2011
 * @copyright	GNU Lesser General Public License v3
 * @brief		Semihosting LowLayer GetChar/SendChar Implement
 */

/*!****************************************************************************
 * Include
 */
#include "debugCore.h"

/*!****************************************************************************
 * Memory
 */

/*!****************************************************************************
 * Local prototypes for the functions
 */
extern uint32_t SH_DoCommand(uint32_t n32In_R0, uint32_t n32In_R1, uint32_t *pn32Out_R0);

/*!****************************************************************************
 * @brief  Transmit a null-terminated string on semihosting mode
 * @param  str is the string that to send
 * @return Character to write
 */
void SH_SendString(const char *str){
	if(coreIsInDebugMode() == 0){
		return;
	}
	if(SH_DoCommand(0x04, (uint32_t) str, 0) != 0){
		return;
	}
}

/*!****************************************************************************
 * @brief  Read a char on semihosting mode
 * @param  None
 * @return Character that have read
 */
char SH_GetChar(){
	uint32_t nRet;

	if(coreIsInDebugMode() == 0){
		return -1;
	}

	while(SH_DoCommand(0x101, 0, &nRet) != 0){
		if(nRet != 0){
			SH_DoCommand(0x07, 0, &nRet);
			return (char) nRet;
		}
	}
	return 0;
}

/*************** LGPL ************** END OF FILE *********** D_EL ************/

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
#include <sys/types.h>
#include <sys/stat.h>
#include "semihosting.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "debugCore.h"

/*!****************************************************************************
 * Memory
 */
#define DEBUG_BUF_LEN		256
static unsigned char g_buf[DEBUG_BUF_LEN];
static unsigned char g_buf_len = 0;

/*!****************************************************************************
 * @brief  Transmit a char on semihosting mode
 * @param  ch is the char that to send
 * @return Character to write
 */
void SH_SendChar(uint32_t ch){
	g_buf[g_buf_len++] = ch;
	g_buf[g_buf_len] = '\0';
	if(g_buf_len + 1 >= sizeof(g_buf) || ch == '\n' || ch == '\0'){
		g_buf_len = 0;
		/* Send the char */
		if(SH_DoCommand(0x04, (uint32_t) g_buf, NULL) != 0){
			return;
		}
	}
}

/*!****************************************************************************
 * @brief  Transmit a null-terminated string on semihosting mode
 * @param  str is the string that to send
 * @return Character to write
 */
void SH_SendString(const char *str){
	if(coreIsInDebugMode() == 0){
		return;
	}
	if(SH_DoCommand(0x04, (uint32_t) str, NULL) != 0){
		return;
	}
}

/*!****************************************************************************
 * @brief    print debug message, printf format
 */
#define DEBAG_PRINT_APP ""
char debugPrintString[DEBUG_BUF_LEN] = { DEBAG_PRINT_APP };
void debugn(const char *fmt, ...){
	if(coreIsInDebugMode() == 0){
		return;
	}
	va_list va;
	va_start(va, fmt);
	vsiprintf(debugPrintString + strlen(DEBAG_PRINT_APP), fmt, va);
	va_end(va);
	strcat(debugPrintString, "\n");
	SH_SendString(debugPrintString);	//Send from semihosting
}

/*!****************************************************************************
 * @brief    print debug message, printf format
 */
void debug(const char *fmt, ...){
	if(coreIsInDebugMode() == 0){
		return;
	}
	va_list va;
	va_start(va, fmt);
	vsiprintf(debugPrintString + strlen(DEBAG_PRINT_APP), fmt, va);
	va_end(va);
	SH_SendString(debugPrintString);	//Send from semihosting
}

/*!****************************************************************************
 * @brief  Read a char on semihosting mode
 * @param  None
 * @return Character that have read
 */
char SH_GetChar(){
	uint32_t nRet;

	while(SH_DoCommand(0x101, 0, &nRet) != 0){
		if(nRet != 0){
			SH_DoCommand(0x07, 0, &nRet);
			return (char) nRet;
		}
	}
	return 0;
}

/*************** LGPL ************** END OF FILE *********** D_EL ************/

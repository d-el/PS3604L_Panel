/*!****************************************************************************
 * @file		semihosting.c
 * @author		d_el
 * @version		V1.0
 * @date		17.02.2018
 * @brief		Semihosting driver
 * @copyright	Copyright (C) 2017 Storozhenko Roman
 *				All rights reserved
 *				This software may be modified and distributed under the terms
 *				of the BSD license.	 See the LICENSE file for details
 */

/*!****************************************************************************
 * Include
 */
#include "stddef.h"
#include "debugCore.h"

/*!****************************************************************************
 * Enumeration
 */
// Semihosting operations.
enum OperationNumber{
	// Regular operations
	SEMIHOSTING_EnterSVC = 0x17,
	SEMIHOSTING_ReportException = 0x18,
	SEMIHOSTING_SYS_CLOSE = 0x02,
	SEMIHOSTING_SYS_CLOCK = 0x10,
	SEMIHOSTING_SYS_ELAPSED = 0x30,
	SEMIHOSTING_SYS_ERRNO = 0x13,
	SEMIHOSTING_SYS_FLEN = 0x0C,
	SEMIHOSTING_SYS_GET_CMDLINE = 0x15,
	SEMIHOSTING_SYS_HEAPINFO = 0x16,
	SEMIHOSTING_SYS_ISERROR = 0x08,
	SEMIHOSTING_SYS_ISTTY = 0x09,
	SEMIHOSTING_SYS_OPEN = 0x01,
	SEMIHOSTING_SYS_READ = 0x06,
	SEMIHOSTING_SYS_READC = 0x07,
	SEMIHOSTING_SYS_REMOVE = 0x0E,
	SEMIHOSTING_SYS_RENAME = 0x0F,
	SEMIHOSTING_SYS_SEEK = 0x0A,
	SEMIHOSTING_SYS_SYSTEM = 0x12,
	SEMIHOSTING_SYS_TICKFREQ = 0x31,
	SEMIHOSTING_SYS_TIME = 0x11,
	SEMIHOSTING_SYS_TMPNAM = 0x0D,
	SEMIHOSTING_SYS_WRITE = 0x05,
	SEMIHOSTING_SYS_WRITEC = 0x03,
	SEMIHOSTING_SYS_WRITE0 = 0x04,

	// Codes returned by SEMIHOSTING_ReportException
	ADP_Stopped_ApplicationExit = ((2 << 16) + 38),
	ADP_Stopped_RunTimeError = ((2 << 16) + 35),
};

/*!****************************************************************************
 *
 */
__attribute__((noinline))
int sh_callHost(int reason, const void* argin, void* argout){
	__asm volatile(
	"	BKPT 0xAB		\n"      		/* Wait ICE or HardFault */
	"					\n"				/* ICE will step over BKPT directly */
	"					\n"				/* HardFault will step BKPT and the next line */
	"	B _SH_ICE       \n"
	"_SH_HardFault:     \n"       		/* Captured by HardFault */
	"	MOVS   R0, #0   \n"          	/* Set return value to 0 */
	"	BX LR           \n"             /* Return */
	"_SH_ICE:           \n"             /* Captured by ICE */
	"					\n"				/* Save return value */
	"	CMP R2, #0      \n"
	"	BEQ _SH_End     \n"
	"	STR R0, [R2]    \n"             /* Save the return value to *pn32Out_R0 */
	"_SH_End:           \n"
	"	MOVS R0, #1     \n"          	/* Set return value to 1 */
	"	BX LR           \n"          	/* Return */
	);

	return 1;
}

/*!****************************************************************************
 * @brief  Transmit a null-terminated string on semihosting mode
 * @param  str is the string that to send
 * @return Character to write
 */
void sh_sendString(const char *str){
	if(coreIsInDebugMode() == 0){
		return;
	}

	sh_callHost(SEMIHOSTING_SYS_WRITE0, str, NULL);
}

/*!****************************************************************************
 * @brief  Read a char on semihosting mode
 * @param  None
 * @return Character that have read
 */
char sh_getChar(){
	uint32_t nRet;

	if(coreIsInDebugMode() == 0){
		return -1;
	}

	while(sh_callHost(0x101, NULL, &nRet) != 0){
		if(nRet != 0){
			sh_callHost(0x07, NULL, &nRet);
			return (char) nRet;
		}
	}
	return 0;
}

/***************** Copyright (C) Storozhenko Roman ******* END OF FILE *******/

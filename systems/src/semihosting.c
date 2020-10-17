/*!****************************************************************************
 * @file		semihosting.c
 * @author		d_el
 * @version		V1.0
 * @date		17.02.2018
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		Semihosting driver
 */

/*!****************************************************************************
 * Include
 */
#include "stm32f4xx.h"
#include "stddef.h"
#include "debugCore.h"

/*!****************************************************************************
 * Enumeration
 */
// Semihosting operations
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
 * @param[in] reason R0
 * @param[in] arg0 R1
 * @return R0
 */
int32_t sh_callHost(uint32_t reason, const void* arg0){
	int retval;

	asm volatile (	"mov r0, %1\n"
					"mov r1, %2\n"
					"bkpt 0xAB\n"
					"mov %0, r0"
					: "=r" (retval)
					: "r" (reason), "r" (arg0)
				);

	return retval;
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

	/* Writes a null-terminated string to the debug channel. When executed under
	 * an ARM debugger, the characters appear on the host debugger console.
	 * On entry, R1 contains a pointer to the first byte of the string.
	 */
	sh_callHost(SEMIHOSTING_SYS_WRITE0, str);
}

/*!****************************************************************************
 * @brief  Read a char on semihosting mode
 * @return Character that have read
 */
char sh_getChar(void){
	if(coreIsInDebugMode() == 0){
		return 0;
	}

	/*
	 * Reads a byte from the console
	 */
	char c = sh_callHost(SEMIHOSTING_SYS_READC, NULL);
	return c;
}

/*!****************************************************************************
 * @brief  Read a time
 * @return Returns the number of seconds since 00:00 January 1, 1970
 */
uint32_t sh_getTime(void){
	if(coreIsInDebugMode() == 0){
		return 0;
	}

	/*
	 * Returns the number of seconds since 00:00 January 1, 1970. This is
	 * real-world time, regardless of any debug agent configuration, such as
	 * RVI or DSTREAM.
	 */
	uint32_t t = sh_callHost(SEMIHOSTING_SYS_TIME, NULL);
	return t;
}

/******************************** END OF FILE ********************************/

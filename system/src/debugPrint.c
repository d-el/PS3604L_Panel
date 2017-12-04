/*!****************************************************************************
 * @file		debugPrint.c
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		06.10.2017
 * @copyright	GNU Lesser General Public License v3
 * @brief		--
 */

/*!****************************************************************************
 * Include
 */
#include "stdint.h"
#include "debugCore.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "semihosting.h"

/*!****************************************************************************
 * MEMORY
 */
#define DEBUG_BUF_LEN	64
static char g_buf[DEBUG_BUF_LEN];

/*!****************************************************************************
 * @brief    print debug message, printf format
 */
void l_print(const char *fmt, ...){
	if(coreIsInDebugMode() == 0){
		return;
	}
	va_list va;
	va_start(va, fmt);
	vsiprintf(g_buf, fmt, va);
	va_end(va);
	SH_SendString(g_buf);	//Send from semihosting
}

/*!****************************************************************************
 * @brief    print debug message, printf format
 */
void l_println(const char *fmt, ...){
	if(coreIsInDebugMode() == 0){
		return;
	}
	va_list va;
	va_start(va, fmt);
	vsiprintf(g_buf, fmt, va);
	va_end(va);
	strcat(g_buf, "\n");
	SH_SendString(g_buf);	//Send from semihosting
}

/*!****************************************************************************
 * @brief writes count bytes from buffer into the standard output
 * @param fd: file descriptor of file into which data is written
 * @param buf: data to be written
 * @param count: number of bytes
 * @return returns the number of bytes actually written
 */
int _write(int fd, const void *buf, size_t count){
	if(count > DEBUG_BUF_LEN + 1){
		return -1;
	}
	memcpy(g_buf, buf, count);
	g_buf[count] = 0;

	SH_SendString(g_buf);
	return count;
}


/*************** LGPL ************** END OF FILE *********** D_EL ************/

/*!****************************************************************************
 * @file		write.c
 * @author		d_el
 * @version		V1.0
 * @date		06.12.2017
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		System control task
 */

/*!****************************************************************************
 * Include
 */
#include "stdint.h"
#include "string.h"
#include "debugCore.h"
#include "semihosting.h"
#include "uart.h"
#include "write.h"
#include "FreeRTOS.h"
#include "task.h"

/*!****************************************************************************
 * Define
 */
#define LOCK_WRITE 0

/*!****************************************************************************
 * MEMORY
 */
static int fdstdout;

/*!****************************************************************************
 * @brief
 */
void print_init(stdOutInterface_type stdoi){
	switch(stdoi){
		case write_semihost:
			break;
		case write_rtt:
			break;
		case write_uart:
			break;
	}

	fdstdout = stdoi;
}

/*!****************************************************************************
 * @brief writes count bytes from buffer into the standard output
 * @param fd: file descriptor of file into which data is written
 * @param buf: data to be written
 * @param count: number of bytes
 * @return returns the number of bytes actually written
 */
int _write(int fd, const void *buf, size_t count){
#if (LOCK_WRITE > 0)
	vTaskSuspendAll ();
#endif
	switch(fd){
		case write_semihost:
			sh_sendString(buf);
			break;
		case write_rtt:
			break;
		case write_uart:
			uart_write(uart4, buf, count);
			while(uart4->txState == uartTxRun);
			break;
		default:
			return -1;
	}
#if (LOCK_WRITE > 0)
	xTaskResumeAll ();
#endif
	return count;
}

/******************************** END OF FILE ********************************/

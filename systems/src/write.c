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
#include <uart.h>

/*!****************************************************************************
 * Define
 */
#define LOCK_WRITE 0
#define debugUart uart4

/*!****************************************************************************
 * MEMORY
 */
static int fdstdout;
static char buffer[1024 * 4];
static char *pwrite = &buffer;
static char *pwriteend = &buffer;
static char *pread = &buffer;
static char *preadend = &buffer;
static size_t len;

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

void uartTxHandler(){
//	if(pend > (buffer + sizeof(buffer))){
//
//	}
//
//
//	pwstart += len;
//	len = pend - pwstart;
//	uart_write(debugUart, pwstart, len);
}

/*!****************************************************************************
 * @brief writes count bytes from buffer into the standard output
 * @param fd: file descriptor of file into which data is written
 * @param buf: data to be written
 * @param count: number of bytes
 * @return returns the number of bytes actually written
 */
int _write(int fd, const void *buf, size_t count){
	size_t remaining1 = (pwrite - buffer);
	size_t remaining2 = ((buffer + sizeof(buffer)) - pwriteend);
	if(remaining2 >= count){
		memcpy(pwriteend, buf, count);
		pwrite = pwriteend;
		pwriteend += count;
	}
	if((remaining1 + remaining2) >= count){
		memcpy(pwriteend, buf, remaining2);
		pwrite = pwriteend;
		memcpy(buffer, buf + remaining2, count - remaining2);
		pwriteend = buffer + count - remaining2;
	}
	else{
		while(1);
	}

	if(debugUart->txState != uartTxRun){
		if(pwrite < pwriteend){
			size_t size = pwriteend - pwrite;
			pread = pwrite;
			preadend = pwrite + size;
			uart_write(debugUart, pwrite, size);
		}else{
			size_t size = buffer + sizeof(buffer) - pwrite;
			pread = pwrite;
			preadend = size;
			uart_write(debugUart, pwrite, size);
		}
	}
	return count;
}

/******************************** END OF FILE ********************************/

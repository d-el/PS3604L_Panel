/*!****************************************************************************
 * @file		plog.c
 * @author		d_el
 * @version		V1.1
 * @date		21.03.2025
 * @copyright	The MIT License (MIT). Copyright (c) 2025 Storozhenko Roman
 * @brief		Logging utility
 */

/*!****************************************************************************
 * Include
 */
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include "plog.h"

/*!****************************************************************************
 * MEMORY
 */
static plog_vsnprintf_type log_vsnprintf;
static plog_write_type log_write;
static plog_timestamp_type log_timestamp;
static char log_buf[1024];
static int log_fd;

/*!****************************************************************************
 * @brief Set function used to output log entries
 *
 * By default, log output goes to UART0. This function can be used to redirect log
 * output to some other destination, such as file or network. Returns the original
 * log handler, which may be necessary to return output to the previous destination.
 *
 * @param func new Function used for output. Must have same signature as vprintf.
 *
 * @return func old Function used for output.
 */
plog_vsnprintf_type plog_setVprintf(plog_vsnprintf_type func){
	plog_vsnprintf_type oldfunc = log_vsnprintf;
	log_vsnprintf = func;
	return oldfunc;
}

/*!****************************************************************************
 * @brief set write function for write
 * @param func new write function
 * @return old write function
 */
plog_write_type plog_setWrite(plog_write_type func){
	plog_write_type oldfunc = log_write;
	log_write = func;
	return oldfunc;
}

/*!****************************************************************************
 * @brief set file descriptor for log
 * @param fd: new file descriptor of file into which data is written
 * @return old file descriptor
 */
int plog_setWriteFd(int fd){
	int oldfd = log_fd;
	log_fd = fd;
	return oldfd;
}

/*!****************************************************************************
 *
 * @param
 * @return
 */
plog_timestamp_type plog_setTimestamp(plog_timestamp_type func){
	plog_timestamp_type oldfunc = log_timestamp;
	log_timestamp = func;
	return oldfunc;
}

/*!****************************************************************************
 *
 */
static int plog_vprintf(const char* format, va_list arg){
	plog_vsnprintf_type p_vprintf = log_vsnprintf ? log_vsnprintf : vsnprintf;
	int len = p_vprintf(log_buf, sizeof(log_buf), format, arg);

	if(log_write != NULL){
		len = log_write(log_fd, log_buf, len);
	}else{
		len = write(log_fd, log_buf, len);
	}

	return len;
}

/*!****************************************************************************
 * @brief
 *
 */
int plog_printf(const char* format, ...){
	va_list va;
	va_start(va, format);
	int len = plog_vprintf(format, va);
	va_end(va);
	return len;
}

/*!****************************************************************************
 * @brief Write message into the log
 *
 * This function is not intended to be used directly. Instead, use one of
 * P_LOGE, P_LOGW, P_LOGI, P_LOGD, P_LOGV macros.
 *
 * This function or these macros should not be used from an interrupt.
 */
int plog_write(plog_level_t level, const char* tag, const char* format, ...){
	(void)level;
	(void)tag;
	va_list va;
	va_start(va, format);
	int len = plog_vprintf(format, va);
	va_end(va);
	return len;
}

/*!****************************************************************************
 * @brief
 *
 */
int plog_hexdumpcolumn(const void *buffer, size_t length, size_t column){
	if(!length) return 0;
	const uint8_t *bbuffer = (uint8_t*)buffer;
	size_t plen = 0;
	while(length){
		size_t linelen = column < length ? column : length;
		plen += plog_printf("%p  ", bbuffer);
		for(size_t i = 0; i < linelen; i++){
			plen += plog_printf("%02X ", bbuffer[i]);
		}
		plen += plog_printf("   ");
		if(linelen < column){
			plen += plog_printf("%*c", (column -  linelen) * 3, ' ');
		}
		for(size_t i = 0; i < column && length; i++){
			char c = isalpha(bbuffer[i]) ? bbuffer[i] : '.';
			plen += plog_printf("%c", c);
		}
		plen += plog_printf("\n");
		bbuffer += linelen;
		length -= linelen;
	}
	return plen;
}

/*!****************************************************************************
 * @brief Function which returns timestamp to be used in log output
 *
 * This function is used in expansion of P_LOGx macros.
 * In the 2nd stage bootloader, and at early application startup stage
 * this function uses CPU cycle counter as time source. Later when
 * FreeRTOS scheduler start running, it switches to FreeRTOS tick count.
 *
 * For now, we ignore millisecond counter overflow.
 *
 * @return timestamp, in milliseconds
 */
uint32_t plog_timestamp(void){
	if(log_timestamp != NULL){
		return log_timestamp();
	}else{
		return 0;
	}
}

/******************************** END OF FILE ********************************/

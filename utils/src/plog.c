/*!****************************************************************************
 * @file		plog.c
 * @author		d_el
 * @version		V1.0
 * @date		25.05.2018
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		Logging library
 */

/*!****************************************************************************
 * Include
 */
#include "plog.h"
#include "stdio.h"
#include "string.h"

/*!****************************************************************************
 * MEMORY
 */
static plog_vprintf_type log_vsprintf;
static plog_write_type log_write;
static plog_timestamp_type log_timestamp;
static char log_buf[256];
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
plog_vprintf_type plog_setVprintf(plog_vprintf_type func){
	plog_vprintf_type oldfunc = log_vsprintf;
	log_vsprintf = func;
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
	plog_timestamp_type oldfunc = plog_timestamp;
	log_timestamp = func;
	return oldfunc;
}

/*!****************************************************************************
 * @brief Write message into the log
 *
 * This function is not intended to be used directly. Instead, use one of
 * P_LOGE, P_LOGW, P_LOGI, P_LOGD, P_LOGV macros.
 *
 * This function or these macros should not be used from an interrupt.
 */
void plog_write(plog_level_t level, const char* tag, const char* format, ...){
	(void)level;
	(void)tag;
	if(log_vsprintf != NULL){
		va_list va;
		va_start(va, format);
		int len = log_vsprintf(log_buf, format, va);
		va_end(va);

		if(log_write != NULL){
			log_write(log_fd, log_buf, len);
		}
	}
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

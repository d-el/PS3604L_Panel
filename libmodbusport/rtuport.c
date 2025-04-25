/*!****************************************************************************
 * @file		rtuport.c
 * @author		d_el
 * @version		V1.0
 * @date		Dec 27, 2020
 * @brief
 * @copyright	The MIT License (MIT). Copyright (c) 2020 Storozhenko Roman
 * @brief		connect interface with regulator
 */

/*!****************************************************************************
 * Include
 */
#include <unistd.h>
#include <assert.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <uart.h>
#include <plog.h>

/*!****************************************************************************
 * MEMORY
 */
#define LIBMODBUSUART uart1
static SemaphoreHandle_t uartRxSem;
#define LOG_LOCAL_LEVEL P_LOG_WARN
static char *logTag = "port";

/*!****************************************************************************
 * @brief
 */
void _modbus_serial_init(void){

}

ssize_t modbus_serial_send(const uint8_t *req, int req_length){
	P_LOGD(logTag, "TX (%"PRIu16")", req_length);
	if(LOG_LOCAL_LEVEL >= P_LOG_DEBUG){
		plog_hexdumpcolumn(req, req_length, 16);
	}
	uart_write(LIBMODBUSUART, req, req_length);
	return req_length;
}

ssize_t modbus_serial_recv(uint8_t *rsp, int rsp_length, int response_timeout){
	if(LIBMODBUSUART->rxState != uartRxRun){
		uart_read(LIBMODBUSUART, rsp, rsp_length);
	}
	BaseType_t res = xSemaphoreTake(uartRxSem, pdMS_TO_TICKS(response_timeout / 1000/*us to ms*/));
	if(res == pdTRUE){
		size_t received = UART1_RxBffSz - uartGetRemainRx(LIBMODBUSUART);
		P_LOGD(logTag, "RX (%"PRIu16")", received);
		if(received == 0){
			return -1; // I/O error
		}
		if(LOG_LOCAL_LEVEL >= P_LOG_DEBUG){
			plog_hexdumpcolumn(rsp, received, 16);
		}
		return received;
	}
	else{
		P_LOGD(logTag, "RX timeout");
		return -2; // Timeout
	}
}

static void uartRxHandler(uart_type *puart){
	(void)puart;
	if(LIBMODBUSUART->rxState == uartRxSuccess){
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		xSemaphoreGiveFromISR(uartRxSem, &xHigherPriorityTaskWoken);
		portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
	}
}

int modbus_serial_connect(const char *device, uint32_t baud, uint8_t parity, uint8_t data_bit, char stop_bit){
	(void)device;
	(void)parity;
	(void)data_bit;
	(void)stop_bit;

	// Create Semaphore for UART
	uartRxSem = xSemaphoreCreateBinary();
	assert(uartRxSem != NULL);

	uart_init(LIBMODBUSUART, baud);
	uart_setCallback(LIBMODBUSUART, NULL, uartRxHandler);
	return 0;
}

/******************************** END OF FILE ********************************/

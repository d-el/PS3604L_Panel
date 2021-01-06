/*!****************************************************************************
 * @file		rtuport.c
 * @author		d_el
 * @version		V1.0
 * @date		Dec 27, 2020
 * @brief
 * @copyright	Copyright (C) 2017 Storozhenko Roman
 *				All rights reserved
 *				This software may be modified and distributed under the terms
 *				of the BSD license.	 See the LICENSE file for details
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

/*!****************************************************************************
 * MEMORY
 */
#define LIBMODBUSUART uart1
static SemaphoreHandle_t uartRxSem;

/*!****************************************************************************
 * @brief
 */
void _modbus_serial_init(void){

}

ssize_t modbus_serial_send(const uint8_t *req, int req_length){
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
		if(received == 0){
			return -1; // I/O error
		}
		return received;
	}
	else{
		return -2; // Timeout
	}
}

static void uartRxHandler(uart_type *puart){
	(void)puart;
	if(LIBMODBUSUART->rxState == uartRxSuccess){
		BaseType_t xHigherPriorityTaskWoken;
		xHigherPriorityTaskWoken = pdFALSE;
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
	vSemaphoreCreateBinary(uartRxSem);
	assert(uartRxSem != NULL);
	xSemaphoreTake(uartRxSem, portMAX_DELAY);

    uart_init(LIBMODBUSUART, baud);
    uart_setCallback(LIBMODBUSUART, NULL, uartRxHandler);
    return 0;
}

/***************** Copyright (C) Storozhenko Roman ******* END OF FILE *******/

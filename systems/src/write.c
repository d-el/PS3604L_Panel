/******************************************************************************
 * @file		write.c
 * @author		d_el
 * @version		V2.0
 * @date		09.02.2025
 * @copyright	The MIT License (MIT). Copyright (c) 2025 Storozhenko Roman
 * @brief		System control task
 */

/*!****************************************************************************
 * Include
 */
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <hal/uart.h>

#define debugUart uart4
#define BUFFER_LEN 1024*4

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

static char buffer[BUFFER_LEN];
static char *bffWr = buffer;
static char *bffRd = buffer;
static char *bffEnd = &buffer[sizeof(buffer) - 1];
static size_t bffSize = sizeof(buffer);

/*!****************************************************************************
 * @brief
 */
void txHandler(uart_type *uartx){
	(void)uartx;
	if(debugUart->txState != uartTxRun){
		if(bffRd < bffWr){
			uart_write(debugUart, bffRd, bffWr - bffRd);
			bffRd = bffWr;
		}
		if(bffRd > bffWr){
			uart_write(debugUart, bffRd, bffEnd - bffRd);
			bffRd = &buffer[0];
		}
	}
}

/*!****************************************************************************
 * @brief
 */
__attribute__((constructor))
void writeInit(void){
	uart_init(debugUart, 921600);
	uart_setCallback(debugUart, txHandler, NULL);
}

/*!****************************************************************************
 * @brief
 */
uint32_t coreInInterruptMode(void){
	if(SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk){
		return 1;	// Interrupt is active, not in thread mode
	}else{
		return 0;	// Interrupt is not active, we are in thread mode
	}
}

/*!****************************************************************************
 * @brief
 */
int _write(int file, const void *ptr, unsigned int len){
	(void)file;

	UBaseType_t uxSavedInterruptStatus = 0;
	if(coreInInterruptMode()){
		uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
	}else{
		taskENTER_CRITICAL();
	}

	const char* pData = (char* const)ptr;
	int numBytes = len;
	int numBytesToWrite;
	int numBytesWritten;

	// Write data to buffer and handle wrap-around if necessary
	numBytesWritten = 0u;
	do{
		if(bffRd > bffWr){
			numBytesToWrite = bffRd - bffWr - 1u;
		}else{
			numBytesToWrite = bffSize - (bffWr - bffRd + 1u);
		}
		numBytesToWrite = MIN(numBytesToWrite, (bffEnd - bffWr));	// Number of bytes that can be written until buffer wrap-around
		numBytesToWrite = MIN(numBytesToWrite, numBytes);
#if 1
		numBytesWritten += numBytesToWrite;
		numBytes        -= numBytesToWrite;
		while (numBytesToWrite--) {
			*bffWr++ = *pData++;
		};
#else
		memcpy((void*) pDst, pData, numBytesToWrite);
		numBytesWritten += numBytesToWrite;
		pData += numBytesToWrite;
		numBytes -= numBytesToWrite;
		bffWr += numBytesToWrite;
#endif
		if(bffWr == bffEnd){
			bffWr = buffer;
		}
	}while(numBytes);

	// Send from UART
	if(debugUart->txState != uartTxRun){
		if(bffRd < bffWr){
			uart_write(debugUart, bffRd, bffWr - bffRd);
			bffRd = bffWr;
		}
		if(bffRd > bffWr){
			uart_write(debugUart, bffRd, bffEnd - bffRd);
			bffRd = buffer;
		}
	}

	if(coreInInterruptMode()){
		taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);
	}else{
		taskEXIT_CRITICAL();
	}

	return numBytesWritten;
}

/******************************** END OF FILE ********************************/

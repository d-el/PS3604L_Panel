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
#include <string.h>
#include <assert.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <uart.h>

#define debugUart uart4

#define qLen 64
#define qElemebtLen 64

typedef struct{
	size_t len;
	char data[qElemebtLen];
} xMessage_t;

xMessage_t workMessage;
static StaticQueue_t xStaticQueue; /* The variable used to hold the queue's data structure. */
static uint8_t ucQueueStorageArea[qLen * sizeof(xMessage_t)];
static QueueHandle_t queueCommand;

void txHandler(uart_type *uartx){
	(void)uartx;
    if(debugUart->txState != uartTxRun){
    	BaseType_t res = xQueueReceiveFromISR(queueCommand, &workMessage, 0);
    	if(res == pdPASS){
    		uart_write(debugUart, workMessage.data, workMessage.len);
    	}
    }
}

__attribute__((constructor))
void writeInit(void){
	queueCommand = xQueueCreateStatic(qLen, sizeof(xMessage_t), ucQueueStorageArea, &xStaticQueue);
	assert(queueCommand != NULL);
    uart_init(debugUart, 460800);
    uart_setCallback(debugUart, txHandler, NULL);
}

/*!****************************************************************************
 * @brief
 */
uint32_t coreInInterruptMode(void){
	if(SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk){
		return 1;   // Interrupt is active, not in thread mode
	}else{
		return 0;   // Interrupt is not active, we are in thread mode
	}
}

int _write(int file, const void *ptr, unsigned int len) {
	(void)file;
    taskENTER_CRITICAL();

    const uint8_t *data = (uint8_t*)ptr;
    BaseType_t res = pdFAIL;
    size_t empty;

    if(coreInInterruptMode()){
    	empty = qLen - uxQueueMessagesWaitingFromISR(queueCommand);
    }else{
    	empty = qLen - uxQueueMessagesWaiting(queueCommand);
    }

    if((empty * qElemebtLen) >= len){
		size_t index = 0;
		while(index < len){
			size_t availableLen = (len - index) < qElemebtLen ? len - index : qElemebtLen;
			xMessage_t m;
			memcpy(m.data, &data[index], availableLen);
			m.len = availableLen;
			index += availableLen;
			if(coreInInterruptMode()){
				res = xQueueSendToBackFromISR(queueCommand, (void*)&m, 0);
			}else{
				res = xQueueSendToBack(queueCommand, (void*)&m, 0);
			}
		}

		if(res == pdPASS && debugUart->txState != uartTxRun){
			if(coreInInterruptMode()){
				res = xQueueReceiveFromISR(queueCommand, &workMessage, 0);
			}else{
				res = xQueueReceive(queueCommand, &workMessage, 0);
			}

			if(res == pdPASS){
				uart_write(debugUart, workMessage.data, workMessage.len);
			}
		}
    }else{
    	//while(1);
    }

    //while(debugUart->txState == uartTxRun);
    taskEXIT_CRITICAL();

    return len;

}

/******************************** END OF FILE ********************************/

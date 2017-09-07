/*!****************************************************************************
 * @file		OSinit.c
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @copyright	GNU Lesser General Public License v3
 * @brief		Create semaphore, mutex, nitialization operational system
 */

/*!****************************************************************************
 * Include
 */
#include "OSinit.h"

/*!****************************************************************************
 * Allocate the memory for the heap on SRAM_2
 */
uint8_t __attribute__((section (".ram2bss"))) ucHeap[ configTOTAL_HEAP_SIZE ];

/*!****************************************************************************
 * TaskHandle
 */
TaskHandle_t windowTskHandle;

/******************************************************************************
 * Semaphore
 */
SemaphoreHandle_t uart1TxSem;
SemaphoreHandle_t uart1RxSem;
SemaphoreHandle_t uart2RxSem;

#include "board.h"
#include "uart.h"
void vTaskLED1(void *pvParameters){
	static uint8_t  buf[3] = { 0x55, 0xAA, 0x77 };

	for(;;){
		gppin_toggle(GP_LED3);
		uart_write(uart1, &buf[0], sizeof(buf));
		vTaskDelay(500);
	}
}

/*!****************************************************************************
 * @brief Init operating system
 */
void OSinit(void){
	BaseType_t Result = pdTRUE;

	//Create semaphore
	vSemaphoreCreateBinary(uart1TxSem);
	xSemaphoreTake(uart1TxSem, portMAX_DELAY);

	vSemaphoreCreateBinary(uart1RxSem);
	xSemaphoreTake(uart1RxSem, portMAX_DELAY);

	vSemaphoreCreateBinary(uart2RxSem);
	xSemaphoreTake(uart2RxSem, portMAX_DELAY);

	//Result &= xTaskCreate(systemTSK, "systemTSK", SYSTEM_TSK_SZ_STACK, NULL, SYSTEM_TSK_PRIO, NULL);
	//Result &= xTaskCreate(uartTSK, "uartTSK", UART_TSK_SZ_STACK, NULL, UART_TSK_PRIO, NULL);
	//Result &= xTaskCreate(wlanTSK, "wlanTSK", WLAN_TSK_SZ_STACK, NULL, WLAN_TSK_PRIO, NULL);
	Result &= xTaskCreate(vTaskLED1, "vTaskLED1", 512, NULL, 2, (TaskHandle_t *) NULL);
	Result &= xTaskCreate(httpServerTSK,	"httpServerTSK",	HTTP_TSK_SZ_STACK,	NULL,	HTTP_TSK_PRIO, NULL);

	if(Result == pdTRUE){

	}else{
		while(1){
			/*LED_ON();
			delay_ms(100);
			LED_OFF();
			delay_ms(900);*/
		}
	}

	//selWindow(startupWindow);

	vTaskStartScheduler();
}

/*************** LGPL ************** END OF FILE *********** D_EL ************/

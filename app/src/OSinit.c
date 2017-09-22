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
#include "stm32f4x7_eth.h"

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

void *pointersTrace[10];
uint8_t linkState;
void vTaskLED1(void *pvParameters){
	char str[32];
	static uint8_t  buf[3] = { 0x55, 0xAA, 0x77 };
	lcd_setColor(black, red);

	while(1){
		gppin_toggle(GP_LED3);

		//Link Down
		if(gppin_get(GP_LANnINT) == 0){
			ETH_ReadPHYRegister(1, PHY_BSR);
			gppin_reset(GP_LED2);
			linkState = 0;
		}

		//Link Up
		if(linkState == 0){
			if((ETH_ReadPHYRegister(1, PHY_BSR) & PHY_Linked_Status) != 0){
				gppin_set(GP_LED2);
				linkState = 1;
			}
		}
		uart_write(uart1, &buf[0], sizeof(buf));
		/*rtc_getTime(&timeStrct);
		strftime(str, sizeof(str), "%H:%M:%S", &timeStrct);
		debugn(str);*/

		/*extern uint32_t call_get;
		sprintf(str, "Get: %u", call_get);
		lcd_putStr(0, 0, &arial, 0, str);*/

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

	Result &= xTaskCreate(systemTSK, "systemTSK", SYSTEM_TSK_SZ_STACK, NULL, SYSTEM_TSK_PRIO, NULL);
	Result &= xTaskCreate(uartTSK, "uartTSK", UART_TSK_SZ_STACK, NULL, UART_TSK_PRIO, NULL);
	//Result &= xTaskCreate(wlanTSK, "wlanTSK", WLAN_TSK_SZ_STACK, NULL, WLAN_TSK_PRIO, NULL);
	//Result &= xTaskCreate(vTaskLED1, "vTaskLED1", 512, NULL, 2, (TaskHandle_t *) NULL);
	//Result &= xTaskCreate(httpServerTSK,	"httpServerTSK",	HTTP_TSK_SZ_STACK,	NULL,	HTTP_TSK_PRIO, NULL);

	if(Result == pdTRUE){

	}else{
		while(1){
			/*LED_ON();
			delay_ms(100);
			LED_OFF();
			delay_ms(900);*/
		}
	}

	selWindow(startupWindow);

	vTaskStartScheduler();
}

/*************** LGPL ************** END OF FILE *********** D_EL ************/

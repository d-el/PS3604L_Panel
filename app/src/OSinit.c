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

/******************************************************************************
 * Semaphore
 */


/*!****************************************************************************
 * @brief Init operating system
 */
void OSinit(void){
	BaseType_t Result = pdTRUE;

	Result &= xTaskCreate(systemTSK, "systemTSK", SYSTEM_TSK_SZ_STACK, NULL, SYSTEM_TSK_PRIO, NULL);
	//Result &= xTaskCreate(uartTSK, "uartTSK", UART_TSK_SZ_STACK, NULL, UART_TSK_PRIO, NULL);
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

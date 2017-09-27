/*!****************************************************************************
 * @file		wlanTSK.c
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		07.08.2017
 * @copyright	GNU Lesser General Public License v3
 * @brief		WLAN program task
 */

/*!****************************************************************************
 * Include
 */
#include "wlanTSK.h"

/*!****************************************************************************
 * MEMORY
 */
SemaphoreHandle_t uart1TxSem;
SemaphoreHandle_t uart1RxSem;
wlan_type wlan;
uint8_t wlanTx[128];
uint8_t wlanMas[128];
uint8_t wlanRx[128];
uint8_t wlanActive;
uint8_t wlanRxActive;

/******************************************************************************
 * Local prototypes for the functions
 */
static void wlanUartTskRxkHook(uart_type *puart);
static void wlanUartTskTxkHook(uart_type *puart);
void wlanUartTx(uint8_t *src, uint16_t len);
void wlanUartRx(uint8_t *dst, uint16_t len);

/*!****************************************************************************
 * @brief
 */
void debugWlanPrint(char *string){
	SH_SendString("> \r\n");
	SH_SendString(string);
	SH_SendString("\r\n");
	memset(string, 0, 128);
}

/*!****************************************************************************
 * @brief
 */
uint32_t pos;
void wlanTSK(void *pPrm){
	TickType_t	xLastWakeTime = xTaskGetTickCount();

	//Create semaphore
	vSemaphoreCreateBinary(uart1TxSem);
	xSemaphoreTake(uart1TxSem, portMAX_DELAY);

	vSemaphoreCreateBinary(uart1RxSem);
	xSemaphoreTake(uart1RxSem, portMAX_DELAY);

	//gppin_set(GP_GPIO2);

	vTaskDelay(1000);
	uart_setCallback(wlanTskUse, wlanUartTskTxkHook, wlanUartTskRxkHook);
	xSemaphoreTake(uart1RxSem, 0);

	//Soft reset
	strcpy(wlanTx, "AT+RST\r\n");
	uart_read(wlanTskUse, wlanRx, 256);
	wlanUartTx(wlanTx, strlen(wlanTx));
	xSemaphoreTake(uart1RxSem, portMAX_DELAY);
	vTaskDelay(1000);
	debugWlanPrint(wlanRx);

	//
	strcpy(wlanTx, "AT\r\n");
	uart_read(wlanTskUse, wlanRx, 256);
	wlanUartTx(wlanTx, strlen(wlanTx));
	xSemaphoreTake(uart1RxSem, portMAX_DELAY);
	vTaskDelay(100);
	debugWlanPrint(wlanRx);

	//Set MAC
	/*strcpy(wlanTx, "AT+CIPSTAMAC_CUR=\"11:22:33:44:55:66\"\r\n");
	uart_read(wlanTskUse, wlanRx, 256);
	wlanUartTx(wlanTx, strlen(wlanTx));
	xSemaphoreTake(uart1RxSem, portMAX_DELAY);
	vTaskDelay(100);
	debugWlanPrint(wlanRx);*/

	//Set MODE 1 : ESP8266 station
	strcpy(wlanTx, "AT+CWMODE=1\r\n");
	uart_read(wlanTskUse, wlanRx, 256);
	wlanUartTx(wlanTx, strlen(wlanTx));
	xSemaphoreTake(uart1RxSem, portMAX_DELAY);
	vTaskDelay(100);
	debugWlanPrint(wlanRx);

	//Запрашиваем список доступных WIFI сетей
	strcpy(wlanTx, "AT+CWLAP\r\n");
	uart_read(wlanTskUse, wlanRx, 256);
	wlanUartTx(wlanTx, strlen(wlanTx));
	xSemaphoreTake(uart1RxSem, portMAX_DELAY);
	vTaskDelay(1000);
	debugWlanPrint(wlanRx);

	//Подключаемся к LINK_D_EL
	strcpy(wlanTx, "AT+CWJAP=\"LINK_D_EL\",\"sn058006790\"\r\n");
	uart_read(wlanTskUse, wlanRx, 256);
	wlanUartTx(wlanTx, strlen(wlanTx));
	xSemaphoreTake(uart1RxSem, portMAX_DELAY);
	vTaskDelay(5000);
	debugWlanPrint(wlanRx);

	//
	strcpy(wlanTx, "AT+CIFSR\r\n");
	uart_read(wlanTskUse, wlanRx, 256);
	wlanUartTx(wlanTx, strlen(wlanTx));
	xSemaphoreTake(uart1RxSem, portMAX_DELAY);
	vTaskDelay(1000);
	debugWlanPrint(wlanRx);

	//Устанавливаем режим передачи командой
	strcpy(wlanTx, "AT+CIPMODE=0\r\n");
	uart_read(wlanTskUse, wlanRx, 256);
	wlanUartTx(wlanTx, strlen(wlanTx));
	xSemaphoreTake(uart1RxSem, portMAX_DELAY);
	vTaskDelay(1000);
	debugWlanPrint(wlanRx);

	//Устанавливаем возможность множественных соединений
	strcpy(wlanTx, "AT+CIPMUX=0\r\n");
	uart_read(wlanTskUse, wlanRx, 256);
	wlanUartTx(wlanTx, strlen(wlanTx));
	xSemaphoreTake(uart1RxSem, portMAX_DELAY);
	vTaskDelay(1000);
	debugWlanPrint(wlanRx);

	//Запускаем сервер на порту 8888
	strcpy(wlanTx, "AT+CIPSERVER=1,8888\r\n");
	uart_read(wlanTskUse, wlanRx, 256);
	wlanUartTx(wlanTx, strlen(wlanTx));
	xSemaphoreTake(uart1RxSem, portMAX_DELAY);
	vTaskDelay(1000);
	debugWlanPrint(wlanRx);

	xSemaphoreTake(uart1RxSem, 0);

	strcpy(wlanTx, "Server started\r\n");
	debugWlanPrint(wlanTx);
	wlan.wlanActive = 1;

	while(1){
		uart_read(wlanTskUse, wlanRx, 256);
		xSemaphoreTake(uart1RxSem, portMAX_DELAY);
		/*SH_SendString("> \r\n");
		SH_SendString(wlanRx);
		SH_SendString("\r\n");*/

		//if(strstr("\r\n+IPD", wlanRx) == NULL){
		static uint32_t cnt = 0;
		if(wlanRx[2] == '+'){
			uint8_t idClient;
			uint8_t lenRx;
			sscanf (wlanRx, "\r\n+IPD,%d,%d", &idClient, &lenRx);
			//sprintf(wlanMas, "cnt = %u, id = %u, len = %u", cnt, idClient, lenRx);
			//debugWlanPrint(wlanMas);

			memset(wlanMas, 0, sizeof(wlanMas));

			for(pos = 0; pos < 20; pos++){
				if(wlanRx[pos] == ':'){
					break;
				}
			}
			memcpy(wlanMas, wlanRx + pos + 1, lenRx);

			//if(lenRx == sizeof(wlanTask_type)){
			if(lenRx == 14){
				memcpy(&wlan.task, wlanRx + pos + 1, lenRx);
				wlan.wlanRxActive = 1;
			}

			//Отправим
			memset(wlanTx, 0, sizeof(wlanTx));
			sprintf(wlanTx, "AT+CIPSEND=%u,%u\r\n", idClient, lenRx);
			uart_read(wlanTskUse, wlanRx, 256);
			wlanUartTx(wlanTx, strlen(wlanTx));
			xSemaphoreTake(uart1RxSem, portMAX_DELAY);

			//
			memset(wlanTx, 0, sizeof(wlanTx));
			sprintf(wlanTx, "%s\r\n", wlanMas);
			uart_read(wlanTskUse, wlanRx, 256);
			wlanUartTx(wlanTx, strlen(wlanTx));
			xSemaphoreTake(uart1RxSem, portMAX_DELAY);
			//xSemaphoreTake(uart1RxSem, 100);
		}

		BeepTime(10, 4100);
		cnt++;
		memset(wlanRx, 0, sizeof(wlanRx));

		//vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(WLAN_TSK_PERIOD));
	}
}

/*!****************************************************************************
 * @brief	uart TX callback
 */
static void wlanUartTskTxkHook(uart_type *puart){
	BaseType_t xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(uart1TxSem, &xHigherPriorityTaskWoken);
	if(xHigherPriorityTaskWoken != pdFALSE){
		portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
	}
}

/*!****************************************************************************
 * @brief	uart RX callback
 */
static void wlanUartTskRxkHook(uart_type *puart){
	BaseType_t xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(uart1RxSem, &xHigherPriorityTaskWoken);
	if(xHigherPriorityTaskWoken != pdFALSE){
		portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
	}
}

/*!****************************************************************************
 * @brief	uart TX
 */
void wlanUartTx(uint8_t *src, uint16_t len){
	uart_write(wlanTskUse, src, len);
	xSemaphoreTake(uart1TxSem, portMAX_DELAY);
}

/*!****************************************************************************
 * @brief	uart RX
 */
void wlanUartRx(uint8_t *dst, uint16_t len){
	uart_read(wlanTskUse, dst, len);
	xSemaphoreTake(uart1RxSem, portMAX_DELAY);
}

/*************** LGPL ************** END OF FILE *********** D_EL ************/

/*!****************************************************************************
 * @file		modbusServerTSK.cpp
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		May 25, 2025
 * @copyright	The MIT License (MIT). Copyright (c) 2025 Storozhenko Roman
 * @brief		HTTP Server
 */

/*!****************************************************************************
 * Include
 */
#include <mb.h>
#include <FreeRTOS.h>
#include <task.h>

/*!****************************************************************************
 *
 */
void modbusServerTSK(void *pPrm){
	(void)pPrm;
	eMBTCPInit(502/*Port*/);
	eMBEnable();

	while(1){
		eMBPoll();
		vTaskDelay(1);
	}
}

/******************************** END OF FILE ********************************/

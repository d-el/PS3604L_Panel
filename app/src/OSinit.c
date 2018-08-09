/*!****************************************************************************
 * @file		OSinit.c
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		Itialization operational system
 */

/*!****************************************************************************
 * Include
 */
#include "OSinit.h"
#include "FreeRTOS.h"
#include "task.h"
#include "systemTSK.h"
#include "assert.h"

/*!****************************************************************************
 * @brief Init operating system
 */
void OSinit(void){
	BaseType_t Result = pdTRUE;

	Result &= xTaskCreate(systemTSK, "systemTSK", SYSTEM_TSK_SZ_STACK, NULL, SYSTEM_TSK_PRIO, NULL);
	assert(Result == pdTRUE);
	vTaskStartScheduler();
}

/******************************** END OF FILE ********************************/

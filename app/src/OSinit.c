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
#include <printp.h>
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
	stopif(Result != pdTRUE, return, "can not create systemTSK");
	vTaskStartScheduler();
}

/*************** LGPL ************** END OF FILE *********** D_EL ************/

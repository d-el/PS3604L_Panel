/*!****************************************************************************
 * @file		OSinit.h
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @copyright	GNU Lesser General Public License v3
 * @brief		Create semaphore, mutex, nitialization operational system
 */

/*!****************************************************************************
 * Include
 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include "httpServerTSK.h"
//#include "systemTSK.h"
//#include "baseTSK.h"
//#include "chargeTSK.h"
//#include "settingTSK.h"
//#include "wlanTSK.h"

/*!****************************************************************************
 *-------NAME--------------------size [4byte Word] */
#define SYSTEM_TSK_SZ_STACK     128+128
#define BASE_TSK_SZ_STACK       256
#define CHARG_TSK_SZ_STACK      256
#define UART_TSK_SZ_STACK       256
#define WLAN_TSK_SZ_STACK       256
#define SETT_TSK_SZ_STACK       256
#define STARTUP_TSK_SZ_STACK    256
#define CUBE_TSK_SZ_STACK       256
#define BUBLE_TSK_SZ_STACK      64
#define HTTP_TSK_SZ_STACK      	512

/*!****************************************************************************
 *-------NAME--------------------size [4byte Word] */
#define SYSTEM_TSK_PRIO         3
#define BASE_TSK_PRIO           1
#define CHARG_TSK_PRIO          1
#define UART_TSK_PRIO           2
#define WLAN_TSK_PRIO           1
#define SETT_TSK_PRIO           1
#define STARTUP_TSK_PRIO        1
#define CUBE_TSK_PRIO           1
#define BUBLE_TSK_PRIO          1
#define HTTP_TSK_PRIO          	1

extern uint8_t __attribute__((section (".ram2bss"))) ucHeap[ configTOTAL_HEAP_SIZE ];

/*!****************************************************************************
 * TaskHandle
 */
extern TaskHandle_t windowTskHandle;

/*!****************************************************************************
 * Semaphore
 */
extern SemaphoreHandle_t uart1TxSem;
extern SemaphoreHandle_t uart1RxSem;
extern SemaphoreHandle_t uart2RxSem;

/*!****************************************************************************
 * Function declaration
 */
void OSinit(void);

/*************** LGPL ************** END OF FILE *********** D_EL ************/

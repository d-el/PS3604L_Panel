/*!****************************************************************************
 * @file		wlanTSK.h
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		07.08.2017
 * @copyright	GNU Lesser General Public License v3
 * @brief		WLAN program task
 */
#ifndef wlanTSK_H
#define wlanTSK_H

/*!****************************************************************************
 * Include
 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "crc.h"
#include "pstypes.h"
#include "OSinit.h"
#include "systemTSK.h"
#include "uart.h"
#include "semihosting.h"

/*!****************************************************************************
 * Define
 */
#define wlanTskUse          		(uart1)
#define WLAN_TSK_QUEUE_COMMAND_LEN	(16)
#define WLAN_TSK_MAX_WAIT_ms    	(100)
#define WLAN_TSK_MAX_ERR 			(10)
#define WLAN_TSK_PERIOD				(1000)	///< [ms]

/*!****************************************************************************
 * Enumeration
 */

/*!****************************************************************************
 * Typedef
 */

typedef struct{
	uint32_t u;              ///< [X_XXXXXX V]
	uint32_t i;              ///< [X_XXXXXX A]
	uint32_t time;           ///< [s]
	request_type request	:8;
	psMode_type mode		:8;
} wlanTask_type;

typedef struct{
	uint32_t power;          ///< [X_XXX Wt]
	uint32_t resistens;      ///< [X_XXX Ohm]
	uint32_t time;           ///< [s]
	uint32_t capacity;       ///< [X_XXX A/h]
	uint32_t u;              ///< [X_XXXXXX V]
	uint32_t i;              ///< [X_XXXXXX A]
	uint16_t adcu;           ///< [LSB]
	uint16_t adci;           ///< [LSB]
	uint16_t uin;            ///< [mV]
	uint16_t temperatureLin; ///< [X_X °С]
} wlanMeas_type;

typedef struct{
	wlanTask_type	task;
	wlanMeas_type	meas;
	uint8_t 		wlanActive;
	uint8_t 		wlanRxActive;
}wlan_type;

/*!****************************************************************************
 * Exported variables
 */
extern wlan_type wlan;

/*!****************************************************************************
 * Macro functions
 */

/*!****************************************************************************
 * Function declaration
 */
void wlanTSK(void *pPrm);

#endif //wlanTSK_H
/*************** LGPL ************** END OF FILE *********** D_EL ************/

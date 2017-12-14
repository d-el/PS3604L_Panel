/*!****************************************************************************
 * @file		regulatorConnTSK.h
 * @author		d_el
 * @version		V1.1
 * @date		13.12.2017
 * @brief		connect interface with regulator
 * @copyright	Copyright (C) 2017 Storozhenko Roman
 *				All rights reserved
 *				This software may be modified and distributed under the terms
 *				of the BSD license.	 See the LICENSE file for details
 */
#ifndef uartTSK_H
#define uartTSK_H

/*!****************************************************************************
 * Include
 */
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "crc.h"
#include "pstypes.h"
#include "OSinit.h"
#include "systemTSK.h"
#include "uart.h"

/*!****************************************************************************
 * Define
 */
#define uartTskUse          		(uart1)
#define UART_TSK_QUEUE_COMMAND_LEN	(16)
#define UART_TSK_MAX_WAIT_ms    	(100)
#define UART_TSK_MAX_ERR 			(10)
#define UART_TSK_PERIOD				(15)	///< [ms]

/*!****************************************************************************
 * Enumeration
 */

/******************************************************************************
 * Typedef
 */
typedef enum {
	uartConnect, uartNoConnect, uartUndef,
} uartTskState_type;

typedef struct {
	volatile uint32_t normAnswer;
	volatile uint32_t noAnswer;
	volatile uint32_t errorAnswer;
	volatile uint32_t queueLen;
	volatile uartTskState_type state;
} uartTsk_type;


/*!****************************************************************************
 * Exported variables
 */
extern uartTsk_type uartTsk;

/*!****************************************************************************
 * Macro functions
 */

/******************************************************************************
 * Prototypes for the functions
 */
void uartTSK(void *pPrm);
uint8_t waitForTf(void);
uint8_t sendCommand(request_type command);

#endif //uartTSK_H
/***************** Copyright (C) Storozhenko Roman ******* END OF FILE *******/

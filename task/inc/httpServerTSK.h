/*!****************************************************************************
 * @file		httpServerTSK.h
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		06.09.2017
 * @copyright	GNU Lesser General Public License v3
 * @brief		HTTP Server
 */
#ifndef httpServerTSK_H
#define httpServerTSK_H

/*!****************************************************************************
 * Include
 */
#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/ip.h"
#include <stdio.h>
#include <string.h>
#include <task/inc/httpServerTSK.h>
#include "debugCore.h"
#include "gpio.h"
#include "uart.h"
#include "rtc.h"
#include "htmlPage.h"

/*!****************************************************************************
 * Define
 */

/*!****************************************************************************
 * Enumeration
 */

/*!****************************************************************************
 * Typedef
 */

/*!****************************************************************************
 * Exported variables
 */

/*!****************************************************************************
 * Macro functions
 */

/*!****************************************************************************
 * Function declaration
 */
void httpServerTSK(void *pPrm);

#endif //httpServerTSK__H
/*************** LGPL ************** END OF FILE *********** D_EL ************/

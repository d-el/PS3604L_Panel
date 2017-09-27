/*!****************************************************************************
 * @file		base.h
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		01.01.2015
 * @copyright	GNU Lesser General Public License v3
 * @brief		This task is base GUI
 */
#ifndef BASE_H
#define BASE_H

/*!****************************************************************************
 * Include
 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "OSinit.h"
#include "ui.h"
#include "pstypes.h"
#include "rtc.h"
#include "beep.h"
#include "key.h"
#include "enco.h"
#include "regulatorConnTSK.h"
#include "wlanTSK.h"
#include "sysTimeMeas.h"

/******************************************************************************
 * Define
 */
#define NPRESET             3
#define IDLE_TIME           (180000)       		//Время до заставки [ms]
#define BASE_TSK_PERIOD     100                  //[ms]

/*!****************************************************************************
 * Enumeration
 */

/******************************************************************************
 * Typedef
 */
typedef enum {
	VAR_VOLT = 0,
	VAR_CURR,
	VAR_MODE,
} baseVar_type;

typedef enum {
	baseImax = 0,
	baseILimitation,
	baseUnprotected,
} baseMode_type;

typedef struct {
	uint16_t u;
	uint16_t i;
	baseMode_type mode;
} baseSet_type;

typedef struct {
	baseSet_type set[NPRESET];
	uint8_t curPreSet;
} base_type;

/*!****************************************************************************
 * Exported variables
 */

extern base_type bs;

/******************************************************************************
 * Macro functions
 */

/*!****************************************************************************
 * Function declaration
 */
void baseTSK(void *pPrm);
void printStatusBar(void);

#endif //BASE_H
/*************** LGPL ************** END OF FILE *********** D_EL ************/

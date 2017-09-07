/******************************************************************************
 * @file			charge.h
 * @author		D_EL - Storozhenko Roman
 * @version      V1.0
 * @date			
 * @copyright GNU Public License
 */
#ifndef CHARGE_H
#define CHARGE_H

#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "OSinit.h"
#include "ui.h"
#include "pstypes.h"
#include "graphics.h"
#include "baseTSK.h"
#include "uartTSK.h"
#include "spfd54124b.h"
#include "key.h"
#include "enco.h"
#include "beep.h"

/******************************************************************************
 * User define
 */
#define CH_TSK_PERIOD   10      //[ms]

/******************************************************************************
 * User typedef
 */
typedef struct {
	uint16_t u;		//[mV]
	uint16_t i;		//[mA]
	uint16_t t;		//[minute]
	uint16_t mode;
} charge_type;

/******************************************************************************
 * User enum
 */
enum {
	C_VOLT = 0,
	C_CURR = 1,
	C_TIME = 2,
	C_MODE = 3,
};

enum {
	ch_modeTime = 0,
	ch_modeCurrent = 1,
};

/******************************************************************************
 * Extern viriables
 */
extern charge_type ch;

/******************************************************************************
 * Macro functions
 */

/******************************************************************************
 * Prototypes for the functions
 */
void chargeTSK(void *pPrm);

#endif //CHARGE_H
/*************** GNU GPL ************** END OF FILE ********* D_EL ***********/

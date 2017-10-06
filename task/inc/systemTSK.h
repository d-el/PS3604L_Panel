/*!****************************************************************************
 * @file		systemTSK.h
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		14.09.2015
 * @copyright	GNU Lesser General Public License v3
 * @brief		System control task
 */
#ifndef systemTSK_H
#define systemTSK_H

/*!****************************************************************************
 * Include
 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "OSinit.h"
#include "board.h"
#include "pstypes.h"
#include "prmSystem.h"
#include "wlanTSK.h"
#include "startupTSK.h"
#include "settingTSK.h"
#include "cube3dTSK.h"
#include "chargeTSK.h"
#include "httpServerTSK.h"
#include "pvd.h"
#include "net.h"
#include "pingService.h"
#include "sntp.h"
#include "assert.h"

/*!****************************************************************************
 * Define
 */
#define SYSTEM_TSK_PERIOD   (100)		///< [ms]
#define I_SHORT_CIRCUIT     (40000)   	///< [X_XXXX A]
#define MAX_VAL_U           (36000)   	///< [X_XXX V]
#define MAX_VAL_I           (40000)   	///< [X_XXXX A]
#define U_BIG_STEP          (1000)    	///< [X_XXX V]
#define I_BIG_STEP          (5000)    	///< [X_XXXX A]

/*!****************************************************************************
 * Enumeration
 */

/*!****************************************************************************
 * Typedef
 */
typedef enum {
	noneWindow,
	startupWindow,
	settingWindow,
	baseWindow,
	chargerWindow,
	cube3dWindow,
	bubblesWindow
} selWindow_type;

typedef struct {
	uint32_t 	ipadr;
	uint32_t 	netmask;
	uint32_t 	gateway;
	uint16_t 	lcdLight;		///< [X_X %]
} frontPanelSetting_type;

typedef struct {
	uint16_t sysSettingLoadDefault	:1;
	uint16_t userSettingLoadDefault	:1;
	uint16_t rtcOscillatorError		:1;
	uint16_t lanLink				:1;
	uint16_t lanActive				:1;
} frontPanelState_type;

typedef struct {
	transfer_type 			tf;					///< Общие данные
	frontPanelSetting_type 	fpSet;				///< Данные настройки передней панели
	frontPanelState_type	state;				///< Флаги состояния
	selWindow_type 			currentSelWindow;	///< Текущее окно GUI
} frontPanel_type;

/*!****************************************************************************
 * Exported variables
 */
extern frontPanel_type fp;

/*!****************************************************************************
 * Macro functions
 */

/*!****************************************************************************
 * Function declaration
 */
void selWindow(selWindow_type window);
void systemTSK(void *pPrm);
void netSettingUpdate(void);
void selWindow(selWindow_type window);
void shutdown(void);

#endif //systemTSK_H
/*************** LGPL ************** END OF FILE *********** D_EL ************/

/*!****************************************************************************
 * @file		systemTSK.h
 * @author		d_el
 * @version		V1.0
 * @date		14.09.2015
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		System control task
 */
#ifndef systemTSK_H
#define systemTSK_H

/*!****************************************************************************
 * Include
 */
#include <stdint.h>

/*!****************************************************************************
 * Define
 */
#define SYSTEM_TSK_PERIOD   (20)		///< [ms]
#define LINK_DETECT_PERIOD  (500)		///< [ms]
#define I_SHORT_CIRCUIT     (4000000)	///< [X_XXXXXX A]

/*!****************************************************************************
 * Enumeration
 */

/*!****************************************************************************
 * Typedef
 */
typedef enum {
	noneWindow,   //!< noneWindow
	startupWindow,//!< startupWindow
	settingWindow,//!< settingWindow
	baseWindow,   //!< baseWindow
	chargerWindow,//!< chargerWindow
} selWindow_type;

typedef struct {
	uint16_t rtcOscillatorError		:1;
	uint16_t mainOscillatorError	:1;

	uint16_t sysSettingLoadDefault	:1;
	uint16_t userSettingLoadDefault	:1;

	uint16_t lanLink				:1;
	uint16_t httpactiv				:1;
} frontPanelState_type;

typedef struct {
	frontPanelState_type	state;
	selWindow_type 			currentSelWindow;
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
void systemTSK(void *pPrm);
void netSettingUpdate(void);
void selWindow(selWindow_type window);
void timezoneUpdate(int8_t timezone);
void OSinit(void);
void saveparametersSystem(void);
void saveparametersUser(void);

#endif //systemTSK_H
/******************************** END OF FILE ********************************/

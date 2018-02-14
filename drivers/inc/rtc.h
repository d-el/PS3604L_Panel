/*!****************************************************************************
 * @file		rtc.h
 * @author		d_el
 * @version		V1.1
 * @date		13.02.2018
 * @brief		Driver RTC module
 * @copyright	Copyright (C) 2018 Storozhenko Roman
 *				All rights reserved
 *				This software may be modified and distributed under the terms
 *				of the BSD license.	 See the LICENSE file for details
 */
#ifndef rtc_H
#define rtc_H

/*!****************************************************************************
 * Include
 */
#include "time.h"
#include "stdint.h"

/*!****************************************************************************
 * Define
 */

/*!****************************************************************************
 * Typedef
 */
typedef enum {
	rtc_Ok,
	rtc_wasOn,
	rtc_initOk,
	rtc_setOk,
	rtc_error,
} rtcStatus_type;

/*!****************************************************************************
 * Enumeration
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
rtcStatus_type rtc_init(void);
rtcStatus_type rtc_setTime(const struct tm *t);
rtcStatus_type rtc_setTimeUnix(time_t time);
rtcStatus_type rtc_setTimeUnixLocal(time_t time);

#endif //rtc_H
/***************** Copyright (C) Storozhenko Roman ******* END OF FILE *******/

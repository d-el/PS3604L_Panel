/*!****************************************************************************
 * @file		rtc.h
 * @author		d_el
 * @version		V1.1
 * @date		13.02.2018
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		Driver RTC module
 */
#ifndef rtc_H
#define rtc_H

#ifdef __cplusplus
extern "C" {
#endif

/*!****************************************************************************
 * Include
 */
#include "time.h"
#include "stdint.h"

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
 * Function declaration
 */
rtcStatus_type rtc_init(void);
rtcStatus_type rtc_setTime(const struct tm *t);
rtcStatus_type rtc_setTimeUnix(time_t time);
rtcStatus_type rtc_setTimeUnixLocal(time_t time);

#ifdef __cplusplus
}
#endif

#endif //rtc_H
/******************************** END OF FILE ********************************/

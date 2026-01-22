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
#include <time.h>
#include <stdint.h>
#include <stdbool.h>

/*!****************************************************************************
 * Function declaration
 */
bool rtc_init(void);
bool rtc_setTime(const struct tm *t);
bool rtc_setTimeUnix(time_t time);
bool rtc_setTimeUnixLocal(time_t time);

#ifdef __cplusplus
}
#endif

#endif //rtc_H
/******************************** END OF FILE ********************************/

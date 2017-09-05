/*!****************************************************************************
 * @file		storage.h
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		05.12.2015
 * @copyright	GNU Lesser General Public License v3
 * @brief		RTC driver
 */
#ifndef rtc_H
#define rtc_H

/*!****************************************************************************
 * Include
 */
#include "stm32f4xx.h"
#include "stdint.h"
//#include "specificMath.h"
#include <time.h>

/*!****************************************************************************
 * Define
 */
#define RTC_LSE_TIMEOUT	(10000)

/*!****************************************************************************
 * Typedef
 */
typedef enum {
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
extern time_t unixTime;
extern struct tm timeStrct;

/*!****************************************************************************
 * Macro functions
 */

/*!****************************************************************************
 * Function declaration
 */
rtcStatus_type rtc_init(void);
void rtc_getTime(struct tm *time);
rtcStatus_type rtc_setTime(const struct tm *t);
rtcStatus_type rtc_setTimeUnix(time_t time);
void rtc_writeToBkp(uint32_t val);
uint32_t rtc_ReadFromBkp(void);

#endif //rtc_H
/*************** LGPL ************** END OF FILE *********** D_EL ************/

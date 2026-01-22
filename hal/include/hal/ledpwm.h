/*!****************************************************************************
 * @file		ledpwm.h
 * @author		d_el
 * @version		V1.0
 * @date		27.12.2015
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		pwm for lcd led
 */
#ifndef ledpwm_H
#define ledpwm_H

#ifdef __cplusplus
extern "C" {
#endif

/*!****************************************************************************
 * Include
 */
#include "stm32f4xx.h"

/*!****************************************************************************
 * Prototypes for the functions
 */
void ledPwm_init(void);
void setLcdBrightness(uint16_t percent);	// [XXX_X %]

#ifdef __cplusplus
}
#endif

#endif //ledpwm_H
/******************************** END OF FILE ********************************/

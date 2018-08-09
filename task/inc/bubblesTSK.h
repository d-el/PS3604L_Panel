/*!****************************************************************************
 * @file		bubbles.h
 * @author		Matthew Weathers + D_EL - Storozhenko Roman
 * @version		V1.0
 * @date		14.09.2004
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 */
#ifndef BUBBLES_H
#define BUBBLES_H

/*!****************************************************************************
 * Include
 */
#include "stdint.h"

/*!****************************************************************************
 * User define
 */

/*!****************************************************************************
 * Typedef
 */
typedef struct {
	int16_t color;
	int16_t radius;
	int16_t speedx;
	int16_t speedy;
	int16_t old_xcenter;
	int16_t old_ycenter;
	int16_t new_xcenter;
	int16_t new_ycenter;
} ball_type;

/*!****************************************************************************
 * Function prototype
 */
void bubblesTSK(void *pPrm);

#endif //BUBBLES_H
/******************************** END OF FILE ********************************/

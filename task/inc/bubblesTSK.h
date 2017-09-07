/*!****************************************************************************
 * @file			bubbles.h
 * @author		Matthew Weathers + D_EL - Storozhenko Roman
 * @version      V1.0
 * @date			14.09.2004
 * @Up date:     19.08.2014
 * @copyright 	GNU Public License
 */
#ifndef BUBBLES_H
#define BUBBLES_H

/*!****************************************************************************
 * Include
 */
#include "stdlib.h"
#include "graphics.h"
#include "OSinit.h"
#include "ui.h"
#include "display.h"
#include "beep.h"

/*!****************************************************************************
 * User define
 */
#define xSize           160
#define ySize           128
#define CountOfBalls    5

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
/*************** GNU GPL ************** END OF FILE ********* D_EL ***********/

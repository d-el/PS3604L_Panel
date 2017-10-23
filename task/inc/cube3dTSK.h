﻿/*!****************************************************************************
 * @file    cube3d.c
 * @author
 * @version V1.0
 * @date    31-October-2013
 * @brief
 */
#ifndef CUBE3D_H
#define CUBE3D_H

/*!****************************************************************************
 * Include
 */
#include "math.h"
#include "ui.h"
#include "baseTSK.h"
#include "display.h"
#include "graphics.h"
#include "rtc.h"
#include "beep.h"
#include "IQmathLib.h"

/*!****************************************************************************
 * User define
 */

/*!****************************************************************************
 * User enum
 */

/*!****************************************************************************
 * User typedef
 */
typedef struct {
	uint8_t x1;
	uint8_t x2;
	uint8_t y1;
	uint8_t y2;
} pairsOfPoints_type;

/*!****************************************************************************
 * External variables
 */

/*!****************************************************************************
 * Macro functions
 */

/******************************************************************************
 * Prototypes for the functions
 */
void cube3dTSK(void *pPrm);

#endif //CUBE3D_H

/******************* (C) COPYRIGHT ***************** END OF FILE ********* D_EL *****/

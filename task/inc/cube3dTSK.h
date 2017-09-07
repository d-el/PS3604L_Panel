/*!****************************************************************************
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
#define CUBE_SIZE   33      //Длина ребра / 2
#define DOTS_COUNT  8       //Всего вершин
#define MESH_COUNT  12      //Количество ребер
#define LCD_X_SIZE  160     //Разрешение по горизонтали
#define LCD_Y_SIZE  100		//Разрешение по вертикали

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
 * Extern viriables
 */

/*!****************************************************************************
 * Macro functions
 */

/******************************************************************************
 * Prototypes for the functions
 */
void cube3dTSK(void *pPrm);
float cos_(unsigned char angle);
float sin_(unsigned char angle);

#endif //CUBE3D_H

/******************* (C) COPYRIGHT ***************** END OF FILE ********* D_EL *****/

/*!****************************************************************************
 * @file		cube3d.c
 * @author		Storozhenko Roman
 * @version		V1.0
 * @date		31.10.2013
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 */

/*!****************************************************************************
 * Include
 */
#include "math.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ui.h"
#include "display.h"
#include "graphics.h"
#include "rtc.h"
#include "beep.h"
#include "key.h"
#include "enco.h"
#include "IQmathLib.h"
#include "systemTSK.h"
#include "cube3dTSK.h"

/*!****************************************************************************
 * User define
 */
#define CUBE_SIZE	33		//Длина ребра / 2
#define DOTS_COUNT	8		//Всего вершин
#define MESH_COUNT	12		//Количество ребер
#define LCD_X_SIZE	160		//Разрешение по горизонтали
#define LCD_Y_SIZE	100		//Разрешение по вертикали

/*!****************************************************************************
 * MEMORY
 */
uint16_t colors[12] = {	//Цвета граней
		red, green, blue, red, green, blue, red, green, blue, red, green, blue, };

/** Массивы содержат номера начальных (s1) и конечных (f1) точек,
 *	по ним соединяем все и рисуем ребра(сетку)
 */
const uint8_t s1[MESH_COUNT] = { 0, 1, 2, 3, 0, 1, 2, 3, 4, 5, 6, 7 };
const uint8_t f1[MESH_COUNT] = { 1, 2, 3, 0, 4, 5, 6, 7, 5, 6, 7, 4 };

/*!****************************************************************************
 * Local functions declaration
 */

/*!****************************************************************************
 * @brief
 */
void cube3dTSK(void *pPrm){
	(void)pPrm;
	/** углы поворота по осям, от 0 до 63 включительно,
	 *	это полный оборот на 360
	 */
	_iq dir_x = 0;
	_iq dir_y = 0;
	_iq dir_z = 0;
	//точки, лучше поменять на long
	int32_t Xa[DOTS_COUNT], Ya[DOTS_COUNT], Za[DOTS_COUNT];
	uint8_t i;
	_iq angle;									//Буфер для угла
	int32_t x1, y1, z1;							//Координаты точек от центра объекта
	float f;									//Временная переменная
	uint8_t x2d[MESH_COUNT], y2d[MESH_COUNT];	//”Плоские” точки
	pairsOfPoints_type pairsOfPoints[MESH_COUNT];

	uint16_t  old_val_encoder = enGeReg();
	disp_fillScreen(black);
	disp_setColor(black, white);

	while(1){
		//Расставляем точки модели, считаем что центр куба
		//совпадает с центром координат
		Xa[0] = -CUBE_SIZE;
		Ya[0] = CUBE_SIZE;
		Za[0] = CUBE_SIZE;
		Xa[1] = CUBE_SIZE;
		Ya[1] = CUBE_SIZE;
		Za[1] = CUBE_SIZE;
		Xa[2] = CUBE_SIZE;
		Ya[2] = -CUBE_SIZE;
		Za[2] = CUBE_SIZE;
		Xa[3] = -CUBE_SIZE;
		Ya[3] = -CUBE_SIZE;
		Za[3] = CUBE_SIZE;
		Xa[4] = -CUBE_SIZE;
		Ya[4] = CUBE_SIZE;
		Za[4] = -CUBE_SIZE;
		Xa[5] = CUBE_SIZE;
		Ya[5] = CUBE_SIZE;
		Za[5] = -CUBE_SIZE;
		Xa[6] = CUBE_SIZE;
		Ya[6] = -CUBE_SIZE;
		Za[6] = -CUBE_SIZE;
		Xa[7] = -CUBE_SIZE;
		Ya[7] = -CUBE_SIZE;
		Za[7] = -CUBE_SIZE;

		//Вращаем наши точки, фактически матрицы вращения упрощенные
		for(i = 0; i < DOTS_COUNT; i++){ //по X
			y1 = Ya[i];
			z1 = Za[i];
			angle = dir_x;
			Ya[i] = (int32_t) (_IQcos(angle) * y1 - _IQsin(angle) * z1) / _IQ(1);
			Za[i] = (int32_t) (_IQcos(angle) * z1 + _IQsin(angle) * y1) / _IQ(1);
		}

		for(i = 0; i < DOTS_COUNT; i++){ //по Y
			x1 = Xa[i];
			z1 = Za[i];
			angle = dir_y;
			Xa[i] = (int32_t) (_IQcos(angle) * x1 + _IQsin(angle) * z1) / _IQ(1);
			Za[i] = (int32_t) (-_IQsin(angle) * x1 + _IQcos(angle) * z1) / _IQ(1);
		}

		for(i = 0; i < DOTS_COUNT; i++){ //и Z незабыть!
			x1 = Xa[i];
			y1 = Ya[i];
			angle = dir_z;
			Xa[i] = (int32_t) (_IQcos(angle) * x1 - _IQsin(angle) * y1) / _IQ(1);
			Ya[i] = (int32_t) (_IQcos(angle) * y1 + _IQsin(angle) * x1) / _IQ(1);
		}

		//Трансформация координат вершин в экранные
		for(i = 0; i < DOTS_COUNT; i++){
			//1000 и 1200 определяют расстояние от	объекта до камеры и
			f = 1000 / (1100 - (float) Za[i]);
			// рисуем объект с центром в по-центре экрана
			x2d[i] = (uint8_t) ((f * (float) Xa[i]) + LCD_X_SIZE / 2);
			y2d[i] = (uint8_t) ((f * (float) Ya[i]) + LCD_Y_SIZE / 2);
		}

		//Рисуем ребра/сетку
		for(i = 0; i < MESH_COUNT; i++){
			grf_line(pairsOfPoints[i].x1, pairsOfPoints[i].y1,		 //Рисуем линию черным цветом
					pairsOfPoints[i].x2, pairsOfPoints[i].y2, black);

			pairsOfPoints[i].x1 = x2d[s1[i]];
			pairsOfPoints[i].y1 = y2d[s1[i]];
			pairsOfPoints[i].x2 = x2d[f1[i]];
			pairsOfPoints[i].y2 = y2d[f1[i]];

			grf_line(pairsOfPoints[i].x1, pairsOfPoints[i].y1,		 //Рисуем линию
					pairsOfPoints[i].x2, pairsOfPoints[i].y2, colors[i]);
		}

		//Calculate new angles
		static _iq alpha = 0;
		alpha += _IQ((2 * M_PI) / 64);
		if(alpha > _IQ(M_PI)){
			alpha -= _IQ(2 * M_PI);
		}
		dir_x = alpha;
		dir_y = alpha;
		dir_z = alpha;

		//Выходим если нажата кнопка или повернут энкодер
		if((keyProc() != 0) || (old_val_encoder != enGeReg())){
			BeepTime(ui.beep.key.time, ui.beep.key.freq);
			selWindow(baseWindow);
		}

		//Print time
		char str[10];
		struct tm tm;
		time_t unixTime = time(NULL);
		localtime_r(&unixTime, &tm);
		strftime(str, sizeof(str), "%H:%M:%S", &tm);
		disp_putStr(48, 110, &arial, 0, str);

		vTaskDelay(pdMS_TO_TICKS(40));
	}
}

/******************* (C) COPYRIGHT ***************** END OF FILE ********* D_EL *****/

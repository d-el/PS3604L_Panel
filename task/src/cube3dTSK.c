/*!****************************************************************************
 * @file    cube3d.c
 * @author
 * @version V1.0
 * @date    31-October-2013
 * @brief
 */

/*!****************************************************************************
 * Include
 */
#include "cube3dTSK.h"

/*!****************************************************************************
 * MEMORY
 */
uint16_t colors[12] = {	//Цвета граней
		red, green, blue, red, green, blue, red, green, blue, red, green, blue, };

const float cospi[] = { 1,  //0
		0.99518473, 0.98078528, 0.95694034, 0.92387953, 0.88192126, 0.83146961, 0.77301045, //7
		0.70710678, 0.63439328, 0.55557023, 0.47139674, 0.38268343, 0.29028468, 0.19509032, 0.09801714, //15
		0  //16
		};

/** массивы содержат номера начальных (s1) и конечных (f1) точек,
 *  по ним соединяем все и рисуем ребра(сетку)
 */
const uint8_t s1[MESH_COUNT] = { 0, 1, 2, 3, 0, 1, 2, 3, 4, 5, 6, 7 };

const uint8_t f1[MESH_COUNT] = { 1, 2, 3, 0, 4, 5, 6, 7, 5, 6, 7, 4 };

/*!****************************************************************************
 * @brief
 */
float cos_(uint8_t angle){
	return _IQcos(angle);
}

/*!****************************************************************************
 * @brief
 */
float sin_(uint8_t angle){
	uint8_t a1, a2;
	a1 = angle & 0x0f;
	a2 = angle >> 4;
	if(a2 == 0)
		return (float) (cospi[16 - a1]);
	if(a2 == 1)
		return (float) (cospi[a1]);
	if(a2 == 2)
		return (float) (cospi[16 - a1] * (-1));
	if(a2 == 3)
		return (float) (cospi[a1] * (-1));
	return 0;
}

/*!****************************************************************************
 * @brief
 */
void cube3dTSK(void *pPrm){
	uint16_t old_val_encoder;
	/** углы поворота по осям, от 0 до 63 включительно,
	 *  это полный оборот на 360
	 */
	_iq dir_x = 0;
	_iq dir_y = 0;
	_iq dir_z = 0;
	//точки, лучше поменять на long
	int32_t Xa[DOTS_COUNT], Ya[DOTS_COUNT], Za[DOTS_COUNT];
	uint8_t i;
	_iq angle;                              //Буфер для угла
	int32_t x1, y1, z1;                           //Координаты точек от центра объекта
	float f;                                  //Временная переменная
	char x2d[MESH_COUNT], y2d[MESH_COUNT];   //”Плоские” точки
	pairsOfPoints_type pairsOfPoints[MESH_COUNT];

	old_val_encoder = enGeReg();
	lcd_fillScreen(black);
	lcd_setColor(black, white);

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
			//1000 и 1200 определяют расстояние от  объекта до камеры и
			f = 1000 / (1100 - (float) Za[i]);
			// рисуем объект с центром в по-центре экрана
			x2d[i] = (uint8_t) ((f * (float) Xa[i]) + LCD_X_SIZE / 2);
			y2d[i] = (uint8_t) ((f * (float) Ya[i]) + LCD_Y_SIZE / 2);
		}

		//Рисуем ребра/сетку
		for(i = 0; i < MESH_COUNT; i++){
			grf_line(pairsOfPoints[i].x1, pairsOfPoints[i].y1,       //Рисуем линию черным цветом
					pairsOfPoints[i].x2, pairsOfPoints[i].y2, black);

			pairsOfPoints[i].x1 = x2d[s1[i]];
			pairsOfPoints[i].y1 = y2d[s1[i]];
			pairsOfPoints[i].x2 = x2d[f1[i]];
			pairsOfPoints[i].y2 = y2d[f1[i]];

			grf_line(pairsOfPoints[i].x1, pairsOfPoints[i].y1,       //Рисуем линию
					pairsOfPoints[i].x2, pairsOfPoints[i].y2, colors[i]);
		}

		//Устанавливаем новые углы
		static _iq alpha = 0;
		alpha += _IQ((2 * M_PI) / 64);
		if(alpha > _IQ(M_PI)){
			alpha -= _IQ(M_PI);
		}else if(alpha < _IQ(-M_PI)){
			alpha += _IQ(M_PI);
		}
		dir_x = dir_y = dir_z = alpha;

		if((keyProc() != 0) || (old_val_encoder != enGeReg())){
			BeepTime(ui.beep.key.time, ui.beep.key.freq);
			selWindow(baseWindow);
		}

		//Печать времени
		rtc_getTime(&timeStrct);
		strftime(str, sizeof(str), "%H:%M:%S", &timeStrct);
		lcd_putStr(48, 110, &arial, 0, str);

		vTaskDelay(40);
	}
}

/******************* (C) COPYRIGHT ***************** END OF FILE ********* D_EL *****/

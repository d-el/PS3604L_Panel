/*!****************************************************************************
 * @file		bubbles.c
 * @author		D_EL - Storozhenko Roman
 * @version		V1.0
 * @date		19.08.2014
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 */

/*!****************************************************************************
 * Include
 */
#include <FreeRTOS.h>
#include <task.h>
#include "graphics.h"
#include "ui.h"
#include "display.h"
#include "beep.h"
#include "rtc.h"
#include "enco.h"
#include "key.h"
#include "systemTSK.h"
#include "bubblesTSK.h"

/*!****************************************************************************
 * User define
 */
#define xSize           160
#define ySize           110
#define CountOfBalls    5

/*!****************************************************************************
 * MEMORY
 */
ball_type	ball[CountOfBalls];

/*!****************************************************************************
 * Local functions declaration
 */
void initialize(void);
void physics(void);
void moveball(void);

/*!****************************************************************************
 *
 */
void bubblesTSK(void *pPrm){
	(void)pPrm;
	uint16_t  old_val_encoder = enco_read();
	disp_fillScreen(black);
	disp_setColor(black, white);
	initialize();

	while(1){
		physics();
		moveball();

		//Выходим если нажата кнопка или повернут энкодер
		if((keyProc() != 0) || (old_val_encoder != enco_read())){
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
		disp_flushfill(ui.color.background);
		vTaskDelay(pdMS_TO_TICKS(60));
	}
}

/*!****************************************************************************
 *
 */
void paint_ball(int16_t x, int16_t y, int16_t r, uint16_t color){
	grf_circle(x, y, r, color);
}

/*!****************************************************************************
 *
 */
void initialize(void){ // Initialize the game
	ball[0].color = red;
	ball[0].radius = 5;
	ball[0].speedx = 3 + grf_rand(2); //Generate Random X direction.
	ball[0].speedy = 2 + grf_rand(2); //Generate Random Y direction.
	ball[0].new_xcenter = xSize / 2;
	ball[0].new_ycenter = ySize / 2;

	ball[1].color = green;
	ball[1].radius = 5;
	ball[1].speedx = 3 + grf_rand(2); // Generate Random X direction.
	ball[1].speedy = -2 + grf_rand(2); // Generate Random Y direction.
	ball[1].new_xcenter = xSize / 2 + 15;
	ball[1].new_ycenter = ySize / 2 + 15;

	ball[2].color = blue;
	ball[2].radius = 5;
	ball[2].speedx = -4 + grf_rand(2); // Generate Random X direction.
	ball[2].speedy = 3 + grf_rand(2); // Generate Random Y direction.
	ball[2].new_xcenter = xSize / 2 - 15;
	ball[2].new_ycenter = ySize / 2 - 15;

	ball[3].color = white;
	ball[3].radius = 5;
	ball[3].speedx = -3 + grf_rand(2); // Generate Random X direction.
	ball[3].speedy = 2 + grf_rand(2); // Generate Random Y direction.
	ball[3].new_xcenter = xSize / 2 + 10;
	ball[3].new_ycenter = ySize / 2 + 10;

	ball[4].color = sky;
	ball[4].radius = 5;
	ball[4].speedx = 2 + grf_rand(2); // Generate Random X direction.
	ball[4].speedy = -4 + grf_rand(2); // Generate Random Y direction.
	ball[4].new_xcenter = xSize / 2 - 10;
	ball[4].new_ycenter = ySize / 2 - 10;
}

/*!****************************************************************************
 *
 */
void moveball(void){
	uint8_t i;
	for(i = 0; i < CountOfBalls; i++){
		ball[i].new_xcenter = ball[i].new_xcenter + ball[i].speedx;
		ball[i].new_ycenter = ball[i].new_ycenter + ball[i].speedy;
		paint_ball(ball[i].new_xcenter, ball[i].new_ycenter, ball[i].radius, ball[i].color);
	}
}

/*!****************************************************************************
 *
 */
void physics(void){
	//uint8_t Sumradius;
	//signed int yDistance, xDistance, yAbsDistance, xAbsDistance;

	for(uint8_t i = 0; i < CountOfBalls; i++){
		//From Top
		if((ball[i].new_ycenter - ball[i].radius + ball[i].speedy) <=  0){
			ball[i].speedy = -ball[i].speedy; // Reflect From Top
		}
		//From Bottom
		if((ball[i].new_ycenter + ball[i].radius + ball[i].speedy) >= (ySize - 1)){
			ball[i].speedy = -ball[i].speedy; // Reflect From Bottom
		}
		//From Left
		if((ball[i].new_xcenter - ball[i].radius + ball[i].speedx) <= 0){
			ball[i].speedx = -ball[i].speedx;
		}
		//From Right
		if((ball[i].new_xcenter + ball[i].radius + ball[i].speedx) >= (xSize - 1)){
			ball[i].speedx = -ball[i].speedx;
		}
	}

	/*for(j = 0; j < CountOfBalls; j++){

		for(k = j + 1; k < CountOfBalls; k++){

			yDistance = ball[j].new_ycenter + ball[j].speedy - ball[k].new_ycenter + ball[k].speedy;
			yAbsDistance = abs(yDistance);

			xDistance = ball[j].new_xcenter + ball[j].speedx - ball[k].new_xcenter + ball[k].speedx;
			xAbsDistance = abs(xDistance);

			Sumradius = ball[j].radius + ball[k].radius;

			if((yAbsDistance <= Sumradius) || (xAbsDistance <= Sumradius)){
				//clash
				//BeepTime(20);
				ball[j].speedx = -ball[j].speedx;
				ball[j].speedy = -ball[j].speedy;

				ball[k].speedx = -ball[k].speedx;
				ball[k].speedy = -ball[k].speedy;
			}
		}
	}*/
}

/******************* (C) COPYRIGHT ***************** END OF FILE ********* D_EL *****/

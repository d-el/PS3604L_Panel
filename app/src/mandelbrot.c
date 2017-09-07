/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2011 Stephen Caudle <scaudle@doceme.com>
 * Copyright (C) 2012 Daniel Serpell <daniel.serpell@gmail.com>
 * Copyright (C) 2015 Piotr Esden-Tempski <piotr@esden.net>
 * Copyright (C) 2015 Chuck McManis <cmcmanis@mcmanis.com>
 * Copyright (C) 2015 Storozhenko Roman
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

/*!****************************************************************************
 * Include
 */
#include "mandelbrot.h"

uint16_t lcd_colors[] = { 0x0, 0x1f00, 0x00f8, 0xe007, 0xff07, 0x1ff8, 0xe0ff, 0xffff, 0xc339, 0x1f00 >> 1, 0x00f8 >> 1, 0xe007 >> 1, 0xff07 >> 1, 0x1ff8 >> 1,
		0xe0ff >> 1, 0xffff >> 1, 0xc339 >> 1, 0x1f00 << 1, 0x00f8 << 1, 0x6007 << 1, 0x6f07 << 1, 0x1ff8 << 1, 0x60ff << 1, 0x6fff << 1, 0x4339 << 1, 0x1f00
				^ 0x6ac9, 0x00f8 ^ 0x6ac9, 0xe007 ^ 0x6ac9, 0xff07 ^ 0x6ac9, 0x1ff8 ^ 0x6ac9, 0xe0ff ^ 0x6ac9, 0xffff ^ 0x6ac9, 0xc339 ^ 0x6ac9, 0, 0, 0, 0, 0 };

void iq_mandel(_iq, _iq, _iq);
static int iq_iterate(_iq, _iq);

/*!****************************************************************************
 *  Main mandelbrot calculation
 */
static int iq_iterate(_iq px, _iq py){
	int it = 0;
	
	_iq x = 0;
	_iq y = 0;
	
	while(it < max_iter){
		_iq nx = _IQmpy(x, x);
		_iq ny = _IQmpy(y, y);
		if((nx + ny) > _IQ(4.0f)){
			return it;
		}
		
		y = 2 * _IQmpy(x, y) + py;
		x = nx - ny + px;
		
		it++;
	}
	return 0;
}

/*!****************************************************************************
 *
 */
void iq_mandel(_iq cx, _iq cy, _iq scale){
	int x, y;
	int change = 0;
	
	for(y = -DISP_H / 2; y < DISP_H / 2; y++){
		for(x = -DISP_W / 2; x < DISP_W / 2; x++){
			int i = iq_iterate(cx + x * scale, cy + y * scale);
			if(i >= max_iter){
				i = max_iter;
			}else{
				change++;
			}
			lcd_setPixel(x + DISP_W / 2, y + DISP_H / 2, lcd_colors[i]);
		}
	}
}

/*!****************************************************************************
 *
 */
int iq_mandelbrot(void){
	int gen = 0;
	_iq scale = _IQ(0.05f);
	_iq center_x = _IQ(-0.5f);
	_iq center_y = _IQ(0.0f);
	
	while(1){
		iq_mandel(center_x, center_y, scale); /* draw mandelbrot */
		
		//center_x += 0.3f * scale;
		center_x += _IQmpy(scale, _IQ(-1.4f));
		//center_y += 0.505f * scale;
		center_y += _IQmpy(scale, _IQ(0.68f));
		//scale	*= 0.875f;
		scale = _IQmpy(scale, _IQ(0.875f));
		
		gen++;
		if(gen > 75){
			scale = _IQ(0.05f) + (rand() % 200000);
			center_x = _IQ(-0.5f) + (rand() % 200000);
			center_y = _IQ(0.0f) + (rand() % 200000);
			gen = 0;
		}
	}
}

/*************** GNU GPL ************** END OF FILE ********* D_EL ***********/

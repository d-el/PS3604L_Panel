/*!****************************************************************************
 * @file		crc32.h
 * @author		d_el
 * @version		V1.0
 * @date		Apr 27, 2020
 * @brief
 * @copyright	Copyright (C) 2017 Storozhenko Roman
 *				All rights reserved
 *				This software may be modified and distributed under the terms
 *				of the BSD license.	 See the LICENSE file for details
 */

#ifndef crc32_H
#define crc32_H

/*!****************************************************************************
 * Include
 */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "stm32f4xx.h"

/*!****************************************************************************
 * Define
 */

/*!****************************************************************************
 * Enumeration
 */

/*!****************************************************************************
 * Typedef
 */

/*!****************************************************************************
 * Exported variables
 */

/*!****************************************************************************
 * Macro functions
 */
static inline void crc32_reset(void){
	CRC->CR = CRC_CR_RESET; // Reset CRC generator
	__DSB();
}

static inline void crc32_init(void){
	RCC->AHB1ENR |= RCC_AHB1ENR_CRCEN;
	RCC->AHB1RSTR |= RCC_AHB1RSTR_CRCRST;
	RCC->AHB1RSTR &= ~RCC_AHB1RSTR_CRCRST;
}

static inline uint32_t crc32_CalcCRC(uint32_t data){
	CRC->DR = data;
	return CRC->DR;
}

/*!****************************************************************************
 * Algorithm	Poly		Init		RefIn	RefOut	XorOut
 * CRC-32		0x04C11DB7	0xFFFFFFFF	true	true	0xFFFFFFFF
 */
static inline uint32_t crc32_posix(const void *buf, size_t len, bool clear){
	uint32_t *p = (uint32_t*) buf;
	uint32_t crc, crc_reg;
	if(clear)
		crc32_reset();
	if(len >= 4){
		while(len >= 4){
			crc_reg = crc32_CalcCRC(__RBIT(*p++));
			len -= 4;
		}
	}else{
		crc = 0xFFFFFFFF;
		crc_reg = crc32_CalcCRC(0xEBABAB);
	}
	crc = __RBIT(crc_reg);
	if(len){
		crc32_CalcCRC(crc_reg);
		switch(len) {
				case 1:
				crc_reg = crc32_CalcCRC(__RBIT((*p & 0xFF) ^ crc) >> 24);
				crc = ( crc >> 8 ) ^ __RBIT(crc_reg);
				break;
				case 2:
				crc_reg = crc32_CalcCRC(__RBIT((*p & 0xFFFF) ^ crc) >> 16);
				crc = ( crc >> 16 ) ^ __RBIT(crc_reg);
				break;
				case 3:
				crc_reg = crc32_CalcCRC(__RBIT((*p & 0xFFFFFF) ^ crc) >> 8);
				crc = ( crc >> 24 ) ^ __RBIT(crc_reg);
				break;
		}
	}
	return ~crc;
}

#endif //crc32_H
/******************************** END OF FILE ********************************/

/*!****************************************************************************
 * @file		flash.c
 * @author		Storozhenko Roman - D_EL
 * @version		V1.1
 * @date		17.08.2023
 * @copyright	The MIT License (MIT). Copyright (c) 2023 Storozhenko Roman
 */

/*!****************************************************************************
* Include
*/
#include <string.h>
#include "stm32f4xx.h"
#include "flash.h"

/*!****************************************************************************
* Memory
*/

#ifndef FLASH_KEY1
	#define FLASH_KEY1               ((uint32_t)0x45670123)
#endif

#ifndef FLASH_KEY2
	#define FLASH_KEY2               ((uint32_t)0xCDEF89AB)
#endif

/*!****************************************************************************
* Memory
*/

/*!****************************************************************************
* @brief    Unlock flash
*/
void flash_unlock(void){
	FLASH->KEYR = FLASH_KEY1;
	FLASH->KEYR = FLASH_KEY2;
}

/*!****************************************************************************
* @brief    Lock flash
*/
void flash_lock(void){
	FLASH->CR |= FLASH_CR_LOCK;
}

/*!****************************************************************************
 * @brief    Returned not 0 if flash busy
 */
uint32_t flash_busy(void){
	return (FLASH->SR & FLASH_SR_BSY);
}

/*!****************************************************************************
* @brief    Erase all pages
*/
void flash_eraseAllPages(void){
	FLASH->CR |= FLASH_CR_MER;
	FLASH->CR |= FLASH_CR_STRT;
	while(flash_busy());
	FLASH->CR &= FLASH_CR_MER;
}

/*!****************************************************************************
* @brief    	Erase sector
* @param[in]    sectorNumber
*/
bool flash_erasePage(void *address){
	FLASH->CR = FLASH_CR_SER;

	intptr_t addr = (intptr_t)address;
	uint32_t sectorNumber = 0;
	if(addr <= 0x0800FFFF){
		sectorNumber = (addr - 0x08000000) / (16 * 1024);
	}else if(addr >= 0x08010000 && addr <= 0x0801FFFF){
		sectorNumber = 4;
	}else{
		sectorNumber = 5 + (addr - 0x08020000) / (128 * 1024);
	}

	FLASH->CR |= sectorNumber << FLASH_CR_SNB_Pos;
	FLASH->CR |= FLASH_CR_STRT;
	while(flash_busy());
	FLASH->CR &= ~(FLASH_CR_SER | FLASH_CR_SNB);
	return true;
}

/*!****************************************************************************
* @brief    Write data
* @param    dst[in] - destination
* @param    src[in] - source
* @param    num[in] - number byte
*/
bool flash_write(void *dst, void *src, size_t num){
	uint8_t *pRd = (uint8_t*)src;
	uint8_t *pWr = (uint8_t*)dst;
	uint8_t *pEnd = pWr + num;

	FLASH->CR = FLASH_CR_PG;
	while(flash_busy());

	while(pWr < pEnd){
		*pWr++ = *pRd++;
		__DSB();
		while(flash_busy());
	}

	FLASH->CR &= ~FLASH_CR_PG;
	while(flash_busy());

	return true;
}

/*!****************************************************************************
* @brief    Write data
* @param    dst[in] - destination
* @param    src[in] - source
* @param    num[in] - number HalfWord (2 byte)
*/
bool flash_writeHalfWord(void *dst, void *src, size_t num){
	uint8_t *pRd = (uint8_t*)src;
	uint8_t *pWr = (uint8_t*)dst;
	uint8_t *pEnd = pWr + num;

	FLASH->CR = FLASH_CR_PG | FLASH_CR_PSIZE_0;
	while(flash_busy());

	while(pWr < pEnd){
		*pWr++ = *pRd++;
		__DSB();
		while(flash_busy());
	}

	FLASH->CR &= ~FLASH_CR_PG;
	while(flash_busy());

	return true;
}

/******************************** END OF FILE ********************************/

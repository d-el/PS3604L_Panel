/*!****************************************************************************
 * @file		24AAxx.c
 * @author		d_el
 * @version		V1.0
 * @date		21.12.2015
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		Driver for eeprom memory 24AA04, 24AA08
 */

/*!****************************************************************************
* Include
*/
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include "24AAxx.h"

/*!****************************************************************************
* Memory
*/
typedef union{
	struct{
		uint8_t rw				:1;
		uint8_t blockSelect		:3;
		uint8_t controlCode		:4;
	}bit;
	uint8_t		all;
}eepAddress_type;

enum{
	eepWrite = 0,
	eepRead	 = 1
};

#define i2ctimeout		(200)	//[ms]
#define CONTROLCODE		(0xA)
#define BYTESINPAGE		(256)
#define BYTESINBLOCK	(16)

/*!****************************************************************************
*/
Eep24AA::Eep24AA(I2cManager* i2c):
m_i2c(i2c)
{}

/*!****************************************************************************
*/
void Eep24AA::setI2c(I2cManager* i2c){
	m_i2c = i2c;
}

/*!****************************************************************************
* @brief	Write data to eeprom memory
* @param	dst - linear address (0 - 1024 for 24AA08)
*			*src - source buffer
*			len - number bytes of read
* @retval	operation status
*/
bool Eep24AA::write(uint16_t dst, const void *src, uint16_t len){
	constexpr uint16_t	eepdelayms = 6;
	uint8_t				*pData;
	eepAddress_type		eepAdr;
	uint8_t				adrInBlock;
	uint16_t			canWrite;

	if(len == 0){
		return false;
	}
	pData = (uint8_t*)src;
	eepAdr.bit.controlCode = CONTROLCODE;
	eepAdr.bit.rw = eepWrite;

	uint8_t eep_tempBff[17];
	while(len > 0){
		eepAdr.bit.blockSelect = dst / BYTESINPAGE;
		adrInBlock = dst % BYTESINPAGE;

		canWrite = (BYTESINPAGE - adrInBlock);
		if(canWrite >= BYTESINBLOCK){
			canWrite = BYTESINBLOCK;
		}

		if(len <= canWrite){
			eep_tempBff[0] = adrInBlock;
			memcpy(&eep_tempBff[1], pData, len);
			if(!m_i2c->write(eepAdr.all, eep_tempBff, len + 1, true, i2ctimeout)){
				return false;
			}
			vTaskDelay(eepdelayms);
			len -= len;
		}
		else{
			eep_tempBff[0] = adrInBlock;
			memcpy(&eep_tempBff[1], pData, canWrite);
			if(!m_i2c->write(eepAdr.all, eep_tempBff, canWrite + 1, true, i2ctimeout)){
				return false;
			}
			vTaskDelay(eepdelayms);
			len -= canWrite;
			dst += canWrite;
			pData += canWrite;
		}
	}
	return true;
}

/*!****************************************************************************
* @brief	Read data from eeprom memory
* @param	*dst - destination buffer
*			src - linear address (0 - 1024 for 24AA08)
*			len - number bytes of read
* @retval	operation status
*/
bool Eep24AA::read(void *dst, uint16_t src, uint16_t len){
	eepAddress_type		eepAdr;
	uint8_t				adrInBlock;

	if(len == 0){
		return false;
	}
	eepAdr.bit.controlCode = CONTROLCODE;
	eepAdr.bit.blockSelect = src / BYTESINPAGE;
	eepAdr.bit.rw = eepWrite;
	adrInBlock = src % BYTESINPAGE;

	if(!m_i2c->write(eepAdr.all, &adrInBlock, 1, false, i2ctimeout)){
		return false;
	}

	eepAdr.bit.rw = eepRead;

	if(!m_i2c->read(eepAdr.all, (uint8_t*)dst, len, i2ctimeout)){
		return false;
	}

	return true;
}

/******************************** END OF FILE ********************************/

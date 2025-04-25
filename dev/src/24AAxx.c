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
#include <assert.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "24AAxx.h"
#include "i2c.h"

/*!****************************************************************************
* Memory
*/
uint8_t eep_tempBff[17];
static SemaphoreHandle_t i2cSem;

/*!****************************************************************************
* EEPROM EMU
*/

/*!****************************************************************************
* @brief	I2C Callback
*/
static void eep_i2cCallback(i2c_type *i2cx){
	(void)i2cx;
	BaseType_t xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(i2cSem, &xHigherPriorityTaskWoken);
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

/*!****************************************************************************
* @brief	Initialize eeprom memory
*/
void eep_init(void){
	// Create Semaphore for I2C
	i2cSem = xSemaphoreCreateBinary();
	assert(i2cSem != NULL);

	i2c_setCallback(usei2c, eep_i2cCallback);
}

/*!****************************************************************************
* @brief	Write data to eeprom memory
* @param	dst - linear address (0 - 1024 for 24AA08)
*			*src - source buffer
*			len - number bytes of read
* @retval	none
*/
eepStatus_type eep_write(uint16_t dst, void *src, uint16_t len){
	BaseType_t			res;
	uint16_t			eepdelayms = 6;
	uint8_t				*pData;
	eepAddress_type		eepAdr;
	uint8_t				adrInBlock;
	uint16_t			canWrite;

	if(len == 0){
		return eepOtherError;
	}
	pData = (uint8_t*)src;
	eepAdr.bit.controlCode = CONTROLCODE;
	eepAdr.bit.rw = eepWrite;

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

			i2c_write(usei2c, eep_tempBff, len + 1, eepAdr.all, i2cNeedStop);
			res = xSemaphoreTake(i2cSem, pdMS_TO_TICKS(i2ctimeout));
			if(res != pdTRUE){
				return eepI2cError;
			}
			vTaskDelay(eepdelayms);

			len -= len;
		}
		else{
			eep_tempBff[0] = adrInBlock;
			memcpy(&eep_tempBff[1], pData, canWrite);

			i2c_write(usei2c, eep_tempBff, canWrite + 1, eepAdr.all, i2cNeedStop);
			res = xSemaphoreTake(i2cSem, pdMS_TO_TICKS(i2ctimeout));
			if(res != pdTRUE){
				return eepI2cError;
			}
			vTaskDelay(eepdelayms);

			len -= canWrite;
			dst += canWrite;
			pData += canWrite;
		}
	}
	return eepOk;
}

/*!****************************************************************************
* @brief	Read data from eeprom memory
* @param	*dst - destination buffer
*			src - linear address (0 - 1024 for 24AA08)
*			len - number bytes of read
* @retval	none
*/
eepStatus_type eep_read(void *dst, uint16_t src, uint16_t len){
	BaseType_t			res;
	eepAddress_type		eepAdr;
	uint8_t				adrInBlock;

	if(len == 0){
		return eepOtherError;
	}
	eepAdr.bit.controlCode = CONTROLCODE;
	eepAdr.bit.blockSelect = src / BYTESINPAGE;
	eepAdr.bit.rw = eepWrite;
	adrInBlock = src % BYTESINPAGE;

	i2c_write(usei2c, &adrInBlock, 1, eepAdr.all, i2cNeedStop);
	res = xSemaphoreTake(i2cSem, pdMS_TO_TICKS(i2ctimeout));
	if(res != pdTRUE){
		return eepI2cError;
	}

	eepAdr.bit.rw = eepRead;

	i2c_read(usei2c, dst, len, eepAdr.all);
	res = xSemaphoreTake(i2cSem, pdMS_TO_TICKS(i2ctimeout));
	if(res != pdTRUE){
		return eepI2cError;
	}

	return eepOk;
}

/******************************** END OF FILE ********************************/

/*!****************************************************************************
 * @file		i2c.cpp
 * @author		d_el
 * @version		V1.0
 * @date		Jan 22, 2026
 * @copyright	License (MIT). Copyright (c) 2024 Storozhenko Roman
 * @brief
 */

/*!****************************************************************************
 * Include
 */
#include <assert.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include <hal/i2c.h>
#include "i2c.h"

static I2cManager i2cmanager;

bool I2cManager::init(){
	m_sem = (void*)xSemaphoreCreateBinary();
	assert(m_sem != nullptr);
	m_mutex = xSemaphoreCreateMutex();
	assert(m_mutex != NULL);
	xSemaphoreGive((SemaphoreHandle_t)m_mutex);

	i2c_init(i2c1);

	auto i2cTxHook = [](i2c_type* i2c){
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		xSemaphoreGiveFromISR((SemaphoreHandle_t)(i2c->tcHookArg), &xHigherPriorityTaskWoken);
		portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
	};
	i2c_setCallback(i2c1, i2cTxHook, m_sem);

	return true;
}

bool I2cManager::write(uint8_t devAddr, const void* src, size_t len, bool needstop, uint16_t timeout){
	if(xSemaphoreTake((SemaphoreHandle_t)m_mutex, pdMS_TO_TICKS(timeout)) == pdTRUE){
		i2c_write(i2c1, src, len, devAddr, needstop ? i2cNeedStop : i2cWithoutStop);
		bool res = pdTRUE == xSemaphoreTake((SemaphoreHandle_t)m_sem, pdMS_TO_TICKS(timeout));
		xSemaphoreGive((SemaphoreHandle_t)m_mutex);
		return res;
	}else{
		return false;
	}
}

bool I2cManager::read(uint8_t devAddr, void* dst, size_t len, uint16_t timeout){
	if(xSemaphoreTake((SemaphoreHandle_t)m_mutex, pdMS_TO_TICKS(timeout)) == pdTRUE){
		i2c_read(i2c1, dst, len, devAddr);
		bool res = pdTRUE == xSemaphoreTake((SemaphoreHandle_t)m_sem, pdMS_TO_TICKS(timeout));
		xSemaphoreGive((SemaphoreHandle_t)m_mutex);
		return res;
	}else{
		return false;
	}
}

I2cManager& i2c(uint8_t number){
	(void)number;
	return i2cmanager;
}

/******************************** END OF FILE ********************************/

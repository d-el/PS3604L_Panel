﻿/*!****************************************************************************
 * @file		regulatorTSK.c
 * @author		d_el
 * @version		V2.1
 * @date		27.11.2021
 * @copyright	The MIT License (MIT). Copyright (c) 2020 Storozhenko Roman
 * @brief		connect interface with regulator
 */

/*!****************************************************************************
 * Include
 */
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include <crc.h>
#include <uart.h>
#include <modbus.h>
#include <plog.h>
#include "regulatorConnTSK.h"

/*!****************************************************************************
 * MEMORY
 */
#define uartTskUse (uart1)
#define LOG_LOCAL_LEVEL P_LOG_VERBOSE
static char *logTag = "regulator";

typedef struct{
	enum{
		cmd_none,
		cmd_enable,
		cmd_calibrateV,
		cmd_calibrateI
	}command;

	union{
		uint16_t enable;

		struct{
			uint32_t value;
			uint8_t number;
		}calibratePoint;
	};
}command_t;

SemaphoreHandle_t regulatorMutex;
QueueHandle_t commandQueue;
static regVersion_t regVersion;
uint16_t enabled;
bool connected;
regTarget_t regTarget;
regState_t regMeas;

bool reg_setVoltage(uint32_t uV){
	xSemaphoreTake(regulatorMutex, portMAX_DELAY);
	regTarget.voltage_set = uV;
	xSemaphoreGive(regulatorMutex);
	return connected;
}

bool reg_setCurrent(uint32_t uA){
	xSemaphoreTake( regulatorMutex, portMAX_DELAY);
	regTarget.current_set = uA;
	xSemaphoreGive(regulatorMutex);
	return connected;
}

bool reg_setDacVoltage(uint16_t lsb){
	xSemaphoreTake( regulatorMutex, portMAX_DELAY);
	regTarget.vdac = lsb;
	xSemaphoreGive(regulatorMutex);
	return connected;
}

bool reg_setDacCurrent(uint16_t lsb){
	xSemaphoreTake( regulatorMutex, portMAX_DELAY);
	regTarget.idac = lsb;
	xSemaphoreGive(regulatorMutex);
	return connected;
}

bool reg_setMode(regMode_t mode){
	xSemaphoreTake( regulatorMutex, portMAX_DELAY);
	regTarget.mode = mode;
	xSemaphoreGive(regulatorMutex);
	return connected;
}

bool reg_setTime(uint32_t s){
	xSemaphoreTake( regulatorMutex, portMAX_DELAY);
	regTarget.time_set = s;
	xSemaphoreGive(regulatorMutex);
	return connected;
}

bool reg_setEnable(bool state){
	if(state != enabled){
		command_t command = {
			.command = cmd_enable,
			.enable = state ? 1 : 0,
		};
		return pdTRUE == xQueueSend(commandQueue, &command, 0);
	}
	return connected;
}

bool reg_setVoltagePoint(uint32_t uV, uint8_t number){
	command_t command = {
		.command = cmd_calibrateV,
		.calibratePoint.value = uV,
		.calibratePoint.number = number
	};
	return pdTRUE == xQueueSend(commandQueue, &command, 0);
}

bool reg_setCurrentPoint(uint32_t uA, uint8_t number){
	command_t command = {
		.command = cmd_calibrateI,
		.calibratePoint.value = uA,
		.calibratePoint.number = number
	};
	return pdTRUE == xQueueSend(commandQueue, &command, 0);
}

bool reg_getTarget(regTarget_t *target){
	*target = regTarget;
	return connected;
}

bool reg_getEnable(bool *state){
	*state = enabled != 0;
	return connected;
}

bool reg_getState(regState_t *state){
	xSemaphoreTake( regulatorMutex, portMAX_DELAY);
	*state = regMeas;
	xSemaphoreGive(regulatorMutex);
	return connected;
}

bool reg_getVersion(regVersion_t *v){
	*v = regVersion;
	return true;
}

bool writeReg(modbus_t *ctx, uint16_t addr, uint16_t value){
	uint8_t try = 3;
	while(try--){
		int mbstatus = modbus_write_register(ctx, addr, value);
		if(mbstatus == 1){
			return true;
		}else{
			P_LOGW(logTag, "error write [%" PRIu16 "], %s", addr, modbus_strerror(libmodbuserrno));
		}
	}
	return false;
}

bool writeRegs(modbus_t *ctx, uint16_t addr, void *src, uint16_t len){
	uint8_t try = 3;
	while(try--){
		int mbstatus = modbus_write_registers(ctx, addr, len, (uint16_t*)src);
		if(mbstatus == len){
			return true;
		}else{
			P_LOGW(logTag, "error write [%" PRIu16 "], %s", addr, modbus_strerror(libmodbuserrno));
		}
	}
	return false;
}

bool readRegs(modbus_t *ctx, uint16_t addr, void *dst, uint16_t len){
	uint8_t try = 3;
	while(try--){
		int mbstatus = modbus_read_registers(ctx, addr, len, (void*)dst);
		if(mbstatus == len){
			return true;
		}else{
			P_LOGW(logTag, "error read [%" PRIu16 "], %s", addr, modbus_strerror(libmodbuserrno));
		}
	}
	return false;
}


/*!****************************************************************************
 * @brief
 */
void regulatorConnTSK(void *pPrm){
	(void)pPrm;
	regulatorMutex = xSemaphoreCreateMutex();
	commandQueue = xQueueCreate(8, sizeof(command_t));
	TickType_t	xLastWakeTime = xTaskGetTickCount();

	modbus_t *ctx;
	ctx = modbus_new_rtu("USART1", 115200, 'N', 8, 1);
	modbus_set_slave(ctx, 0x01);
	modbus_set_debug(ctx, FALSE);
	modbus_connect(ctx);

	uint8_t errorCount = 0;
	const uint8_t errorThreshold = 5;
	readRegs(ctx, 0x0000, &regVersion, 3);
	
	while(1){
		xSemaphoreTake(regulatorMutex, portMAX_DELAY);
		regTarget_t lpcalRegTarget = regTarget;
		xSemaphoreGive(regulatorMutex);

		// Set target value
		int32_t number = sizeof(lpcalRegTarget)/sizeof(uint16_t);
		if(writeRegs(ctx, 0x0100, &lpcalRegTarget, number)){
			errorCount = 0;
		}
		else{
			if(errorCount < errorThreshold)
				errorCount++;
		}

		// Process command
		command_t command;
		if(xQueuePeek(commandQueue, &command, 0) == pdPASS ){
			switch(command.command){
				case cmd_enable:{
					if(writeReg(ctx, 0x0109, command.enable)){
						xQueueReceive(commandQueue, &command, 0);
						errorCount = 0;
					}else{
						if(errorCount < errorThreshold)
							errorCount++;
					}
				}break;

				case cmd_calibrateV:{
					if(writeRegs(ctx, 0x0500 + 2 * command.calibratePoint.number, &command.calibratePoint.value, 2)){
						xQueueReceive(commandQueue, &command, 0);
						errorCount = 0;
					}else{
						if(errorCount < errorThreshold)
							errorCount++;
					}
				}break;

				case cmd_calibrateI:{
					if(writeRegs(ctx, 0x0600 + 2 * command.calibratePoint.number, &command.calibratePoint.value, 2)){
						errorCount = 0;
						xQueueReceive(commandQueue, &command, 0);
					}else{
						if(errorCount < errorThreshold)
							errorCount++;
					}
				}break;

				default:
					P_LOGW(logTag, "error command");
			}
		}

		// Read enable state
		if(readRegs(ctx, 0x0109, &enabled, 1)){
			errorCount = 0;
		}else{
			if(errorCount < errorThreshold)
				errorCount++;
		}

		// Read measure
		regState_t localRegMeas;
		number = sizeof(localRegMeas)/sizeof(uint16_t);
		if(readRegs(ctx, 0x0200, &localRegMeas, number)){
			xSemaphoreTake( regulatorMutex, portMAX_DELAY);
			regMeas = localRegMeas;
			xSemaphoreGive(regulatorMutex);
			errorCount = 0;
		}
		else{
			if(errorCount < errorThreshold)
				errorCount++;
		}

		connected = errorCount < errorThreshold ? true : false;
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(20));
	}
}

/******************************** END OF FILE ********************************/

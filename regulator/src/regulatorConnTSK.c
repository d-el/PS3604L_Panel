/*!****************************************************************************
 * @file		regulatorTSK.c
 * @author		d_el
 * @version		V2.1
 * @date		27.11.2021
 * @copyright	The MIT License (MIT). Copyright (c) 2025 Storozhenko Roman
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
#include <modbus-private.h> // For TCP bridge
#include <plog.h>
#include "regulatorConnTSK.h"

/*!****************************************************************************
 * MEMORY
 */
#define LOG_LOCAL_LEVEL P_LOG_DEBUG
static char *logTag = "regulator";

typedef struct{
	bool *result;
	SemaphoreHandle_t semaphore;
	void *src, *dst;
	uint16_t writeAddr, readAddr;
	uint16_t writeNum, readNum;
}command_t;

static SemaphoreHandle_t regulatorMutex;
static SemaphoreHandle_t tcpSem;
static QueueHandle_t commandQueue;
static regVersion_t regVersion;
static regTarget_t regTarget = { .current_set = 100, .mode = reg_limitation };
static regState_t regMeas;
static uint16_t enabled;
static uint8_t *mobusRequstData;
static uint16_t *mobusRequstLen;
static bool mobusRequstResult;
static bool gremote;
static bool connected = true;
static const uint16_t defaultslave = 1;

static bool reg_waitCommand(command_t* cmd){
	bool res;
	cmd->result = &res;
	cmd->semaphore = xSemaphoreCreateBinary();
	if(cmd->semaphore == NULL){
		P_LOGE(logTag, "error create semaphore");
		return false;
	}
	xQueueSend(commandQueue, cmd, 0);
	BaseType_t osres = xSemaphoreTake(cmd->semaphore, 500);
	vSemaphoreDelete(cmd->semaphore);
	return osres == pdTRUE && cmd->result;
}

bool reg_versionGet(regVersion_t *v){
	*v = regVersion;
	return true;
}

bool reg_serialGet(uint32_t* sn){
	command_t command = { .readAddr = 0x0004, .dst = sn, .readNum = 2 };
	return reg_waitCommand(&command);
}

bool reg_calibrationTimeGet(time_t* time){
	command_t command = { .readAddr = 0x0300, .dst = time, .readNum = 2 };
	return reg_waitCommand(&command);
}

bool reg_dacMaxValueGet(int32_t *val){
	command_t command = { .readAddr = 0x0302, .dst = val, .readNum = 2 };
	return reg_waitCommand(&command);
}

bool reg_getTarget(regTarget_t *target){
	uint16_t number = sizeof(regTarget_t)/sizeof(uint16_t);
	command_t command = { .readAddr = 0x0100, .dst = target, .readNum = number };
	return reg_waitCommand(&command);
}

bool reg_voltageSet(int32_t uV){
	xSemaphoreTake(regulatorMutex, portMAX_DELAY);
	regTarget.voltage_set = uV;
	xSemaphoreGive(regulatorMutex);
	return connected;
}

bool reg_currentSet(int32_t uA){
	xSemaphoreTake( regulatorMutex, portMAX_DELAY);
	regTarget.current_set = uA;
	xSemaphoreGive(regulatorMutex);
	return connected;
}

bool reg_dacVoltageSet(int32_t lsb){
	xSemaphoreTake( regulatorMutex, portMAX_DELAY);
	regTarget.vdac = lsb;
	xSemaphoreGive(regulatorMutex);
	return connected;
}

bool reg_dacCurrentSet(int32_t lsb){
	xSemaphoreTake( regulatorMutex, portMAX_DELAY);
	regTarget.idac = lsb;
	xSemaphoreGive(regulatorMutex);
	return connected;
}

bool reg_modeSet(regMode_t mode){
	xSemaphoreTake( regulatorMutex, portMAX_DELAY);
	regTarget.mode = mode;
	xSemaphoreGive(regulatorMutex);
	return connected;
}

bool reg_timeSet(uint32_t ms){
	xSemaphoreTake( regulatorMutex, portMAX_DELAY);
	regTarget.time_set = ms;
	xSemaphoreGive(regulatorMutex);
	return connected;
}

bool reg_enableSet(bool state){
	if(state != enabled){
		uint16_t enable = state ? 1 : 0;
		command_t command = { .writeAddr = 0x010B, .src = &enable, .writeNum = 1 };
		if(!reg_waitCommand(&command)){
			return false;
		}
	}
	return true;
}

bool reg_enableGet(bool *state){
	*state = enabled != 0;
	return connected;
}

bool reg_wireResistanceSet(uint32_t r){
	command_t command = { .writeAddr = 0x010C, .src = &r, .writeNum = 2 };
	return reg_waitCommand(&command);
}

bool reg_setCalibrationTime(void){
	time_t t = time(NULL);
	command_t command = { .writeAddr = 0x0300, .src = &t, .writeNum = 2 };
	return reg_waitCommand(&command);
}

bool reg_saveSettingsSet(regSave_t save){
	uint16_t s = save;
	command_t command = { .writeAddr = 0x010F, .src = &s, .writeNum = 1 };
	return reg_waitCommand(&command);
}

bool reg_saveSettingsGet(regSave_t *save){
	uint16_t s;
	command_t command = { .readAddr = 0x010F, .dst = &s, .readNum = 1 };
	if(!reg_waitCommand(&command)){
		return false;
	}
	*save = s;
	return true;
}

bool reg_crangeSet(regCrange_t crange){
	uint16_t c = crange;
	command_t command = { .writeAddr = 0x0110, .src = &c, .writeNum = 1 };
	return reg_waitCommand(&command);
}

bool reg_crangeGet(regCrange_t* crange){
	uint16_t c;
	command_t command = { .readAddr = 0x0110, .dst = &c, .readNum = 1 };
	if(!reg_waitCommand(&command)){
		return false;
	}
	*crange = c;
	return true;
}

bool reg_vFilterSizeSet(uint16_t size){
	uint16_t c = size;
	command_t command = { .writeAddr = 0x0112, .src = &c, .writeNum = 1 };
	return reg_waitCommand(&command);
}

bool reg_iFilterSizeSet(uint16_t size){
	uint16_t c = size;
	command_t command = { .writeAddr = 0x0113, .src = &c, .writeNum = 1 };
	return reg_waitCommand(&command);
}

bool reg_vIntegrationSizeSet(uint16_t size){
	uint16_t c = size;
	command_t command = { .writeAddr = 0x0114, .src = &c, .writeNum = 1 };
	return reg_waitCommand(&command);
}

bool reg_iIntegrationSizeSet(uint16_t size){
	uint16_t c = size;
	command_t command = { .writeAddr = 0x0115, .src = &c, .writeNum = 1 };
	return reg_waitCommand(&command);
}

bool reg_getState(regState_t *state){
	xSemaphoreTake( regulatorMutex, portMAX_DELAY);
	*state = regMeas;
	xSemaphoreGive(regulatorMutex);
	return connected;
}

bool reg_setVoltagePoint(int32_t uV, uint8_t number){
	command_t command = { .writeAddr = 0x0310 + 2 * number, .src = &uV, .writeNum = 2 };
	if(!reg_waitCommand(&command)){
		return false;
	}
	return reg_setCalibrationTime();
}

bool reg_setMicroCurrentPoint(int32_t uA, uint8_t number){
	command_t command = { .writeAddr = 0x0330 + 2 * number, .src = &uA, .writeNum = 2 };
	if(!reg_waitCommand(&command)){
		return false;
	}
	return reg_setCalibrationTime();
}

bool reg_setCurrentPoint(int32_t uA, uint8_t number){
	command_t command = { .writeAddr = 0x0320 + 2 * number, .src = &uA, .writeNum = 2 };
	if(!reg_waitCommand(&command)){
		return false;
	}
	return reg_setCalibrationTime();
}

bool reg_modbusRequest(uint8_t *req, uint16_t *req_length){
	mobusRequstData = req;
	mobusRequstLen = req_length;
	BaseType_t osres = xSemaphoreTake(tcpSem, 1000);
	if(osres != pdTRUE){
		return false;
	}
	return mobusRequstResult;
}

void reg_setremote(bool rem){
	gremote = rem;
}

bool reg_getremote(void){
	return gremote;
}

static bool writeReg(modbus_t *ctx, uint16_t addr, uint16_t value){
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

static bool writeRegs(modbus_t *ctx, uint16_t addr, void *src, uint16_t len){
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

static bool readRegs(modbus_t *ctx, uint16_t addr, void *dst, uint16_t len, uint8_t try){
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

static bool rawModbusMessage(modbus_t *ctx, uint8_t *req, uint16_t *req_length){
	uint8_t try = 3;
	while(try--){
		req[0] = defaultslave;
		ssize_t msg_length = ctx->backend->send_msg_pre(req, *req_length);
		ssize_t rc = ctx->backend->send(ctx, req, msg_length);
		rc = modbus_receive_confirmation(ctx, req);
		if(rc < 3){
			P_LOGW(logTag, "rawModbusMessage error read");
			return false;
		}
		*req_length = rc - 2;
		return true;
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

	tcpSem = xSemaphoreCreateBinary();
	xSemaphoreTake(tcpSem, 0);

	modbus_t *ctx = modbus_new_rtu("USART1", 921600, 'N', 8, 1);
	modbus_set_slave(ctx, defaultslave);
	modbus_set_debug(ctx, FALSE);
	modbus_connect(ctx);

	uint8_t errorCount = 0;
	const uint8_t errorThreshold = 5;
	readRegs(ctx, 0x0000, &regVersion, 3, 16);

	while(1){
		xSemaphoreTake(regulatorMutex, portMAX_DELAY);
		regTarget_t lpcalRegTarget = regTarget;
		xSemaphoreGive(regulatorMutex);

		// Set target local value
		if(gremote == false){
			int32_t number = sizeof(lpcalRegTarget)/sizeof(uint16_t);
			if(writeRegs(ctx, 0x0100, &lpcalRegTarget, number)){
				errorCount = 0;
			}
			else{
				if(errorCount < errorThreshold)
					errorCount++;
			}
		}

		// Process command
		command_t command;
		if(xQueueReceive(commandQueue, &command, 0) == pdPASS ){
			bool result = false;
			if(command.writeNum){
				if(command.writeNum == 1){
					result = writeReg(ctx, command.writeAddr, *(uint16_t*)command.src);
				}else{
					result = writeRegs(ctx, command.writeAddr, command.src, command.writeNum);
				}
			}
			if(command.readNum){
				result = readRegs(ctx, command.readAddr, command.dst, command.readNum, 3);
			}
			*command.result = result;
			xSemaphoreGive(command.semaphore);
		}

		// Read enable state
		if(readRegs(ctx, 0x010B, &enabled, 1, 3)){
			errorCount = 0;
		}else{
			if(errorCount < errorThreshold)
				errorCount++;
		}

		// Read measure
		regState_t localRegMeas;
		int32_t number = sizeof(localRegMeas)/sizeof(uint16_t);
		if(readRegs(ctx, 0x0200, &localRegMeas, number, 3)){
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

		if(mobusRequstData){
			mobusRequstResult = rawModbusMessage(ctx, mobusRequstData, mobusRequstLen);
			mobusRequstData = NULL;
			xSemaphoreGive(tcpSem);
		}
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
	}
}

/******************************** END OF FILE ********************************/

/*!****************************************************************************
 * @file		regulatorConnTSK.h
 * @author		d_el
 * @version		V2.1
 * @date		27.11.2021
 * @copyright	The MIT License (MIT). Copyright (c) 2020 Storozhenko Roman
 * @brief		connect interface with regulator
 */
#ifndef uartTSK_H
#define uartTSK_H

#ifdef __cplusplus
extern "C" {
#endif

/*!****************************************************************************
 * Include
 */
#include <stdint.h>
#include <stdbool.h>

/*!****************************************************************************
 * Define
 */

/******************************************************************************
 * Typedef
 */
typedef struct __attribute__ ((packed)){
	uint16_t major;
	uint16_t minor;
	uint16_t patch;
} regVersion_t;

typedef struct __attribute__ ((packed)){
	uint32_t voltage_set;
	uint32_t current_set;
	uint16_t vdac;
	uint16_t idac;
	uint16_t mode;
	uint32_t time_set;
} regTarget_t;

typedef enum {
	reg_overcurrentShutdown,
	reg_limitation,
	reg_timeShutdown,
	reg_lowCurrentShutdown,
	reg_raw
} regMode_t;

typedef union __attribute__ ((packed)){
	struct{
	uint16_t m_errorExternalIAdc :1;
	uint16_t m_errorTemperatureSensor :1;
	uint16_t m_overheated :1;
	uint16_t m_lowInputVoltage :1;
	uint16_t m_reverseVoltage :1;
	uint16_t m_notCalibrated :1;
	uint16_t m_limitation :1;
	uint16_t m_externaIDac :1;
	};
	uint16_t all;
} regStatus_t;

typedef enum {
	v_none = 0,
	v_errorTemperatureSensor = 1,
	v_overheated = 2,
	v_lowInputVoltage = 3,
	v_reverseVoltage = 4,
	v_overCurrent = 5,
	v_timeShutdown = 6,
	v_lowCurrentShutdown = 7,
	v_request = 8
} disablecause_t;

typedef struct __attribute__ ((packed)){
	uint32_t voltage;		///< [X_XXXXXX V]
	uint32_t current;		///< [X_XXXXXX A]
	uint32_t power;			///< [X_XXX Wt]
	uint32_t resistance;	///< [X_XXX Ohm]
	uint32_t time;			///< [s]
	uint32_t capacity;		///< [X_XXX Ah]
	uint32_t input_voltage;	///< [X_XXXXXX V]
	uint16_t temperature;	///< [X_X °С]
	regStatus_t status;
	uint16_t disablecause;
	uint16_t vadc;			///< [LSB]
	uint16_t iadc;			///< [LSB]
	int32_t iexternaladc;	///< [LSB]
} regState_t;

/******************************************************************************
 * Prototypes for the functions
 */
void regulatorConnTSK(void *pPrm);

bool reg_setVoltage(uint32_t uV);
bool reg_setCurrent(uint32_t uA);
bool reg_setDacVoltage(uint16_t lsb);
bool reg_setDacCurrent(uint16_t lsb);
bool reg_setMode(regMode_t mode);
bool reg_setTime(uint32_t s);
bool reg_setEnable(bool state);
bool reg_setVoltagePoint(uint32_t uV, uint8_t number);
bool reg_setCurrentPoint(uint32_t uA, uint8_t number);

bool reg_getTarget(regTarget_t *target);
bool reg_getEnable(bool *state);
bool reg_getState(regState_t *state);
bool reg_getVersion(regVersion_t *v);

void reg_setremote(bool rem);
bool reg_modbusRequest(uint8_t *req, uint16_t *req_length);

void modbusServerTSK(void *pPrm);

#ifdef __cplusplus
}
#endif

#endif //uartTSK_H
/******************************** END OF FILE ********************************/

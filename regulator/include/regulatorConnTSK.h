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
#include <time.h>

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
	int32_t voltage_set;
	int32_t current_set;
	int32_t vdac;
	int32_t idac;
	uint16_t mode;
	uint32_t time_set;	///< [ms]
} regTarget_t;

typedef enum {
	reg_overcurrentShutdown,
	reg_limitation,
	reg_lowCurrentShutdown,
	reg_raw
} regMode_t;

typedef enum {
	reg_save_nop,
	reg_save_do,
	reg_save_ok,
	reg_save_nothing,
	reg_save_error
} regSave_t;

typedef enum {
	reg_crange_hi,
	reg_crange_auto
} regCrange_t;

typedef union __attribute__ ((packed)){
	struct{
	uint16_t m_errorExternalIAdc :1;
	uint16_t m_errorTemperatureSensor :1;
	uint16_t m_overheated :1;
	uint16_t m_lowInputVoltage :1;
	uint16_t m_reverseVoltage :1;
	uint16_t m_calibrationEmpty :1;
	uint16_t m_limitation :1;
	uint16_t m_externaIDac :1;
	uint16_t cRangeLoOverflow :1;
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
	int32_t voltage;		///< [X_XXXXXX V]
	int32_t current;		///< [X_XXXXXX A]
	uint32_t power;			///< [X_XXXXXX Wt]
	int32_t resistance;		///< [X_XXXX Ohm]
	uint32_t time;			///< [X_XXX s]
	uint32_t capacity;		///< [X_XXX Ah]
	int32_t input_voltage;	///< [X_XXXXXX V]
	int16_t temperature;	///< [X_X °С]
	regStatus_t status;
	uint16_t disablecause;
	int32_t vadc;			///< [LSB]
	int32_t iadc;			///< [LSB]
	int32_t iexternaladc;	///< [LSB]
} regState_t;

/******************************************************************************
 * Prototypes for the functions
 */
void regulatorConnTSK(void *pPrm);

bool reg_setVoltage(int32_t uV);
bool reg_setCurrent(int32_t uA);
bool reg_setDacVoltage(int32_t lsb);
bool reg_setDacCurrent(int32_t lsb);
bool reg_setMode(regMode_t mode);
bool reg_setTime(uint32_t ms);

bool reg_setEnable(bool state);
bool reg_getEnable(bool *state);

bool reg_setWireResistance(uint32_t r);					// X_XXXX Ohm
bool reg_setVoltagePoint(int32_t uV, uint8_t number);
bool reg_setMicroCurrentPoint(int32_t uA, uint8_t number);
bool reg_setCurrentPoint(int32_t uA, uint8_t number);

bool reg_setSaveSettings(regSave_t save);
bool reg_getSaveSettings(regSave_t *save);
bool reg_setCrange(regCrange_t crange);

bool reg_getCalibrationTime(time_t* time);
bool reg_getDacMaxValue(int32_t *val);

bool reg_getTarget(regTarget_t *target);
bool reg_getState(regState_t *state);
bool reg_getVersion(regVersion_t *v);
bool reg_getSerial(uint32_t* sn);

void reg_setremote(bool rem);
bool reg_getremote(void);

bool reg_modbusRequest(uint8_t *req, uint16_t *req_length);

void modbusServerTSK(void *pPrm);

#ifdef __cplusplus
}
#endif

#endif //uartTSK_H
/******************************** END OF FILE ********************************/

/*!****************************************************************************
 * @file    types.h
 * @author  d_el
 * @version V1.0
 * @date    29.12.2015, by d_el
 * @brief   Типы данных для двух проектов
 */
#ifndef types_H
#define types_H

/*!****************************************************************************
 * Include
 */
#include "stdint.h"

/*!****************************************************************************
 * User define
 */
#define NUMBER_CALIBRATE_POINTS			4
#define VOLTAGE_CALIBRATE_POINTS		10, 100, 5000, 30000	/*[X_XXX V]*/
#define CURRENT_CALIBRATE_POINTS		1, 10, 100, 2000		/*[X_XXX A]*/
#define CURRENT_SENSOR_THRESHOLD_UP   	100     //[X_XXX A]
#define CURRENT_SENSOR_THRESHOLD_DOWN   90     	//[X_XXX A]

/*!****************************************************************************
 * User enum
 */

/*!****************************************************************************
 * User typedef
 */
typedef enum {
	mode_off,                   //!< mode_off
	mode_overcurrentShutdown,	//!< mode_overcurrentShutdown
	mode_limitation,         	//!< mode_limitation
	mode_timeShutdown,       	//!< mode_timeShutdown
	mode_lowCurrentShutdown, 	//!< mode_lowCurrentShutdown
	mode_Uadj,                  //!< mode_Uadj
	mode_Iadj,                  //!< mode_Iadj
	
	mode_raw,
} psMode_type;

typedef enum {
	setNone,
	setSwitchOn,
	setSwitchOff,
	setSavePointU0,
	setSavePointU1,
	setSavePointU2,
	setSavePointU3,
	setSavePointI0,
	setSavePointI1,
	setSavePointI2,
	setSavePointI3,
	setSaveSettings
} request_type;

typedef union {
	struct {
		uint32_t ovfCurrent :1;
		uint32_t switchIsON :1;
		uint32_t modeIlim :1;
		
		//Аварии
		uint32_t ovfLinearRegTemper :1;
		uint32_t errorLinearRegTemperSens :1;
		uint32_t lowInputVoltage :1;
		uint32_t reverseVoltage :1;
		uint32_t noCalibration :1;
	} bit;
	uint32_t all;
} psState_type;

typedef struct {
	uint32_t power;          ///< [X_XXX Wt]
	uint32_t resistens;      ///< [X_XXX Ohm]
	uint32_t time;           ///< [s]
	uint32_t capacity;       ///< [X_XXX A/h]
	uint32_t u;              ///< [X_XXXXXX V]
	uint32_t i;              ///< [X_XXXXXX A]
	uint16_t adcu;           ///< [LSB]
	uint16_t adci;           ///< [LSB]
	uint16_t uin;            ///< [mV]
	uint16_t temperatureLin; ///< [X_X °С]
} meas_type;

typedef struct {
	uint32_t u;              ///< [X_XXXXXX V]
	uint32_t i;              ///< [X_XXXXXX A]
	uint16_t dacu;           ///< [LSB]
	uint16_t daci;           ///< [LSB]
	uint32_t time;           ///< [s]
	request_type request;
	psMode_type mode;
} task_type;

typedef struct {
	psState_type state;
	meas_type meas;
	task_type task;
} transfer_type;

/*!****************************************************************************
 * Extern viriables
 */

/*!****************************************************************************
 * Macro functions
 */

/*!****************************************************************************
 * Prototypes for the functions
 */

#endif //types_H
/***************** (C) COPYRIGHT ************** END OF FILE ******** d_el ****/

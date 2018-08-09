/*!****************************************************************************
 * @file		crc.h
 * @author		Storozhenko Roman
 * @version		V2.1
 * @date		15.01.2018
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		CRC counting module
 */

#ifndef crc_H
#define crc_H

/*!****************************************************************************
* Include
*/
#include "stdint.h"

/*!****************************************************************************
 * Enumeration
 */
typedef enum{
	crc_modbus,
	crc_can,
	crc_1wire
}crc_type;

/*!****************************************************************************
 * Typedef
 */
typedef struct{
	uint16_t	table[256];
	uint16_t	initVal;
	uint16_t	finalXorValue;
}crc16Struct_type;

typedef struct{
	uint8_t		table[256];
	uint8_t		initVal;
	uint8_t		finalXorValue;
}crc8Struct_type;

/*!****************************************************************************
 * Exported variables
 */
extern const crc16Struct_type crcModBus;
extern const crc16Struct_type crcCan;
extern const crc8Struct_type crc1Wire;

/*!****************************************************************************
 * Function declaration
 */
uint16_t crc16Calc(const crc16Struct_type *dat, void *src, uint32_t len);
uint16_t crc16CalcWithInit(const crc16Struct_type *dat, void *src, uint32_t len, uint16_t initVal);
uint8_t crc8Calc(const crc8Struct_type *dat, void *src, uint32_t len);

#endif	//crc_H
/******************************** END OF FILE ********************************/

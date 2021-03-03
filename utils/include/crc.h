/*!****************************************************************************
 * @file		crc.h
 * @author		d_el - Storozhenko Roman
 * @version		V2.1
 * @date		12.12.2017
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 */

#ifndef crc_H
#define crc_H

#ifdef __cplusplus
extern "C" {
#endif

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
extern crc16Struct_type crcModBus;
extern crc16Struct_type crcCan;
extern crc8Struct_type crc1Wire;

/*!****************************************************************************
 * Function declaration
 */
uint16_t crc16Calc(crc16Struct_type *dat, const void *src, uint32_t len);
uint16_t crc16CalcWithInit(crc16Struct_type *dat, const void *src, uint32_t len, uint16_t initVal);
uint8_t crc8Calc(crc8Struct_type *dat, const void *src, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif	//crc_H
/******************************** END OF FILE ********************************/

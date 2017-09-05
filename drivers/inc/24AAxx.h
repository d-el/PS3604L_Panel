/*!****************************************************************************
 * @file		24AAxx.h
 * @author		d_el
 * @version		V1.0
 * @date		21.12.2015
 * @copyright	GNU Lesser General Public License v3
 * @brief		Driver for eeprom memory 24AA04, 24AA08
 */
#ifndef e24AAxx_H
#define e24AAxx_H

/*!****************************************************************************
 * Include
 */
#include "stdint.h"
#include "string.h"
#include "i2c.h"
//#include "delay.h"

/*!****************************************************************************
 * Define
 */
#define CONTROLCODE     (0xA)
#define BYTESINPAGE     (256)
#define BYTESINBLOCK    (16)
#define eepI2c          (i2c2)

/*!****************************************************************************
 * Enumeration
 */
enum {
	eepWrite = 0,
	eepRead = 1
};

/*!****************************************************************************
 * Typedef
 */
typedef enum {
	eepOk = 0,
	eepI2cError,
	eepOtherError
} eepStatus_type;

typedef union {
	struct {
		uint8_t rw :1;
		uint8_t blockSelect :3;
		uint8_t controlCode :4;
	} bit;
	uint8_t adr;
} eepAddress_type;

/*!****************************************************************************
 * Exported variables
 */

/*!****************************************************************************
 * Macro functions
 */

/*!****************************************************************************
 * Function declaration
 */
eepStatus_type eep_write(uint16_t dst, void *src, uint16_t len);
eepStatus_type eep_read(void *dst, uint16_t src, uint16_t len);

#endif //e24AAxx_H
/*************** LGPL ************** END OF FILE *********** D_EL ************/

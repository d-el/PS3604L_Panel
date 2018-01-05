/*!****************************************************************************
* @file    24AAxx.h
* @author  d_el
* @version V1.0
* @date    21.12.2015, by d_el
* @brief   --
*/
#ifndef e24AAxx_H
#define e24AAxx_H

/*!****************************************************************************
* Include
*/
#include "stdint.h"

/*!****************************************************************************
* User include
*/

/*!****************************************************************************
* User define
*/
#define usei2c          (i2c1)
#define i2ctimeout      (200)	//[ms]
#define CONTROLCODE     (0xA)
#define BYTESINPAGE     (256)
#define BYTESINBLOCK    (16)

/*!****************************************************************************
* User enum
*/
enum{
    eepWrite = 0,
    eepRead  = 1
};

/*!****************************************************************************
* User typedef
*/
typedef enum{
    eepOk = 0,
    eepI2cError,
    eepOtherError
}eepStatus_type;

typedef union{
    struct{
        uint8_t rw              :1;
        uint8_t blockSelect     :3;
        uint8_t controlCode     :4;
    }bit;
    uint8_t     all;
}eepAddress_type;

/*!****************************************************************************
* External variables
*/

/*!****************************************************************************
* Macro functions
*/

/*!****************************************************************************
* Prototypes for the functions
*/
void eep_init(void);
eepStatus_type eep_write(uint16_t dst, void *src, uint16_t len);
eepStatus_type eep_read(void *dst, uint16_t src, uint16_t len);

#endif //e24AAxx_H
/***************** (C) COPYRIGHT ************** END OF FILE ******** d_el ****/

/*!****************************************************************************
* @file    24AAxx.c
* @author  d_el
* @version V1.0
* @date    21.12.2015, by d_el
* @brief   Driver for eeprom memory 24AA04, 24AA08
*/

/*!****************************************************************************
* Include
*/
#include "24AAxx.h"

/*!****************************************************************************
* Memory
*/
uint8_t eep_tempBff[17];

/*!****************************************************************************
* EEPROM EMU
*/
/*uint8_t chip[1024];
void i2c_write(uint8_t adr, uint8_t *p, uint32_t len){
    uint16_t linear_adr;

    eepAddress_type *ea = (eepAddress_type*)&adr;
    linear_adr = ((ea->bit.blockSelect * BYTESINPAGE) + *p);

    memcpy(chip + linear_adr, p +1, len);
}
void i2c_read(uint8_t adr, uint8_t *p, uint32_t len){
    uint16_t linear_adr;

    eepAddress_type *ea = (eepAddress_type*)&adr;
    linear_adr = ((ea->bit.blockSelect * BYTESINPAGE) + *p);

    memcpy(p, chip + linear_adr, len);
}*/



/*!****************************************************************************
* @brief    Write data to eeprom memory
* @param    dst - linear address (0 - 1024 for 24AA08)
*           *src - source buffer
*           len - number bytes of read
* @retval   none
*/
eepStatus_type eep_write(uint16_t dst, void *src, uint16_t len){
	uint16_t eepdelayms = 6;
    uint8_t             *pData;
    eepAddress_type     eepAdr;
    uint8_t             adrInBlock;
    uint16_t            canWrite;

    if(len == 0){
        return eepOtherError;
    }
    pData = (uint8_t*)src;
    eepAdr.bit.controlCode = CONTROLCODE;
    eepAdr.bit.rw = eepWrite;

    while(len > 0){
        eepAdr.bit.blockSelect = dst / BYTESINPAGE;
        adrInBlock = dst % BYTESINPAGE;

        canWrite = (BYTESINPAGE - adrInBlock);
        if(canWrite >= BYTESINBLOCK){
            canWrite = BYTESINBLOCK;
        }

        if(len <= canWrite){
            eep_tempBff[0] = adrInBlock;
            memcpy(&eep_tempBff[1], pData, len);

            i2c_write(usei2c, eep_tempBff, len + 1, eepAdr.all);
            while(usei2c->state == i2cTxRun);
            if(usei2c->state != i2cTxSuccess){
                return eepI2cError;
            }
            vTaskDelay(eepdelayms);

            len -= len;
        }
        else{
            eep_tempBff[0] = adrInBlock;
            memcpy(&eep_tempBff[1], pData, canWrite);

            i2c_write(usei2c, eep_tempBff, canWrite + 1, eepAdr.all);
            while(usei2c->state == i2cTxRun);
            if(usei2c->state != i2cTxSuccess){
                return eepI2cError;
            }
            vTaskDelay(eepdelayms);

            len -= canWrite;
            dst += canWrite;
            pData += canWrite;
        }
    }
    return eepOk;
}

/*!****************************************************************************
* @brief    Read data from eeprom memory
* @param    *dst - destination buffer
*           src - linear address (0 - 1024 for 24AA08)
*           len - number bytes of read
* @retval   none
*/
eepStatus_type eep_read(void *dst, uint16_t src, uint16_t len){
    eepAddress_type     eepAdr;
    uint8_t             adrInBlock;

    if(len == 0){
        return eepOtherError;
    }
    eepAdr.bit.controlCode = CONTROLCODE;
    eepAdr.bit.blockSelect = src / BYTESINPAGE;
    eepAdr.bit.rw = eepWrite;
    adrInBlock = src % BYTESINPAGE;

    i2c_write(usei2c, &adrInBlock, 1, eepAdr.all);  //����� len ������
    while(usei2c->state == i2cTxRun);
    if(usei2c->state != i2cTxSuccess){
        return eepI2cError;
    }

    eepAdr.bit.rw = eepRead;

    i2c_read(usei2c, dst, len, eepAdr.all);
    while(usei2c->state == i2cRxRun);
    if(usei2c->state != i2cRxSuccess){
        return eepI2cError;
    }

    return eepOk;
}

/***************** (C) COPYRIGHT ************** END OF FILE ******** d_el ****/

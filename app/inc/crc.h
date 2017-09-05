/**
 *******************************************************************************
 * @file    crc.h
 * @author  Storozhenko Roman - D_EL
 * @version V1.0.0
 * @date    23.06.2015
 * @brief
 */
#ifndef crc_H
#define crc_H

/*!****************************************************************************
 * Include
 */
#include "stdint.h"

/******************************************************************************
 * Prototypes for the functions
 */
uint16_t GetCrcPO(void);
uint16_t GetCrc(void *Bff, uint32_t szBff);
uint16_t GetCrc3Word(void *Bff);
uint16_t CrcPice(uint8_t *Bff, uint32_t szBff, uint16_t CrcPrev);

#endif  //crc_H
/*************** (C) COPYRIGHT ************** END OF FILE ********* D_EL *****/

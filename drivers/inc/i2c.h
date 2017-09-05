/*!****************************************************************************
 * @file    i2c.h
 * @author  Storozhenko Roman - D_EL
 * @version V1.0
 * @date    22.11.2015
 * @brief
 */
#ifndef i2c_H
#define i2c_H

/*!****************************************************************************
 * Include
 */
#include "gpio.h"
#include "stdint.h"
#include "stddef.h"
//#include "OSinit.h"

/*!****************************************************************************
 * User define
 */
//I2C1
#define     I2C1_USE                    (0)
#define     I2C1_TC_HOOK                (0)
#define     I2C1_TxBffSz                (64)
#define     I2C1_RxBffSz                (32)
#define     I2C1_EventInterruptPrior    (15)
#define     I2C1_ErrorInterruptPrior    (15)
#define     I2C1_RxDmaInterruptPrior    (15)
#define     I2C1_TxDmaInterruptPrior    (15)
#define     I2C1_ClockFreg              400000  //[Hz]

//I2C2
#define     I2C2_USE                    (1)
#define     I2C2_TC_HOOK                (0)
#define     I2C2_TxBffSz                (32)
#define     I2C2_RxBffSz                (32)
#define     I2C2_EventInterruptPrior    (12)
#define     I2C2_ErrorInterruptPrior    (12)
#define     I2C2_RxDmaInterruptPrior    (12)
#define     I2C2_TxDmaInterruptPrior    (12)
#define     I2C2_PINAFSCL               (4)
#define     I2C2_PINAFSDA               (4)
#define     I2C2_ClockFreg              400000  //[Hz]

//I2C3
#define     I2C3_USE                    0
#define     I2C3_TC_HOOK                1
#define     I2C3_TxBffSz                64
#define     I2C3_RxBffSz                32
#define     I2C3_EventInterruptPrior    15
#define     I2C3_ErrorInterruptPrior    15
#define     I2C3_RxDmaInterruptPrior    15
#define     I2C3_TxDmaInterruptPrior    15
#define     I2C3_ClockFreg              400000  //[Hz]

/*!****************************************************************************
 * User typedef
 */
typedef enum {
	i2cUnknownState, i2cFree, i2cTxRun, i2cRxRun, i2cDmaRefresh, i2cTxSuccess, i2cRxSuccess,

	i2cErrTx, i2cErrRx,

	i2cOk, i2cBusError, i2cAcknowledgeFailure, i2cPECError, i2cTimeout, i2cSoftTimeout
} i2cState_type;

typedef enum {
	i2cNeedStop, i2cWithoutStop
} i2c_stopMode_type;

typedef struct {
	uint32_t 				clockSpeed;     //[Hz]
	I2C_TypeDef 			*I2C;
	DMA_Stream_TypeDef 		*pDmaTxDmaCh;
	DMA_Stream_TypeDef 		*pDmaRxDmaCh;
	uint8_t 				slaveAdr;
	i2c_stopMode_type 		stopMode;
	volatile i2cState_type 	state;
} i2c_type;

/*!****************************************************************************
 * User enum
 */

/*!****************************************************************************
 * Extern viriables
 */
/*
 * i2c1 memory
 */
#if (I2C1_USE > 0)
extern i2c_type i2c1Sct;
extern i2c_type *i2c1;
extern uint8_t i2c1TxBff[I2C1_TxBffSz];
extern uint8_t i2c1RxBff[I2C1_RxBffSz];
#endif //I2C1_USE
/*
 * i2c2 memory
 */
#if (I2C2_USE > 0)
extern i2c_type i2c2Sct;
extern i2c_type *i2c2;
extern uint8_t i2c2TxBff[I2C2_TxBffSz];
extern uint8_t i2c2RxBff[I2C2_RxBffSz];
#endif //I2C2_USE
/*
 * i2c3 memory
 */
#if (I2C3_USE > 0)
extern i2c_type i2c3Sct;
extern i2c_type *i2c3;
extern uint8_t i2c3TxBff[I2C3_TxBffSz];
extern uint8_t i2c3RxBff[I2C3_RxBffSz];
#endif //I2C3_USE

/*!****************************************************************************
 * Macro functions
 */
static inline void i2c2TC_Hook(void){
	//BaseType_t  xHigherPriorityTaskWoken;
	//xHigherPriorityTaskWoken = pdFALSE;
	//xSemaphoreGiveFromISR(i2c2Sem, &xHigherPriorityTaskWoken);
	//if (xHigherPriorityTaskWoken != pdFALSE){
	//    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
	//}
}

/*!****************************************************************************
 * Prototypes for the functions
 */
void i2c_init(i2c_type *i2cx);
void i2c_reInit(i2c_type *i2cx);
void i2c_write(i2c_type *i2cx, void *src, uint16_t len, uint8_t slaveAdr, i2c_stopMode_type stopMode);
void i2c_read(i2c_type *i2cx, void *dst, uint16_t len, uint8_t slaveAdr);

#endif //i2c_H
/*************** (C) COPYRIGHT ************** END OF FILE ********* D_EL *****/

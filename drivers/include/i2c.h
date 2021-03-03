/*!****************************************************************************
 * @file		i2c.h
 * @author		d_el
 * @version		V1.6
 * @date		18.12.2017
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		Driver for I2C STM32F4 MCUs
 */
#ifndef i2c_H
#define i2c_H

#ifdef __cplusplus
extern "C" {
#endif

/*!****************************************************************************
 * Include
 */
#include "stm32f4xx.h"

/******************************************************************************
* User define
*/
#define I2C_CLOCK	42000000	//[Hz]

//I2C1
#define		I2C1_USE					(1)
#define		I2C1_TxBffSz				(32)
#define		I2C1_RxBffSz				(32)
#define		I2C1_EventInterruptPrior	(15)
#define		I2C1_ErrorInterruptPrior	(15)
#define		I2C1_RxDmaInterruptPrior	(15)
#define		I2C1_TxDmaInterruptPrior	(15)
#define		I2C1_PINAF					(4)
#define		I2C1_ClockFreg				(400000)	//[Hz]

//I2C2
#define		I2C2_USE					(0)
#define		I2C2_TxBffSz				(32)
#define		I2C2_RxBffSz				(32)
#define		I2C2_EventInterruptPrior	(15)
#define		I2C2_ErrorInterruptPrior	(15)
#define		I2C2_RxDmaInterruptPrior	(15)
#define		I2C1_PINAF					(4)
#define		I2C2_ClockFreg				(400000)	//[Hz]

//I2C3
#define		I2C3_USE					(0)
#define		I2C3_TxBffSz				(32)
#define		I2C3_RxBffSz				(32)
#define		I2C3_EventInterruptPrior	(15)
#define		I2C3_ErrorInterruptPrior	(15)
#define		I2C3_RxDmaInterruptPrior	(15)
#define		I2C1_PINAF					(4)
#define		I2C3_ClockFreg				(400000	//[Hz]

/******************************************************************************
* User typedef
*/
typedef enum{
	i2cUnknownState,
	i2cFree,
	i2cTxRun,
	i2cRxRun,
	i2cDmaRefresh,
	i2cTxSuccess,
	i2cRxSuccess,

	i2cErrTx,
	i2cErrRx,

	i2cOk,
	i2cBusError,
	i2cAcknowledgeFailure,
	i2cPECError,
	i2cTimeout,
	i2cSoftTimeout
}i2cState_type;

typedef enum{
    i2cNeedStop,
    i2cWithoutStop
}i2c_stopMode_type;

typedef struct i2cStruct{
	I2C_TypeDef			*pI2c;
	DMA_Stream_TypeDef	*pDmaStreamRx;
	DMA_Stream_TypeDef	*pDmaStreamTx;
	uint32_t			clockSpeed;		//[Hz]
	uint8_t				*pTxBff;
	uint8_t				*pRxBff;
	void (*tcHook)(struct i2cStruct *i2cx);
	uint8_t				dmaChannelRx	:4;
	uint8_t				dmaChannelTx	:4;
	uint8_t				slaveAdr;
	i2c_stopMode_type	stopMode;
	volatile i2cState_type	state;
}i2c_type;

typedef void (*i2cCallback_type)(i2c_type *i2cx);

/******************************************************************************
* External variables
*/
/*
* i2c1 memory
*/
#if (I2C1_USE == 1)
extern i2c_type		   *i2c1;
#endif //I2C1_USE

/******************************************************************************
* Macro functions
*/

/******************************************************************************
* Prototypes for the functions
*/
void i2c_init(i2c_type *i2cx);
void i2c_reInit(i2c_type *i2cx);
void i2c_setCallback(i2c_type *i2cx, i2cCallback_type tcHook);
void i2c_write(i2c_type *i2cx, void *src, uint16_t len, uint8_t slaveAdr, i2c_stopMode_type stopMode);
void i2c_read(i2c_type *i2cx, void *dst, uint16_t len, uint8_t slaveAdr);

#ifdef __cplusplus
}
#endif

#endif //i2c_H
/******************************** END OF FILE ********************************/

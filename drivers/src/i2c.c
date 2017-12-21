/*!****************************************************************************
 * @file		i2c.c
 * @author		d_el
 * @version		V1.5
 * @date		18.12.2017
 * @brief		Driver for I2C STM32F4 MCUs
 * @copyright	Copyright (C) 2017 Storozhenko Roman
 *				All rights reserved
 *				This software may be modified and distributed under the terms
 *				of the BSD license.	 See the LICENSE file for details
 */

/*!****************************************************************************
 * Include
 */
#include "i2c.h"

/**************************************
 * i2c1 memory
 */
#if (I2C1_USE == 1)
i2c_type		i2c1Sct;
i2c_type		*i2c1 = &i2c1Sct;
uint8_t			i2c1TxBff[I2C1_TxBffSz];
uint8_t			i2c1RxBff[I2C1_RxBffSz];
#endif //I2C1_USE

/**************************************
 * i2c2 memory
 */
#if (I2C2_USE == 1)
i2c_type		i2c2Sct;
i2c_type		*i2c2 = &i2c2Sct;
uint8_t			i2c2TxBff[I2C2_TxBffSz];
uint8_t			i2c2RxBff[I2C2_RxBffSz];
#endif //I2C2_USE

/**************************************
 * i2c3 memory
 */
#if (I2C3_USE == 1)
i2c_type		i2c3Sct;
i2c_type		*i2c3 = &i2c3Sct;
uint8_t			i2c3TxBff[I2C3_TxBffSz];
uint8_t			i2c3RxBff[I2C3_RxBffSz];
#endif //I2C3_USE

/******************************************************************************
 *
 */
void i2c_init(i2c_type *i2cx){
	uint32_t speed;

	#if (I2C1_USE == 1)
	if(i2cx == i2c1){
		/************************************************
		* Memory setting
		*/
		i2cx->pI2c			= I2C1;
		i2cx->pDmaStreamTx	= DMA1_Stream6;
		i2cx->pDmaStreamRx	= DMA1_Stream5;
		i2cx->dmaChannelTx	= 1;
		i2cx->dmaChannelRx	= 1;
		speed = I2C1_ClockFreg;

		/************************************************
		* IO
		* PB6, PB8 - clk
		* PB7, PB9 - sda
		*/
		gppin_init(GPIOB, 6, alternateFunctionOpenDrain, pullUp, 0, I2C1_PINAF);
		gppin_init(GPIOB, 7, alternateFunctionOpenDrain, pullUp, 0, I2C1_PINAF);

		/************************************************
		* NVIC
		*/
		NVIC_EnableIRQ(I2C1_EV_IRQn);			//Event interrupt enable
		NVIC_SetPriority(I2C1_EV_IRQn, I2C1_EventInterruptPrior);
		NVIC_EnableIRQ(I2C1_ER_IRQn);			//Error interrupt enable
		NVIC_SetPriority(I2C1_ER_IRQn, I2C1_ErrorInterruptPrior);

		NVIC_EnableIRQ(DMA1_Stream5_IRQn);		//RX
		NVIC_SetPriority(DMA1_Stream5_IRQn, I2C1_RxDmaInterruptPrior);
		NVIC_EnableIRQ(DMA1_Stream6_IRQn);		//TX
		NVIC_SetPriority(DMA1_Stream6_IRQn, I2C1_TxDmaInterruptPrior);

		/************************************************
		* I2C clock
		*/
		RCC->APB1RSTR |= RCC_APB1RSTR_I2C1RST;	//Reset
		RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C1RST;
		RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

		/************************************************
		* DMA clock
		*/
		RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
	}
	#endif //I2C1_USE

	#if (I2C2_USE == 1)
	if(i2cx == i2c2){
	}
	#endif //I2C2_USE

	#if (I2C3_USE == 1)
	if(i2cx == i2c3){
	}
	#endif //I2C3_USE

	/************************************************
	* I2C
	*/
	#define I2C_RISE_TIME(__FREQRANGE__, __SPEED__)			   (((__SPEED__) <= 100000U) ? ((__FREQRANGE__) + 1U) : ((((__FREQRANGE__) * 300U) / 1000U) + 1U))
	#define I2C_SPEED_FAST(__PCLK__, __SPEED__) (((__PCLK__) / ((__SPEED__) * 25U)))

	i2cx->pI2c->CR2 |= I2C_CR2_ITEVTEN;
	i2cx->pI2c->CR2 |= I2C_CR2_ITERREN;
	i2cx->pI2c->CR2 |= I2C_CR2_DMAEN;
	i2cx->pI2c->CR2 |= I2C_CR2_LAST;
	i2cx->pI2c->CR2 |= I2C_CLOCK / 1000000;

	i2cx->pI2c->CCR	|= I2C_CCR_FS;								   //Fast Mode I2C
	i2cx->pI2c->CCR	|= I2C_CCR_DUTY;							   //Fast Mode tlow/thigh = 16/9 (see CCR)
	i2cx->pI2c->CCR	|= I2C_CLOCK / (speed * 25);
	i2cx->pI2c->TRISE |= I2C_RISE_TIME(I2C_CLOCK / 1000000, speed);

	i2cx->pI2c->CR1 |= I2C_CR1_PE;

	i2cx->pI2c->CR1 |= I2C_CR1_ACK;

	/************************************************
	* DMA
	*/
	//DMA I2C TX
	i2cx->pDmaStreamTx->CR |= (uint32_t)((i2cx->dmaChannelTx & 0x03) << 25);  //Channel selection
	i2cx->pDmaStreamTx->CR |= DMA_SxCR_PL_1;							//Priority level High
	i2cx->pDmaStreamTx->CR &= ~DMA_SxCR_MSIZE;							//Memory data size 8 bit
	i2cx->pDmaStreamTx->CR &= ~DMA_SxCR_PSIZE;							//Memory data size 8 bit
	i2cx->pDmaStreamTx->CR |= DMA_SxCR_MINC;							//Memory increment mode enebled
	i2cx->pDmaStreamTx->CR &= ~DMA_SxCR_PINC;							//Peripheral increment mode disabled
	i2cx->pDmaStreamTx->CR |= DMA_SxCR_DIR_0;							//Direction Memory-to-peripheral
	i2cx->pDmaStreamTx->CR |= DMA_SxCR_TCIE;							//Transfer complete interrupt enable
	i2cx->pDmaStreamTx->PAR = (uint32_t)&i2cx->pI2c->DR;

	//DMA I2C RX
	i2cx->pDmaStreamRx->CR |= (uint32_t)((i2cx->dmaChannelRx & 0x03) << 25);  //Channel selection
	i2cx->pDmaStreamRx->CR |= DMA_SxCR_PL_1;							//Priority level High
	i2cx->pDmaStreamRx->CR &= ~DMA_SxCR_MSIZE;							//Memory data size 8 bit
	i2cx->pDmaStreamRx->CR &= ~DMA_SxCR_PSIZE;							//Memory data size 8 bit
	i2cx->pDmaStreamRx->CR |= DMA_SxCR_MINC;							//Memory increment mode enebled
	i2cx->pDmaStreamRx->CR &= ~DMA_SxCR_PINC;							//Peripheral increment mode disabled
	i2cx->pDmaStreamRx->CR &= ~DMA_SxCR_DIR;							//Direction Peripheral-to-memory
	i2cx->pDmaStreamRx->CR |= DMA_SxCR_TCIE;							//Transfer complete interrupt enable
	i2cx->pDmaStreamRx->PAR = (uint32_t)&i2cx->pI2c->DR;
}

/******************************************************************************
 *
 */
void i2c_reInit(i2c_type *i2cx){
	i2cx->pDmaStreamRx->CR	&= ~DMA_SxCR_EN;
	i2cx->pDmaStreamTx->CR	&= ~DMA_SxCR_EN;
	i2c_init(i2cx);
}

/*!****************************************************************************
 * @brief	Set callback I2C
 */
void i2c_setCallback(i2c_type *i2cx, i2cCallback_type tcHook){
	i2cx->tcHook = tcHook;
}

/******************************************************************************
 *
 */
void i2c_write(i2c_type *i2cx, void *src, uint16_t len, uint8_t slaveAdr){
	i2cx->pDmaStreamTx->CR		&= ~DMA_SxCR_EN;
	i2cx->pDmaStreamTx->M0AR	= (uint32_t)src;
	i2cx->pDmaStreamTx->NDTR	= len;
	i2cx->slaveAdr				= slaveAdr & 0xFE;				//Write
	i2cx->pDmaStreamTx->CR		|= DMA_SxCR_EN;
	i2cx->pI2c->CR1				 |= I2C_CR1_START;				//Generation start condition
	i2cx->state					= i2cTxRun;
}

/******************************************************************************
 *
 */
void i2c_read(i2c_type *i2cx, void *dst, uint16_t len, uint8_t slaveAdr){
	i2cx->pDmaStreamRx->CR		&= ~DMA_SxCR_EN;
	i2cx->pDmaStreamRx->M0AR	= (uint32_t)dst;
	i2cx->pDmaStreamRx->NDTR	= len;
	i2cx->slaveAdr				= slaveAdr | 0x01;				//Read
	i2cx->pDmaStreamRx->CR		|= DMA_SxCR_EN;
	I2C1->CR1					|= I2C_CR1_START;				//Generation start condition
	i2cx->state					= i2cRxRun;
}

/******************************************************************************
 * I2C EVENT HANDLER
 */
void i2cEventHendler(i2c_type *i2cx){
	volatile uint32_t I2Cx_SR1;

	I2Cx_SR1 = i2cx->pI2c->SR1;

	if(I2Cx_SR1 & I2C_SR1_SB){					//Start condition generated
		i2cx->pI2c->DR = i2c1Sct.slaveAdr;
		return;
	}
	if(I2Cx_SR1 & I2C_SR1_ADDR){				//ADDR is set
		(void)i2cx->pI2c->SR2;					//Clear ADDR flag by reading SR2 register
		return;
	}

	if(I2Cx_SR1 & I2C_SR1_BTF){					//Data byte transfer succeeded
		I2C1->CR1	|= I2C_CR1_STOP;
		while((I2C1->CR1 & I2C_CR1_STOP) != 0);
		if(i2cx->state == i2cTxRun){
			i2cx->state = i2cTxSuccess;
		}
		if(i2cx->tcHook != NULL){				//Call hook
			i2cx->tcHook(i2cx);
		}
	}
}

/******************************************************************************
 * I2C ERROR HANDLER
 */
void i2cErrorHendler(i2c_type *i2cx){
	volatile uint32_t I2Cx_SR1;

	I2Cx_SR1 = i2cx->pI2c->SR1;

	if((I2Cx_SR1 & I2C_SR1_BERR) != 0){
		i2cx->state = i2cBusError;
	}
	else if((I2Cx_SR1 & I2C_SR1_AF) != 0){
		i2cx->state = i2cAcknowledgeFailure;
	}
	else if((I2Cx_SR1 & I2C_SR1_PECERR) != 0){
		i2cx->state = i2cPECError;
	}
	i2cx->pI2c->SR1 = ~(I2C_SR1_TIMEOUT | I2C_SR1_PECERR | I2C_SR1_AF | I2C_SR1_ARLO | I2C_SR1_BERR);
}

/******************************************************************************
 * I2C1
 */
#if (I2C1_USE == 1)
void I2C1_EV_IRQHandler(void){
	i2cEventHendler(i2c1);
}
void I2C1_ER_IRQHandler(void){
	i2cErrorHendler(i2c1);
}
void DMA1_Stream5_IRQHandler(void){			//RX
	i2c1Sct.state = i2cRxSuccess;
	DMA1->HIFCR	 =	DMA_HIFCR_CTCIF5;		//Clear flag
}
void DMA1_Stream6_IRQHandler(void){			//TX
	DMA1->HIFCR	 =	DMA_HIFCR_CTCIF6;		//Clear flag
}
#endif

/******************************************************************************
 * I2C2
 */
#if (I2C2_USE == 1)
void I2C1_EV_IRQHandler(void){
	i2cEventHendler(i2c2);
}
void I2C1_ER_IRQHandler(void){
	i2cErrorHendler(i2c2);
}
void DMA1_Stream3_IRQHandler(void){			//RX
	DMA1->HIFCR	 =	DMA_HIFCR_CTCIF3;		//Clear flag
	i2c2Sct.state = i2cRxSuccess;
}
void DMA1_Stream7_IRQHandler(void){			//TX
	DMA1->HIFCR	 =	DMA_HIFCR_CTCIF7;		//Clear flag
}
#endif

/******************************************************************************
 * I2C3
 */
#if (I2C3_USE == 1)
void I2C1_EV_IRQHandler(void){
	i2cEventHendler(i2c3);
}
void I2C1_ER_IRQHandler(void){
	i2cErrorHendler(i2c3);
}
void DMA1_Stream2_IRQHandler(void){			//RX
	DMA1->HIFCR	 =	DMA_HIFCR_CTCIF2;		//Clear flag
	i2c3Sct.state = i2cRxSuccess;
}
void DMA1_Stream4_IRQHandler(void){			//TX
	DMA1->HIFCR	 =	DMA_HIFCR_CTCIF4;		//Clear flag
}
#endif

/***************** Copyright (C) Storozhenko Roman ******* END OF FILE *******/

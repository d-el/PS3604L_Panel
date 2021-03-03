/*!****************************************************************************
 * @file		crc.c
 * @author		d_el
 * @version		V1.5
 * @date		09.01.2016
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		Driver for uart STM32F4 MCUs
 */

/*!****************************************************************************
 * Include
 */
#include "stddef.h"
#include "gpio.h"
#include "uart.h"
#include "board.h"

/*!****************************************************************************
 * uart1 memory
 */
#if (UART1_USE > 0)
uart_type uart1Sct;
uart_type *uart1 = &uart1Sct;
uint8_t uart1TxBff[UART1_TxBffSz];
uint8_t uart1RxBff[UART1_RxBffSz];
#endif //UART1_USE

/*!****************************************************************************
 * uart3 memory
 */
#if (UART3_USE > 0)
uart_type uart3Sct;
uart_type *uart3 = &uart3Sct;
uint8_t uart3TxBff[UART3_TxBffSz];
uint8_t uart3RxBff[UART3_RxBffSz];
#endif //UART3_USE

/*!****************************************************************************
 * uart4 memory
 */
#if (UART4_USE > 0)
uart_type uart4Sct;
uart_type *uart4 = &uart4Sct;
uint8_t uart4TxBff[UART4_TxBffSz];
uint8_t uart4RxBff[UART4_RxBffSz];
#endif //UART4_USE

/*!****************************************************************************
 * @brief
 */
void uart_init(uart_type *uartx, uint32_t baudRate){
	uint32_t dmaChannelRx = 0;
	uint32_t dmaChannelTx = 0;

	#if(UART1_USE > 0)
	if(uartx == uart1){
		/************************************************
		 * Memory setting
		 */
		dmaChannelTx			= 4;
		dmaChannelRx			= 4;
		uartx->pUart			= USART1;
		uartx->pTxBff			= uart1TxBff;
		uartx->pRxBff			= uart1RxBff;
		uartx->pDmaStreamTx		= DMA2_Stream7;
		uartx->pDmaStreamRx		= DMA2_Stream2;
		uartx->dmaIfcrTx		= &DMA2->HIFCR;
		uartx->dmaIfcrRx		= &DMA2->LIFCR;
		uartx->dmaIfcrMaskTx	= DMA_HIFCR_CTCIF7/* | DMA_HIFCR_CTEIF7*/;
		uartx->dmaIfcrMaskRx	= DMA_LIFCR_CTCIF2/* | DMA_LIFCR_CTEIF2*/;
		uartx->frequency        = APB2_FREQ;

		#if(UART1_RX_IDLE_LINE_MODE > 0)
		uartx->rxIdleLineMode = 1;
		#endif

		/************************************************
		 * IO
		 */
		gppin_init(GPIOB, 6, alternateFunctionPushPull, pullDisable, 0, UART1_PINAFTX);		//PA9, PB6 USART1_TX
		#if(UART1_HALFDUPLEX == 0)
		gppin_init(GPIOB, 7, alternateFunctionPushPull, pullUp, 0, UART1_PINAFRX);			//PA10, PB7 USART1_RX
		#else
		uartx->halfDuplex = 1;
		#endif

		/************************************************
		 * NVIC
		 */
		NVIC_EnableIRQ(USART1_IRQn);
		NVIC_SetPriority(USART1_IRQn, UART1_TXIRQPrior);
		#if(UART1_RX_IDLE_LINE_MODE == 0)
		NVIC_EnableIRQ(DMA2_Stream2_IRQn);
		NVIC_SetPriority(DMA2_Stream2_IRQn, UART1_RxDmaInterruptPrior);
		#endif

		/************************************************
		 * USART clock
		 */
		RCC->APB2ENR  |= RCC_APB2ENR_USART1EN;										//USART clock enable
		RCC->APB2RSTR |= RCC_APB2RSTR_USART1RST;									//USART reset
		RCC->APB2RSTR &= ~RCC_APB2RSTR_USART1RST;

		/************************************************
		 * DMA clock
		 */
		RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
	}
	#endif //UART1_USE

	#if(UART3_USE > 0)
	if(uartx == uart3){
		/************************************************
		 * Memory setting
		 */
		dmaChannelTx			= 4;
		dmaChannelRx			= 4;
		uartx->pUart			= USART3;
		uartx->pTxBff			= uart3TxBff;
		uartx->pRxBff			= uart3RxBff;
		uartx->pDmaStreamTx		= DMA1_Stream3;
		uartx->pDmaStreamRx		= DMA1_Stream1;
		uartx->dmaIfcrTx		= &DMA1->LIFCR;
		uartx->dmaIfcrRx		= &DMA1->LIFCR;
		uartx->dmaIfcrMaskTx	= DMA_LIFCR_CTCIF3;
		uartx->dmaIfcrMaskRx	= DMA_LIFCR_CTCIF1;
		uartx->frequency        = APB1_FREQ;

		#if(UART3_RX_IDLE_LINE_MODE > 0)
		uartx->rxIdleLineMode = 1;
		#endif

		/************************************************
		 * IO
		 */
		gppin_init(GPIOD, 8, alternateFunctionOpenDrain, pullDisable, 0, UART3_PINAFTX);		//PD8, PC10 USART3_TX
		#if(UART3_HALFDUPLEX == 0)
		gppin_init(GPIOD, 9, alternateFunctionPushPull, pullUp, 0, UART3_PINAFRX);			//PD9, PC11 USART3_RX
		#else
		uartx->halfDuplex = 1;
		#endif

		/************************************************
		 * NVIC
		 */
		NVIC_EnableIRQ(USART3_IRQn);
		NVIC_SetPriority(USART3_IRQn, UART3_TXIRQPrior);
		#if(UART3_RX_IDLE_LINE_MODE == 0)
		NVIC_EnableIRQ(DMA1_Stream1_IRQn);
		NVIC_SetPriority(DMA1_Stream1_IRQn, UART3_RxDmaInterruptPrior);
		#endif

		/************************************************
		 * USART clock
		 */
		RCC->APB1ENR  |= RCC_APB1ENR_USART3EN;										//USART clock enable
		RCC->APB1RSTR |= RCC_APB1RSTR_USART3RST;									//USART reset
		RCC->APB1RSTR &= ~RCC_APB1RSTR_USART3RST;

		/************************************************
		 * DMA clock
		 */
		RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
	}
	#endif //UART3_USE

	#if(UART4_USE > 0)
	if(uartx == uart4){
		/************************************************
		 * Memory setting
		 */
		dmaChannelTx			= 4;
		dmaChannelRx			= 4;
		uartx->pUart			= UART4;
		uartx->pTxBff			= uart4TxBff;
		uartx->pRxBff			= uart4RxBff;
		uartx->pDmaStreamTx		= DMA1_Stream4;
		uartx->pDmaStreamRx		= DMA1_Stream2;
		uartx->dmaIfcrTx		= &DMA1->HIFCR;
		uartx->dmaIfcrRx		= &DMA1->LIFCR;
		uartx->dmaIfcrMaskTx	= DMA_HIFCR_CTCIF4;
		uartx->dmaIfcrMaskRx	= DMA_LIFCR_CTCIF2;
		uartx->frequency        = APB1_FREQ;

		#if(UART4_RX_IDLE_LINE_MODE > 0)
		uartx->rxIdleLineMode = 1;
		#endif

		/************************************************
		 * IO
		 */
		gppin_init(GPIOC, 10, alternateFunctionPushPull, pullDisable, 0, UART4_PINAFTX);		//PC10 USART4_TX
		#if(UART4_HALFDUPLEX == 0)
		gppin_init(GPIOC, 11, alternateFunctionPushPull, pullUp, 0, UART4_PINAFRX);				//PC11 USART4_RX
		#else
		uartx->halfDuplex = 1;
		#endif

		/************************************************
		 * NVIC
		 */
		NVIC_EnableIRQ(UART4_IRQn);
		NVIC_SetPriority(UART4_IRQn, UART4_TXIRQPrior);
		#if(UART4_RX_IDLE_LINE_MODE == 0)
		NVIC_EnableIRQ(DMA1_Stream2_IRQn);
		NVIC_SetPriority(DMA1_Stream2_IRQn, UART4_RxDmaInterruptPrior);
		#endif

		/************************************************
		 * USART clock
		 */
		RCC->APB1ENR  |= RCC_APB1ENR_UART4EN;										//USART clock enable
		RCC->APB1RSTR |= RCC_APB1RSTR_UART4RST;										//USART reset
		RCC->APB1RSTR &= ~RCC_APB1RSTR_UART4RST;

		/************************************************
		 * DMA clock
		 */
		RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
	}
	#endif //UART4_USE

	/************************************************
	 * USART
	 */
	if(uartx->halfDuplex != 0){
		uartx->pUart->CR3 |= USART_CR3_HDSEL;									//Half duplex mode is selected
	}
	uartx->pUart->CR1 |= USART_CR1_UE;											//UART enable
	uartx->pUart->CR1 &= ~USART_CR1_M;											//8bit
	uartx->pUart->CR1 &= ~USART_CR1_OVER8;										//Oversampling by 16
	uartx->pUart->CR2 &= ~USART_CR2_STOP;										//1 stop bit

	uart_setBaud(uartx, baudRate);												//Baud rate
	uartx->pUart->CR3 |= USART_CR3_DMAT;										//DMA enable transmitter
	uartx->pUart->CR3 |= USART_CR3_DMAR;										//DMA enable receiver

	uartx->pUart->CR1 |= USART_CR1_TE;											//Transmitter enable
	uartx->pUart->CR1 |= USART_CR1_RE;											//Receiver enable
	if(uartx->rxIdleLineMode != 0){
		//Clear IDLE flag by sequence (read USART_SR register followed by a read to the USART_DR register)
		(void)uartx->pUart->SR;
		(void)uartx->pUart->DR;
		uartx->pUart->CR1 |= USART_CR1_IDLEIE;
	}

	uartx->pUart->CR1 |= USART_CR1_TCIE;										//Enable the interrupt transfer complete

	/************************************************
	* DMA
	*/
	//DMA UART TX
	uartx->pDmaStreamTx->CR		= 0;
	uartx->pDmaStreamTx->CR		|= (uint32_t)((dmaChannelTx & 0x07) << DMA_SxCR_CHSEL_Pos);		//Channel selection
	uartx->pDmaStreamTx->CR		|= DMA_SxCR_PL_1;								//Priority level High
	uartx->pDmaStreamTx->CR		&= ~DMA_SxCR_MSIZE;								//Memory data size 8 bit
	uartx->pDmaStreamTx->CR		&= ~DMA_SxCR_PSIZE;								//Peripheral data size 8 bit
	uartx->pDmaStreamTx->CR		|= DMA_SxCR_MINC;								//Memory increment mode enabled
	uartx->pDmaStreamTx->CR		&= ~DMA_SxCR_PINC;								//Peripheral increment mode disabled
	uartx->pDmaStreamTx->CR		|= DMA_SxCR_DIR_0;								//Direction Memory-to-peripheral
	uartx->pDmaStreamTx->PAR	= (uint32_t)&uartx->pUart->DR;					//Peripheral address
	uartx->pDmaStreamTx->M0AR	= (uint32_t)NULL;								//Memory address
	//DMA UART RX
	uartx->pDmaStreamRx->CR		= 0;
	uartx->pDmaStreamRx->CR		|= (uint32_t)((dmaChannelRx & 0x07) << DMA_SxCR_CHSEL_Pos);		//Channel selection
	uartx->pDmaStreamRx->CR		|= DMA_SxCR_PL_1;								//Priority level High
	uartx->pDmaStreamRx->CR		&= ~DMA_SxCR_MSIZE;								//Memory data size 8 bit
	uartx->pDmaStreamRx->CR		&= ~DMA_SxCR_PSIZE;								//Peripheral data size 8 bit
	uartx->pDmaStreamRx->CR		|= DMA_SxCR_MINC;								//Memory increment mode enabled
	uartx->pDmaStreamRx->CR		&= ~DMA_SxCR_PINC;								//Peripheral increment mode disabled
	uartx->pDmaStreamRx->CR		&= ~DMA_SxCR_DIR;								//Direction Peripheral-to-memory
	uartx->pDmaStreamRx->CR		|= DMA_SxCR_TCIE;								//Transfer complete interrupt enable
	uartx->pDmaStreamRx->PAR	= (uint32_t)&uartx->pUart->DR;					//Peripheral address
	uartx->pDmaStreamRx->M0AR	= (uint32_t)NULL;								//Memory address
}

/*!****************************************************************************
 * @brief
 */
void uart_deinit(uart_type *uartx){
	(void)uartx;
	while(1);
}

/*!****************************************************************************
* @brief	transfer data buffer
*/
void uart_setBaud(uart_type *uartx, uint32_t baudRate){
	if(uartx->baudRate != baudRate){
		uartx->pUart->BRR = uartx->frequency / baudRate;
		uartx->baudRate = baudRate;
	}
}

/*!****************************************************************************
 * @brief	Set callback uart
 */
void uart_setCallback(uart_type *uartx, uartCallback_type txHoock, uartCallback_type rxHoock){
	uartx->txHoock = txHoock;
	uartx->rxHoock = rxHoock;
}

/*!****************************************************************************
 * @brief
 */
void uart_write(uart_type *uartx, void const *src, uint16_t len){
	uartx->pDmaStreamTx->CR		&= ~DMA_SxCR_EN;									//Channel disabled
	uartx->pUart->SR = 0;
	uartx->pDmaStreamTx->M0AR	= (uint32_t)src;									//Memory address
	uartx->pDmaStreamTx->NDTR	= len;												//Number of data
	uartx->pDmaStreamTx->CR		|= DMA_SxCR_EN;										//Channel enabled
	uartx->txState				= uartTxRun;
}

/******************************************************************************
 * @brief
 */
void uart_read(uart_type *uartx, void *dst, uint16_t len){
	uartx->pDmaStreamRx->CR		&= ~DMA_SxCR_EN;									//Channel disabled
	uartx->pUart->SR = 0;
	uartx->pDmaStreamRx->M0AR	= (uint32_t)dst;									//Memory address
	uartx->pDmaStreamRx->NDTR	= len;												//Number of data.
	uartx->pDmaStreamRx->CR		|= DMA_SxCR_EN;										//Channel enabled
	uartx->rxState				= uartRxRun;
}

/******************************************************************************
 * @brief
 */
void uart_stopRead(uart_type *uartx){
	uartx->pDmaStreamRx->CR &= ~DMA_SxCR_EN;									//Channel disabled
	uartx->rxState = uartRxStop;
}

/******************************************************************************
 * Transfer complete interrupt (USART TX and IDLE RX)
 */
void USART_IRQHandler(uart_type *uartx){
	uint16_t uartsr = uartx->pUart->SR;

	/************************************************
	 * USART TRANSFER COMPLETE
	 */
	if((uartsr & USART_SR_TC) != 0){
		uartx->pDmaStreamTx->CR &= ~DMA_SxCR_EN;										//Channel disabled
		uartx->txCnt++;
		uartx->txState = uartTxSuccess;
		*uartx->dmaIfcrTx = uartx->dmaIfcrMaskTx;											//Clear flag
		uartx->pUart->SR &= ~USART_SR_TC;
		if(uartx->txHoock != NULL){
			uartx->txHoock(uartx);
		}
	}
	/************************************************
	 * USART IDLE LINE interrupt
	 */
	else if((uartsr & USART_SR_IDLE) != 0){
		uartx->pDmaStreamRx->CR &= ~DMA_SxCR_EN;									   //Channel disabled
		uartx->rxCnt++;
		uartx->rxState = uartRxSuccess;
		if(uartx->rxHoock != NULL){
			uartx->rxHoock(uartx);
		}
		*uartx->dmaIfcrRx = uartx->dmaIfcrMaskRx;										//Clear flag
		//Clear IDLE flag by sequence (read USART_SR register followed by a read to the USART_DR register)
		(void)uartx->pUart->SR;
		(void)uartx->pUart->DR;
	}
}

/******************************************************************************
 * Transfer complete interrupt (USART RX)
 */
void DmaStreamRxIRQHandler(uart_type *uartx){
	uartx->pDmaStreamRx->CR &= ~DMA_SxCR_EN;											//Channel disabled
	uartx->rxCnt++;
	uartx->rxState = uartRxSuccess;
	if(uartx->rxHoock != NULL){
		uartx->rxHoock(uartx);
	}
	*uartx->dmaIfcrRx = uartx->dmaIfcrMaskRx;											//Clear flag
}

/******************************************************************************
 * UART1
 */
#if (UART1_USE > 0)
void USART1_IRQHandler(void){
	USART_IRQHandler(uart1);
}
#if (UART1_RX_IDLE_LINE_MODE == 0)
void DMA2_Stream2_IRQHandler(void){
	DmaStreamRxIRQHandler(uart1);											//Clear flag
}
#endif
#endif

/******************************************************************************
 * UART3
 */
#if (UART3_USE > 0)
void USART3_IRQHandler(void){
	USART_IRQHandler(uart3);
}
#if (UART3_RX_IDLE_LINE_MODE == 0)
void DMA1_Stream1_IRQHandler(void){
	DmaStreamRxIRQHandler(uart3);											//Clear flag
}
#endif
#endif

/******************************************************************************
 * UART4
 */
#if (UART4_USE > 0)
void UART4_IRQHandler(void){
	USART_IRQHandler(uart4);
}
#if (UART4_RX_IDLE_LINE_MODE == 0)
void DMA1_Stream2_IRQHandler(void){
	DmaStreamRxIRQHandler(uart4);											//Clear flag
}
#endif
#endif

/******************************** END OF FILE ********************************/

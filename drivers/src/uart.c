/*!****************************************************************************
 * @file		uart.c
 * @author		d_el - Storozhenko Roman
 * @version		V1.5
 * @date    	09.01.2016
 * @copyright	GNU Lesser General Public License v3
 * @brief		Driver for uart STM32L4 MCUs
 *
 * @history 26.03.2016 - remade for new gpio driver
 * @history 24.09.2016 - rx isr, uart write
 * @history 05.08.2017 - make callback from pointers
 */

/*!****************************************************************************
 * Include
 */
#include "uart.h"

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
 * uart2 memory
 */
#if (UART2_USE > 0)
uart_type uart2Sct;
uart_type *uart2 = &uart2Sct;
uint8_t uart2TxBff[UART2_TxBffSz];
uint8_t uart2RxBff[UART2_RxBffSz];
#endif //UART2_USE

/*!****************************************************************************
 * uart3 memory
 */
#if (UART3_USE > 0)
uart_type uart3Sct;
uart_type *uart3 = &uart3Sct;
volatile uint8_t uart3TxBff[UART3_TxBffSz];
volatile uint8_t uart3RxBff[UART3_RxBffSz];
#endif //UART3_USE


#define UART_FREQ 				(16000000U)		///< [Hz]
#define uartMakeMantissa(baud)	(UART_FREQ / 16 / (baud))
#define uartMakeFraction(baud)	(((UART_FREQ + (baud) / 2)  / (baud)) - (uartMakeMantissa(baud) * 16))
#define uartMakeBrr(baud)		(uartMakeMantissa(baud) << USART_BRR_DIV_Mantissa_Pos | uartMakeFraction(baud))

uint32_t usartBaudRate[] = {
		9600,
		38400,
		57600,
		115200,
};

uint16_t usartBaudRateDiv[] = {
	uartMakeBrr(9600),
	uartMakeBrr(38400),
	uartMakeBrr(57600),
	uartMakeBrr(115200),
};

/*!****************************************************************************
 * @brief
 */
void uart_init(uart_type *uartx, uartBaudRate_type baudRate){
    uint32_t            dmaChannelRx;
    uint32_t            dmaChannelTx;

	#if(UART1_USE > 0)
	if(uartx == uart1){
		/************************************************
		 * Memory setting
		 */
		uartx->pUart        = USART1;
		uartx->pTxBff       = uart1TxBff;
		uartx->pRxBff       = uart1RxBff;
		uartx->pDmaStreamTx = DMA2_Stream7;
		uartx->pDmaStreamRx = DMA2_Stream2;
        dmaChannelTx       	= 4;
        dmaChannelRx       	= 4;
		#if(UART1_RX_IDLE_LINE_MODE > 0)
		uartx->rxIdleLineMode = 1;
		#endif

		/************************************************
		 * IO
		 */
		gppin_init(GPIOB, 6, alternateFunctionPushPull, pullDisable, 0, UART1_PINAFTX);		//PA9, PB6 USART1_TX
		#if(UART1_HALFDUPLEX == 0)
		gppin_init(GPIOB, 7, alternateFunctionPushPull, pullUp, 0, UART1_PINAFRX); 	//PA10, PB7 USART1_RX
		#else
		uartx->halfDuplex = 1;
		#endif

		/************************************************
		 * NVIC
		 */
		NVIC_EnableIRQ(USART1_IRQn);
		NVIC_SetPriority(USART1_IRQn, UART1_TXIRQPrior);
		#if(UART1_RX_IDLE_LINE_MODE == 0)
		NVIC_EnableIRQ(DMA2_Stream2_IRQn);                                 		            //Р’РєР»СЋС‡РёС‚СЊ РїСЂРµСЂС‹РІР°РЅРёСЏ РѕС‚ DMA1_Channel 5
		NVIC_SetPriority(DMA2_Stream2_IRQn, UART1_RxDmaInterruptPrior);			            //РЈСЃС‚Р°РЅРѕРІРёС‚СЊ РїСЂРёРѕСЂРёС‚РµС‚
		#endif

		/************************************************
		 * USART clock
		 */
		RCC->APB2ENR  |= RCC_APB2ENR_USART1EN;                            			//USART1 clock enable
		RCC->APB2RSTR |= RCC_APB2RSTR_USART1RST;//USART1 reset
		RCC->APB2RSTR &= ~RCC_APB2RSTR_USART1RST;

		/************************************************
		 * DMA clock
		 */
		RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;

		/************************************************
		 * DMA request settings
		 */
		//DMA2_CSELR->CSELR &= ~DMA_CSELR_C6S_Msk;        							//Channel 4 clear
		//DMA2_CSELR->CSELR |= 0x2 << DMA_CSELR_C6S_Pos;								//Channel 4 mapped on USART1_TX
		//DMA2_CSELR->CSELR &= ~DMA_CSELR_C7S_Msk;									//Channel 5 clear
		//DMA2_CSELR->CSELR |= 0x2 << DMA_CSELR_C7S_Pos;								//Channel 5 mapped on USART1_RX
	}
	#endif //UART1_USE

	#if(UART2_USE > 0)
	if(uartx == uart2){
		/************************************************
		 * Memory setting
		 */
		uartx->pUart = USART2;
		uartx->pTxBff = uart2TxBff;
		uartx->pRxBff = uart2RxBff;
		uartx->pDmaStreamTx = DMA1_Channel7;
		uartx->pDmaStreamRx = DMA1_Channel6;
		#if (UART2_RX_IDLE_LINE_MODE > 0)
		uartx->rxIdleLineMode = 1;
		#endif

		/************************************************
		 * IO
		 */
		gppin_init(GPIOA, 2, alternateFunctionPushPull, pullDisable, 0, UART2_PINAFTX); //PA2 USART2_TX
		#if(UART2_HALFDUPLEX == 0)
		gppin_init(GPIOA, 3, alternateFunctionPushPull, pullDisable, 0, UART2_PINAFRX); //PA3 USART2_RX
		#else
		uartx->halfDuplex = 1;
		#endif

		/************************************************
		 * NVIC
		 */
		NVIC_EnableIRQ(USART2_IRQn);
		NVIC_SetPriority(USART2_IRQn, UART2_TXIRQPrior);
		#if (UART2_RX_IDLE_LINE_MODE == 0)
		NVIC_EnableIRQ(DMA1_Channel6_IRQn);                                 	//Р’РєР»СЋС‡РёС‚СЊ РїСЂРµСЂС‹РІР°РЅРёСЏ РѕС‚ DMA1_Channel 6
		NVIC_SetPriority(DMA1_Channel6_IRQn, UART2_RxDmaInterruptPrior);		//РЈСЃС‚Р°РЅРѕРІРёС‚СЊ РїСЂРёРѕСЂРёС‚РµС‚
		#endif

		/************************************************
		 * USART clock
		 */
		RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;                            		//USART1 clock enable
		RCC->APB1RSTR1 |= RCC_APB1RSTR1_USART2RST;                          	//USART1 reset
		RCC->APB1RSTR1 &= ~RCC_APB1RSTR1_USART2RST;

		/************************************************
		 * DMA clock
		 */
		RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

		/************************************************
		 * DMA request settings
		 */
		DMA1_CSELR->CSELR &= ~DMA_CSELR_C7S_Msk;        						//Channel 7 clear
		DMA1_CSELR->CSELR |= 0x2 << DMA_CSELR_C7S_Pos;  						//Channel 7 mapped on USART1_TX
		DMA1_CSELR->CSELR &= ~DMA_CSELR_C6S_Msk;        						//Channel 6 clear
		DMA1_CSELR->CSELR |= 0x2 << DMA_CSELR_C6S_Pos;  						//Channel 6 mapped on USART1_RX
	}
	#endif //UART2_USE

	#if(UART3_USE > 0)
	if(uartx == uart3){
		/************************************************
		 * Memory setting
		 */
		uartx->pUart = USART3;
		uartx->pTxBff = (uint8_t*)uart3TxBff;
		uartx->pRxBff = (uint8_t*)uart3RxBff;
		uartx->pDmaStreamTx = DMA1_Channel2;
		uartx->pDmaStreamRx = DMA1_Channel3;
		#if (UART3_RX_IDLE_LINE_MODE > 0)
		uartx->rxIdleLineMode = 1;
		#endif

		/************************************************
		 * IO
		 */
		gppin_init(GPIOB, 10, alternateFunctionPushPull, pullDisable, 0, UART3_PINAFTX);  //PB10 USART3_TX
		#if (UART2_HALFDUPLEX == 0)
		gppin_init(GPIOB, 11, alternateFunctionPushPull, pullDisable, 0, UART3_PINAFRX);  //PB11 USART3_RX
		#else
		uartx->halfDuplex = 1;
		#endif

		/************************************************
		 * NVIC
		 */
		NVIC_EnableIRQ(USART3_IRQn);
		NVIC_SetPriority(USART3_IRQn, UART3_TXIRQPrior);
		#if(UART3_RX_IDLE_LINE_MODE == 0)
		NVIC_EnableIRQ(DMA1_Channel3_IRQn);                                 	//Р’РєР»СЋС‡РёС‚СЊ РїСЂРµСЂС‹РІР°РЅРёСЏ РѕС‚ DMA1_Channel 3
		NVIC_SetPriority(DMA1_Channel3_IRQn, UART3_RxDmaInterruptPrior);		//РЈСЃС‚Р°РЅРѕРІРёС‚СЊ РїСЂРёРѕСЂРёС‚РµС‚
		#endif

		/************************************************
		 * USART clock
		 */
		RCC->APB1ENR1 |= RCC_APB1ENR1_USART3EN;                            		//USART3 clock enable
		RCC->APB1RSTR1 |= RCC_APB1RSTR1_USART3RST;								//USART3 reset
		RCC->APB1RSTR1 &= ~RCC_APB1RSTR1_USART3RST;

		/************************************************
		 * DMA clock
		 */
		RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

		/************************************************
		 * DMA request settings
		 */
		DMA1_CSELR->CSELR &= ~DMA_CSELR_C2S_Msk;        						//Channel 2 clear
		DMA1_CSELR->CSELR |= 0x2 << DMA_CSELR_C2S_Pos;							//Channel 2 mapped on USART3_TX
		DMA1_CSELR->CSELR &= ~DMA_CSELR_C3S_Msk;								//Channel 3 clear
		DMA1_CSELR->CSELR |= 0x2 << DMA_CSELR_C3S_Pos;							//Channel 3 mapped on USART3_RX
	}
	#endif //UART3_USE

	/************************************************
	 * USART
	 */
	if(uartx->halfDuplex != 0){
		uartx->pUart->CR3 |= USART_CR3_HDSEL;                           		//Half duplex mode is selected
	}
	uartx->pUart->CR1 |= USART_CR1_UE;                                			//UART enable
	uartx->pUart->CR1 &= ~USART_CR1_M;                                			//8bit
    uartx->pUart->CR1 &= ~USART_CR1_OVER8;                                      //Oversampling by 16
	uartx->pUart->CR2 &= ~USART_CR2_STOP;                             			//1 stop bit

	uartx->pUart->BRR = usartBaudRateDiv[baudRate];                   			//Baud rate
	uartx->pUart->CR3 |= USART_CR3_DMAT;                              			//DMA enable transmitter
	uartx->pUart->CR3 |= USART_CR3_DMAR;                              			//DMA enable receiver

	uartx->pUart->CR1 |= USART_CR1_TE;                                			//Transmitter enable
	uartx->pUart->CR1 |= USART_CR1_RE;                                			//Receiver enable
	//uartx->pUart->ICR = USART_ICR_TCCF | USART_ICR_IDLECF;            			//Clear the flags
	if(uartx->rxIdleLineMode != 0){
		//uartx->pUart->ICR = USART_ICR_IDLECF;									//Clear flag
		uartx->pUart->CR1 |= USART_CR1_IDLEIE;
	}

	uartx->pUart->CR1 |= USART_CR1_TCIE;                              			//Enable the interrupt transfer complete

    /************************************************
    * DMA
    */
    //DMA UART TX
    uartx->pDmaStreamTx->CR 	= 0;
    uartx->pDmaStreamTx->CR 	|= (uint32_t)((dmaChannelTx & 0x07) << DMA_SxCR_CHSEL_Pos);     //Channel selection
    uartx->pDmaStreamTx->CR 	|= DMA_SxCR_PL_1;                               //Priority level High
    uartx->pDmaStreamTx->CR 	&= ~DMA_SxCR_MSIZE;                             //Memory data size 8 bit
    uartx->pDmaStreamTx->CR 	&= ~DMA_SxCR_PSIZE;                             //Memory data size 8 bit
    uartx->pDmaStreamTx->CR 	|= DMA_SxCR_MINC;                               //Memory increment mode enabled
    uartx->pDmaStreamTx->CR 	&= ~DMA_SxCR_PINC;                              //Peripheral increment mode disabled
    uartx->pDmaStreamTx->CR 	|= DMA_SxCR_DIR_0;                              //Direction Memory-to-peripheral
    uartx->pDmaStreamTx->PAR    = (uint32_t)&uartx->pUart->DR;                	//Peripheral address
    uartx->pDmaStreamTx->M0AR   = (uint32_t)NULL;                          		//Memory address
    //DMA UART RX
    uartx->pDmaStreamRx->CR 	= 0;
    uartx->pDmaStreamRx->CR 	|= (uint32_t)((dmaChannelRx & 0x07) << DMA_SxCR_CHSEL_Pos);     //Channel selection
    uartx->pDmaStreamRx->CR 	|= DMA_SxCR_PL_1;                               //Priority level High
    uartx->pDmaStreamRx->CR 	&= ~DMA_SxCR_MSIZE;                             //Memory data size 8 bit
    uartx->pDmaStreamRx->CR 	&= ~DMA_SxCR_PSIZE;                             //Memory data size 8 bit
    uartx->pDmaStreamRx->CR 	|= DMA_SxCR_MINC;                               //Memory increment mode enabled
    uartx->pDmaStreamRx->CR 	&= ~DMA_SxCR_PINC;                              //Peripheral increment mode disabled
    uartx->pDmaStreamRx->CR 	&= ~DMA_SxCR_DIR;                               //Direction Peripheral-to-memory
    uartx->pDmaStreamRx->CR 	|= DMA_SxCR_TCIE;                               //Transfer complete interrupt enable
    uartx->pDmaStreamRx->PAR 	= (uint32_t)&uartx->pUart->DR;                	//Peripheral address
    uartx->pDmaStreamRx->M0AR 	= (uint32_t)NULL;                          		//Memory address
}

/*!****************************************************************************
 * @brief
 */
void uart_deinit(uart_type *uartx){
//	uartx->pUartTxDmaCh->CCR &= ~DMA_CCR_EN;               						//Channel disabled
//	uartx->pUartRxDmaCh->CCR &= ~DMA_CCR_EN;               						//Channel disabled
//	#if (UART1_USE > 0)
//	if(uartx->pUart == USART1){
//		RCC->APB2ENR &= ~RCC_APB2ENR_USART1EN;           					//USART1 clock disable
//		NVIC_DisableIRQ(DMA1_Channel4_IRQn);
//		NVIC_DisableIRQ(DMA1_Channel5_IRQn);
//	}
//	#endif //UART1_USE
//
//	#if (UART2_USE > 0)
//	if(uartx->pUart == USART2){
//		RCC->APB1ENR1 &= ~RCC_APB1ENR1_USART2EN;           					//USART1 clock disable
//		NVIC_DisableIRQ(DMA1_Channel6_IRQn);
//		NVIC_DisableIRQ(DMA1_Channel7_IRQn);
//	}
//	#endif //UART2_USE
//
//	#if (UART3_USE > 0)
//	if(uartx->pUart == USART3){
//		RCC->APB1ENR1 &= ~RCC_APB1ENR1_USART3EN;       						//USART1 clock disable
//		NVIC_DisableIRQ(DMA1_Channel2_IRQn);
//		NVIC_DisableIRQ(DMA1_Channel3_IRQn);
//	}
//	#endif //UART3_USE
}

/*!****************************************************************************
* @brief    transfer data buffer
*/
void uart_setBaud(uart_type *uartx, uartBaudRate_type baudRate){
    if((uartx->baudRate != baudRate)&&(baudRate < BR_NUMBER)){
        uartx->pUart->BRR   = usartBaudRateDiv[baudRate];
        uartx->baudRate     = baudRate;
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
void uart_write(uart_type *uartx, void *src, uint16_t len){
	uartx->pDmaStreamTx->CR     &= ~DMA_SxCR_EN;                               		//Channel disabled
	uartx->pDmaStreamTx->M0AR   = (uint32_t)src;                             		//Memory address
	uartx->pDmaStreamTx->NDTR   = len;                                      		//Number of data
	uartx->pDmaStreamTx->CR     |= DMA_SxCR_EN;                                		//Channel enabled
	uartx->txState              = uartTxRun;
}

/******************************************************************************
 * @brief
 */
void uart_read(uart_type *uartx, void *dst, uint16_t len){
	//uartx->pUart->ICR = 0xFFFFFFFFU;                                       		//Clear all flags
	//(void) uartx->pUart->RDR;
	//uartx->pUart->ICR = 0xFFFFFFFFU;
	uartx->pDmaStreamRx->CR     &= ~DMA_SxCR_EN;                                    //Channel disabled
	uartx->pDmaStreamRx->M0AR   = (uint32_t)dst;                             		//Memory address
	uartx->pDmaStreamRx->NDTR   = len;                                      		//Number of data.
	uartx->pDmaStreamRx->CR     |= DMA_SxCR_EN;                                		//Channel enabled
	uartx->rxState              = uartRxRun;
}

/******************************************************************************
 * @brief
 */
void uart_stopRead(uart_type *uartx){
	uartx->pDmaStreamRx->CR &= ~DMA_SxCR_EN;                               		//Channel disabled
	uartx->rxState          = uartRxStop;
}

/******************************************************************************
 * Transfer complete interrupt USART1_IRQn (USART1 TX and IDLE RX)
 */
#if (UART1_USE > 0)
void USART1_IRQHandler(void){
	uint16_t uartsr = uart1->pUart->SR;

	/************************************************
	 * USART TRANSFER COMPLETE
	 */
	if((uartsr & USART_SR_TC) != 0){
		DMA2_Stream7->CR &= ~DMA_SxCR_EN;                                     	//Channel disabled
		uart1->txCnt++;
		uart1->txState = uartTxSuccess;
		if(uart1->txHoock != NULL){
			uart1->txHoock(uart1);
		}
		DMA2->HIFCR = DMA_HIFCR_CTCIF7;                               		    //Clear flag
		uart1->pUart->SR &= ~USART_SR_TC;
	}
	/************************************************
	 * USART IDLE LINE interrupt
	 */
#if (UART1_RX_IDLE_LINE_MODE > 0)
	if((uartsr & USART_SR_IDLE) != 0){
		DMA2_Stream2->CR &= ~DMA_SxCR_EN;                                       //Channel disabled
		uart1->rxCnt++;
		uart1->rxState = uartRxSuccess;
		if(uart1->rxHoock != NULL){
			uart1->rxHoock(uart1);
		}
		DMA2->LIFCR = DMA_LIFCR_CTCIF2;                                  		//Clear flag
		//Clear IDLE flag by sequence (read USART_SR register followed by a read to the USART_DR register)
		(void)uart1->pUart->SR;
		(void)uart1->pUart->DR;
	}
#endif
}
/******************************************************************************
 * Transfer complete interrupt DMA1_Channel5 (USART1 RX)
 */
#if (UART1_RX_IDLE_LINE_MODE == 0)
void DMA2_Stream2_IRQHandler(void){
	DMA2_Stream2->CR &= ~DMA_SxCR_EN;                                     		//Channel disabled
	uart1->rxCnt++;
	uart1->rxState = uartRxSuccess;
	if(uart1->rxHoock != NULL){
		uart1->rxHoock(uart1);
	}
	DMA2->LIFCR = DMA_LIFCR_CTCIF2;                                      	    //Clear flag
}
#endif //(UART1_RX_IDLE_LINE_MODE == 0)
#endif //UART1_USE

/******************************************************************************
 * Transfer complete interrupt USART2_IRQn (USART2 TX and IDLE RX)
 */
#if (UART2_USE > 0)
void USART2_IRQHandler(void){
	uint16_t uartsr = uart2->pUart->ISR;

	/************************************************
	 * USART TRANSFER COMPLETE
	 */
	if((uartsr & USART_ISR_TC) != 0){
		DMA1_Channel7->CCR &= ~DMA_CCR_EN;                                     	//Channel disabled
		uart2->txCnt++;
		uart2->txState = uartTxSuccess;
		if(uart2->txHoock != NULL){
			uart2->txHoock(uart2);
		}
		DMA1->IFCR = DMA_IFCR_CTCIF7;                                      		//Clear flag
		uart2->pUart->ICR |= USART_ICR_TCCF;
	}
	/************************************************
	 * USART IDLE LINE interrupt
	 */
#if (UART2_RX_IDLE_LINE_MODE > 0)
	if((uartsr & USART_ISR_IDLE) != 0){
		DMA1_Channel6->CCR &= ~DMA_CCR_EN;                                     //Channel disabled
		uart2->rxCnt++;
		uart2->rxState = uartRxSuccess;
		if(uart2->rxHoock != NULL){
			uart2->rxHoock(uart2);
		}
		DMA1->IFCR = DMA_IFCR_CTCIF6;                                      		//Clear flag
		uart2->pUart->ICR = USART_ICR_IDLECF;
	}
#endif
}
/******************************************************************************
 * Transfer complete interrupt DMA1_Channel6 (USART2 RX)
 */
#if (UART2_RX_IDLE_LINE_MODE == 0)
void DMA1_Channel6_IRQHandler(void){
	DMA1_Channel6->CCR &= ~DMA_CCR_EN;                                     		//Channel disabled
	uart2->rxCnt++;
	uart2->rxState = uartRxSuccess;
	if(uart2->rxHoock != NULL){
		uart2->rxHoock(uart2);
	}
	DMA1->IFCR = DMA_IFCR_CTCIF6;                                      			//Clear flag
}
#endif //(UART2_RX_IDLE_LINE_MODE == 0)
#endif //UART2_USE

/******************************************************************************
 * Transfer complete interrupt USART3_IRQn (USART3 TX and IDLE RX)
 */
#if (UART3_USE > 0)
void USART3_IRQHandler(void){
	uint16_t uartsr = uart3->pUart->ISR;

	/************************************************
	 * USART TRANSFER COMPLETE
	 */
	if((uartsr & USART_ISR_TC) != 0){
		DMA1_Channel2->CCR &= ~DMA_CCR_EN;                           			//Channel disabled
		uart3->txCnt++;
		uart3->txState = uartTxSuccess;
		if(uart3->txHoock != NULL){
			uart3->txHoock(uart3);
		}
		DMA1->IFCR = DMA_IFCR_CTCIF2;                               			//Clear flag
		uart3->pUart->ICR |= USART_ICR_TCCF;
	}
	/************************************************
	 * USART IDLE LINE interrupt
	 */
#if (UART3_RX_IDLE_LINE_MODE > 0)
	if((uartsr & USART_ISR_IDLE) != 0){
		DMA1_Channel3->CCR &= ~DMA_CCR_EN;                                 		//Channel disabled
		uart3->rxCnt++;
		uart3->rxState = uartRxSuccess;
		if(uart3->rxHoock != NULL){
			uart3->rxHoock(uart3);
		}
		DMA1->IFCR = DMA_IFCR_CTCIF3;                             				//Clear flag
		uart3->pUart->ICR = USART_ICR_IDLECF;
	}
#endif
}
/******************************************************************************
 * Transfer complete interrupt DMA1_Channel3 (USART3 RX)
 */
#if (UART3_RX_IDLE_LINE_MODE == 0)
void DMA1_Channel3_IRQHandler(void){
	DMA1_Channel3->CCR &= ~DMA_CCR_EN;                                     		//Channel disabled
	uart3->rxCnt++;
	uart3->rxState = uartRxSuccess;
	if(uart3->rxHoock != NULL){
		uart3->rxHoock(uart3);
	}
	DMA1->IFCR = DMA_IFCR_CTCIF3;                                      			//Clear flag
}
#endif //(UART2_RX_IDLE_LINE_MODE == 0)
#endif //UART3_USE

/******************* (C) COPYRIGHT ***************** END OF FILE ********* D_EL *****/

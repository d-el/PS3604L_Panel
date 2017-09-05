/*!****************************************************************************
 * @file		uart.h
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
#ifndef UART_H
#define UART_H

/*!****************************************************************************
 * Include
 */
#include "stm32f4xx.h"
#include "gpio.h"
#include "stdint.h"
#include "stdio.h"

/*!****************************************************************************
 * Define
 */
//UART1
#define     UART1_USE                   (1)
#define     UART1_TxBffSz               (16)
#define     UART1_RxBffSz               (16)
#define     UART1_RxDmaInterruptPrior   (15)
#define     UART1_TXIRQPrior            (15)
#define     UART1_PINAFTX               (7)
#define     UART1_PINAFRX               (7)
#define     UART1_HALFDUPLEX            (0)
#define     UART1_RX_IDLE_LINE_MODE     (1)

//UART2
#define     UART2_USE                   (0)
#define     UART2_TxBffSz               (64)
#define     UART2_RxBffSz               (64)
#define     UART2_RxDmaInterruptPrior   (15)
#define     UART2_TXIRQPrior            (15)
#define     UART2_PINAFTX               (7)
#define     UART2_PINAFRX               (7)
#define     UART2_HALFDUPLEX            (0)
#define     UART2_RX_IDLE_LINE_MODE     (1)

//UART3
#define     UART3_USE                   (0)
#define     UART3_TxBffSz               (255)
#define     UART3_RxBffSz               (255)
#define     UART3_RxDmaInterruptPrior   (15)
#define     UART3_TXIRQPrior            (15)
#define     UART3_PINAFTX               (7)
#define     UART3_PINAFRX               (7)
#define     UART3_HALFDUPLEX            (0)
#define     UART3_RX_IDLE_LINE_MODE     (1)

/*!****************************************************************************
 * Enumeration
 */

/******************************************************************************
 * Typedef
 */
typedef enum {
	BR9600,
	BR38400,
	BR57600,
	BR115200,
	BR_NUMBER
} uartBaudRate_type;

typedef enum {
	uartTxFree,
	uartTxRun,
	uartTxSuccess,
	uartTxErr
} uartTxState_type;

typedef enum {
	uartRxFree,
	uartRxRun,
	uartRxSuccess,
	uartRxStop,
	uartRxErr
} uartRxState_type;

typedef struct uartStruct{
	USART_TypeDef 				*pUart;
	uint8_t 					*pTxBff;
	uint8_t 					*pRxBff;
	DMA_Stream_TypeDef 		    *pDmaStreamTx;
	DMA_Stream_TypeDef 		    *pDmaStreamRx;
	void (*txHoock)(struct uartStruct *uart);
	void (*rxHoock)(struct uartStruct *uart);
	volatile uartTxState_type 	txState :8;
	volatile uartRxState_type 	rxState :8;
	uartTxState_type 			baudRate :4;
	uint8_t 					halfDuplex :1;
	uint8_t 					rxIdleLineMode :1;
	volatile uint16_t 			txCnt;
	volatile uint16_t 			rxCnt;
} uart_type;

typedef void (*uartCallback_type)(uart_type *uart);

/*!****************************************************************************
 * Exported variables
 */
#if (UART1_USE == 1)
extern uart_type *uart1;
#endif //UART1_USE

#if (UART2_USE == 1)
extern uart_type *uart2;
#endif //UART2_USE

#if (UART3_USE == 1)
extern uart_type *uart3;
#endif //UART3_USE

/*!****************************************************************************
 * Macro functions
 */
#define uartGetRemainTx(uartx)      (uartx->pDmaStreamTx->NDTR)
#define uartGetRemainRx(uartx)      (uartx->pDmaStreamRx->NDTR)

/*!****************************************************************************
 * Prototypes for the functions
 */
void uart_init(uart_type *uartx, uartBaudRate_type baudRate);
void uart_deinit(uart_type *uartx);
void uart_setBaud(uart_type *uartx, uartBaudRate_type baudRate);
void uart_setCallback(uart_type *uartx, uartCallback_type txHoock, uartCallback_type rxHoock);
void uart_setBaud(uart_type *uartx, uartBaudRate_type baudRate);
void uart_write(uart_type *uartx, void *src, uint16_t len);
void uart_read(uart_type *uartx, void *dst, uint16_t len);
void uart_stopRead(uart_type *uartStruct);

#endif //UART_H
/*************** LGPL ************** END OF FILE *********** D_EL ************/


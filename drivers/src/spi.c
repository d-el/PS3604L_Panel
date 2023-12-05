/*!****************************************************************************
 * @file		spi.c
 * @author		d_el
 * @version		V1.0
 * @date		05.12.2023
 * @copyright	License (MIT). Copyright (c) 2023 Storozhenko Roman
 * @brief
 */

/*!****************************************************************************
 * Include
 */
#include <stm32f4xx.h>
#include "gpio.h"
#include "spi.h"


#define LCD_SPI				SPI3
#define LCD_DMA_CHANNEL		0
#define LCD_DMA_STREAM		DMA1_Stream7
#define LCD_DMAMEM_STREAM	DMA2_Stream5
#define LCD_DMA_IRQPrior	15

/*!****************************************************************************
 * MEMORY
 */
static spicb_type flushcb, setbufcb;

/*!****************************************************************************
 * @brief SPI initialization
 */
void spi_init(void){
	//Max speed - fPCLK/2
	RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;		//Clock enable
	RCC->APB1RSTR |= RCC_APB1RSTR_SPI3RST;	//Reset module
	RCC->APB1RSTR &= ~RCC_APB1RSTR_SPI3RST;

	LCD_SPI->CR1 |= SPI_CR1_MSTR;				//Master configuration
	LCD_SPI->CR1 |= SPI_CR1_SSM;				//Software slave management enabled
	LCD_SPI->CR1 |= SPI_CR1_SSI;				//Internal slave select
	LCD_SPI->CR1 &= ~SPI_CR1_DFF;				//8-bit data frame format is selected for transmission/reception
	LCD_SPI->CR1 &= ~SPI_CR1_LSBFIRST;			//MSB transmitted first

	LCD_SPI->CR1 |= SPI_CR1_SPE;				//SPI enable

	gppin_init(GPIOB, 3, alternateFunctionPushPull, pullDisable, 0, 6);	//SPI1_SCK
	//gppin_init(GPIOB, 4, alternateFunctionPushPull, pullDisable, 0, 6);	//SPI1_MISO
	gppin_init(GPIOB, 5, alternateFunctionPushPull, pullDisable, 0, 6);	//SPI1_MOSI
}

/*!****************************************************************************
 * @brief Send data from SPI
 */
void spi_send(uint8_t data){
	LCD_SPI->DR = data;
	while((LCD_SPI->SR & SPI_SR_TXE) != 0);
	while((LCD_SPI->SR & SPI_SR_BSY) != 0);
}

/*!****************************************************************************
 *
 */
void spi_initBlock(void){
	/************************************************
	 * DMA ME2MEM
	 */
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
	LCD_DMAMEM_STREAM->CR = 0;
	//LCD_DMAMEM_STREAM->CR	|= DMA_SxCR_PL_0;								// Priority level Medium
	LCD_DMAMEM_STREAM->CR |= DMA_SxCR_MSIZE_0;								// Memory data size half-word (16-bit)
	LCD_DMAMEM_STREAM->CR |= DMA_SxCR_PSIZE_0;								// Memory data size half-word (16-bit)
	LCD_DMAMEM_STREAM->CR |= DMA_SxCR_MINC;									// Memory increment mode enabled
	LCD_DMAMEM_STREAM->CR &= ~DMA_SxCR_PINC;								// Peripheral increment mode disabled
	LCD_DMAMEM_STREAM->CR &= ~DMA_SxCR_CIRC;								// Circular mode disable
	LCD_DMAMEM_STREAM->CR |= DMA_SxCR_DIR_1;								// Direction Memory-to-memory
	LCD_DMAMEM_STREAM->CR |= DMA_SxCR_TCIE;									// Transfer complete interrupt enable
	LCD_DMAMEM_STREAM->PAR = 0;												// Peripheral address

	NVIC_EnableIRQ(DMA2_Stream5_IRQn);
	NVIC_SetPriority(DMA2_Stream5_IRQn, LCD_DMA_IRQPrior);

	/************************************************
	 * DMA
	 */
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
	LCD_DMA_STREAM->CR = 0;
	LCD_DMA_STREAM->CR |= (uint32_t)((LCD_DMA_CHANNEL & 0x03) << 25);	// Channel selection
	LCD_DMA_STREAM->CR |= DMA_SxCR_PL_1;								// Priority level High
	LCD_DMA_STREAM->CR |= DMA_SxCR_MSIZE_0;								// Memory data size half-word (16-bit)
	LCD_DMA_STREAM->CR |= DMA_SxCR_PSIZE_0;								// Memory data size half-word (16-bit)
	LCD_DMA_STREAM->CR |= DMA_SxCR_MINC;								// Memory increment mode enabled
	LCD_DMA_STREAM->CR &= ~DMA_SxCR_PINC;								// Peripheral increment mode disabled
	LCD_DMA_STREAM->CR &= ~DMA_SxCR_CIRC;								// Circular mode disable
	LCD_DMA_STREAM->CR |= DMA_SxCR_DIR_0;								// Direction Memory-to-peripheral
	LCD_DMA_STREAM->CR |= DMA_SxCR_TCIE;								// Transfer complete interrupt enable
	LCD_DMA_STREAM->PAR = (uint32_t)&LCD_SPI->DR;						// Peripheral address

	gppin_set(GP_LCD_DC);
	LCD_SPI->CR1 |= SPI_CR1_DFF;	//16-bit data frame format is selected for transmission/reception
	LCD_SPI->CR2 |= SPI_CR2_TXDMAEN;

	NVIC_EnableIRQ(DMA1_Stream7_IRQn);
	NVIC_SetPriority(DMA1_Stream7_IRQn, LCD_DMA_IRQPrior);
}

/*!****************************************************************************
 * @brief Flush video buffer to display
 */
void spi_writeBlock(uint16_t* block, size_t len, spicb_type cb){
	flushcb = cb;
	LCD_DMA_STREAM->M0AR = (uint32_t)block; // Memory address
	LCD_DMA_STREAM->NDTR = len;
	LCD_DMA_STREAM->CR |= DMA_SxCR_EN;
}

/*!****************************************************************************
 * @brief Set fill video buffer
 */
void spi_fillBlock(uint16_t* block, size_t len, const uint16_t* color, spicb_type cb){
	setbufcb = cb;
	LCD_DMAMEM_STREAM->M0AR = (uint32_t)block; // Memory address
	LCD_DMAMEM_STREAM->PAR	= (uint32_t)color;
	LCD_DMAMEM_STREAM->NDTR = len;
	LCD_DMAMEM_STREAM->CR |= DMA_SxCR_EN;
}

/*!****************************************************************************
 * @brief Handler tx DMA
 */
void DMA1_Stream7_IRQHandler(void){
	if(flushcb){
		flushcb();
	}
	LCD_DMA_STREAM->CR &= ~DMA_SxCR_EN;
	DMA1->HIFCR = DMA_HIFCR_CTCIF7; //Clear flag
}

/*!****************************************************************************
 * @brief Handler memory DMA
 */
void DMA2_Stream5_IRQHandler(void){
	if(setbufcb){
		setbufcb();
	}
	LCD_DMAMEM_STREAM->CR &= ~DMA_SxCR_EN;
	DMA2->HIFCR = DMA_HIFCR_CTCIF5; //Clear flag
}

/******************************** END OF FILE ********************************/

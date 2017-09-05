/*!****************************************************************************
 * @file		spfd54124b.c
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		19.12.2015
 * @copyright	GNU Lesser General Public License v3
 * @brief		Driver display on controller spfd54124b
 */

/*!****************************************************************************
 * Include
 */
#include "spfd54124b.h"

/*!****************************************************************************
 * MEMORY
 */
uint16_t __attribute__((section (".ccmrambss"))) videoBff[ (SPFD54124B_W * SPFD54124B_H * 18) / (sizeof(videoBff[0]) * 8) ];	///< 1 pix = 18 bit


/*!****************************************************************************
 * Local prototypes for the functions
 */
void spfd_gotoxy(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void spfd_lcdDat16(uint16_t data);

/*!****************************************************************************
 * @brief Передача данных
 */
void spfd_lcdSpiTx(uint32_t data){
	data = ~data;
	SPI3->DR = data;                    //Загружаем данные для передачи
	while(!(SPI3->SR & SPI_SR_TXE))
		;   //Ожидание окончания передачи
}

/*!****************************************************************************
 * @brief Передача команды
 */
void spfd_lcdCmd(uint8_t data){
	spfd_lcdSpiTx(data);
}

/*!****************************************************************************
 * @brief Передача данных
 */
void spfd_lcdDat(uint8_t data){
	spfd_lcdSpiTx((1 << 8) | (data));
}

/*!****************************************************************************
 * @brief Передача данных 16 bit
 */
void spfd_lcdDat16(uint16_t data){
	spfd_lcdSpiTx((1 << 8) | (data >> 8));
	spfd_lcdSpiTx((1 << 8) | (data & 0x00FF));
}

/*!****************************************************************************
 *
 */
void spfd_initSpi2(void){
	gppin_init(GPIOB, 3, alternateFunctionPushPull, pullDisable, 0, 5);	//SPI1_SCK
	gppin_init(GPIOB, 4, alternateFunctionPushPull, pullDisable, 0, 5);	//SPI1_MISO
	gppin_init(GPIOB, 5, alternateFunctionPushPull, pullDisable, 0, 5);	//SPI1_MOSI

	//Максимальная скорость - fPCLK/2
	RCC->APB2ENR |= RCC_APB1ENR_SPI3EN;		//Clock enable
	RCC->APB2RSTR |= RCC_APB1RSTR_SPI3RST;	//Reset module
	RCC->APB2RSTR &= ~RCC_APB1RSTR_SPI3RST;

	//SPI1->CR1       |= SPI_CR1_BR_1;
	SPI3->CR1 |= SPI_CR1_MSTR;				//Master configuration
	SPI3->CR1 |= SPI_CR1_SSM;				//Software slave management enabled
	SPI3->CR1 |= SPI_CR1_SSI;				//Игнорирование NSS входа
	//SPI3->CR2 = SPI_CR2_DS_3;				//Data size 9bit
	SPI3->CR1 |= SPI_CR1_SPE;				//SPI enable
}

/*!****************************************************************************
 *
 */
void spfd_initSpi2DMA(void){
	/************************************************
	 * DMA
	 */
	uint32_t dmaChannelTx = 0;
	DMA_Stream_TypeDef *pDmaStreamTx = DMA1_Stream7;

	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
    pDmaStreamTx->CR 	= 0;
    pDmaStreamTx->CR 	|= (uint32_t)((dmaChannelTx & 0x03) << 25);     //Channel selection
    pDmaStreamTx->CR 	|= DMA_SxCR_PL_1;                               //Priority level High
    pDmaStreamTx->CR 	&= ~DMA_SxCR_MSIZE;                             //Memory data size 8 bit
    pDmaStreamTx->CR 	&= ~DMA_SxCR_PSIZE;                             //Memory data size 8 bit
    pDmaStreamTx->CR 	|= DMA_SxCR_MINC;                               //Memory increment mode enabled
    pDmaStreamTx->CR 	&= ~DMA_SxCR_PINC;                              //Peripheral increment mode disabled
    pDmaStreamTx->CR 	|= DMA_SxCR_CIRC;                        		//Circular mode enable
    pDmaStreamTx->CR 	|= DMA_SxCR_DIR_0;                              //Direction Memory-to-peripheral
    pDmaStreamTx->NDTR   = sizeof(videoBff);              				//Number of data
    pDmaStreamTx->PAR    = (uint32_t)&SPI3->DR;                			//Peripheral address
    pDmaStreamTx->M0AR   = (uint32_t)&videoBff[0];                      //Memory address
}

/*!****************************************************************************
 *
 */
void spfd_updateVideoStart(void){
	spfd_gotoxy(0, 0, 159, 127);
	SPI3->CR2 |= SPI_CR2_TXDMAEN;
	DMA1_Stream7->CR |= DMA_SxCR_EN;
}

/*!****************************************************************************
 *
 */
void spfd_updateVideoStop(void){
	DMA1_Stream7->CR &= ~DMA_SxCR_EN;
	SPI3->CR2 &= ~SPI_CR2_TXDMAEN;
}

/*!****************************************************************************
 * @brief Инициализация LCD
 */
void spfd_init(void){
//	//gppin_init(GPIOB, 12, outPushPull, pullDisable, 0, 0);               //RST
//	spfd_initSpi2();
//
//	//HI_lcd_rst;
//	for(uint32_t i = 0; i < 10000; i++){
//		__NOP();
//	}
//	//LO_lcd_rst;
//	for(uint32_t i = 0; i < 10000; i++){
//		__NOP();
//	}
//	//HI_lcd_rst;
//	for(uint32_t i = 0; i < 10000; i++){
//		__NOP();
//	}
//
//	spfd_lcdCmd(0xBA);
//	spfd_lcdDat(0x07);
//	spfd_lcdDat(0x15); //Порядок отсылки данных
//
//	spfd_lcdCmd(0x25);
//	spfd_lcdDat(0x3F);	//Контраст
//	spfd_lcdCmd(0x11);	//Выход из режима sleep (sleepout)
//	//lcd_cmd(0x14);
//
//	spfd_lcdCmd(0x36);	//Memory Data Access Control
//	//lcd_dat(0xA0);	//Альбомная ориентация
//	spfd_lcdDat(0x60);	//Альбомная ориентация, повернутая на 180°
//
//	spfd_lcdCmd(0x37);	//VSCROLL ADDR
//	spfd_lcdDat(0x00);
//
//	spfd_lcdCmd(0x3a);	//COLMOD pixel format
//	spfd_lcdDat(0x05);	//COLMOD pixel format 4=12,5=16,6=18
//
//	spfd_lcdCmd(0x29);	//DISPON
//	spfd_lcdCmd(0x20);	//INVOFF
//	spfd_lcdCmd(0x13);	//NORON

	//memset(videoBff, 0xFF, sizeof(videoBff));

	for(uint32_t i = 0; i < sizeof(videoBff) / sizeof(videoBff[0]); i++){
		videoBff[i] = 0;
	}

	//spfd_initSpi2DMA();
	//spfd_updateVideoStart();
}

/*!****************************************************************************
 * @brief Программное выключение LCD
 */
void spfd_disable(void){
	spfd_updateVideoStop();
	spfd_lcdCmd(0x28);	//DISPOFF
	spfd_lcdCmd(0x11);	//Вход в режим sleep (sleepin)
}

/*!****************************************************************************
 * @brief Установка контрастности дисплея
 */
void spfd_contrast(uint8_t data){
	/*lcd_cmd(0x25);          //Write contrast
	 if((data > 85)&&(data < 115)){
	 lcd_data(data);
	 }
	 else{
	 lcd_data(100);
	 }*/
	/*lcd_cmd(0x28);	//DISPOFF
	 lcd_cmd(0x11);	//Вход в режим sleep (sleepin)

	 lcd_cmd(0x25);
	 lcd_dat(contrast); // Контраст

	 lcd_cmd(0x11);	//Выход из режима sleep (sleepout)
	 lcd_cmd(0x29);	//DISPON*/
}

/*!****************************************************************************
 * @brief Установка окна заливки
 */
void spfd_gotoxy(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1){
	//Выбираем диапазон столбцов
	spfd_lcdCmd(0x2A);        				//CASETP command –
	spfd_lcdDat(0);           				//Второй байт всегда нулевой, т.к. передача по 2 байта
	spfd_lcdDat(displayOffsetX + x0);		//Левый угол - x
	spfd_lcdDat(0);
	spfd_lcdDat(displayOffsetX + x1);     	//Правый угол – x
	// Диапазон строк
	spfd_lcdCmd(0x2B);        				//PASETP command
	spfd_lcdDat(0);
	spfd_lcdDat(displayOffsetY + y0);		//Левый угол - y
	spfd_lcdDat(0);
	spfd_lcdDat(displayOffsetY + y1);		//Правый угол - y
	spfd_lcdCmd(0x2C);        				//RAMWR command
}

/*************** LGPL ************** END OF FILE *********** D_EL ************/

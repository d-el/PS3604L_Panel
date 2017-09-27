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
//uint16_t __attribute__((section (".ccmrambss"))) videoBff[ (SPFD54124B_W * SPFD54124B_H * 18) / (sizeof(videoBff[0]) * 8) ];	///< 1 pix = 18 bit
uint16_t videoBff[ (SPFD54124B_W * SPFD54124B_H * 18) / (sizeof(videoBff[0]) * 8) ];


/*!****************************************************************************
 * Local prototypes for the functions
 */
void spfd_gotoxy(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

/*!****************************************************************************
 *
 */
void spfd_spiInit(void){
	gppin_init(GPIOB, 3, alternateFunctionPushPull, pullDisable, 0, 6);	//SPI1_SCK
	gppin_init(GPIOB, 4, alternateFunctionPushPull, pullDisable, 0, 6);	//SPI1_MISO
	gppin_init(GPIOB, 5, alternateFunctionPushPull, pullDisable, 0, 6);	//SPI1_MOSI

	//Максимальная скорость - fPCLK/2
	RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;		//Clock enable
	RCC->APB1RSTR |= RCC_APB1RSTR_SPI3RST;	//Reset module
	RCC->APB1RSTR &= ~RCC_APB1RSTR_SPI3RST;

	//SPI3->CR1 |= SPI_CR1_BR_2 | SPI_CR1_BR_1;
	SPI3->CR1 |= SPI_CR1_MSTR;				//Master configuration
	SPI3->CR1 |= SPI_CR1_SSM;				//Software slave management enabled
	SPI3->CR1 |= SPI_CR1_SSI;				//Игнорирование NSS входа

	SPI3->CR1 |= SPI_CR1_DFF;		//16-bit data frame format is selected for transmission/reception
	SPI3->CR1 |= SPI_CR1_LSBFIRST;	//LSB transmitted first

	SPI3->CR1 |= SPI_CR1_SPE;				//SPI enable
}

/*!****************************************************************************
 *
 */
void spfd_softSpiInit(void){
	gppin_init(GPIOB, 3, outPushPull, pullDisable, 0, 0);	//SPI1_SCK
	gppin_init(GPIOB, 5, outPushPull, pullDisable, 0, 0);	//SPI1_MOSI
}

/*!****************************************************************************
 *
 */
void sw_SPIsend(uint16_t data){
	uint32_t mask;
	for(uint32_t mask = 1 << 8; mask != 0; mask >>= 1){
		(data & mask) ? _gppin_set(GPIOB, pinm5) : _gppin_reset(GPIOB, pinm5);
		asm volatile ("nop");
		_gppin_set(GPIOB, pinm3);
		asm volatile ("nop");
		_gppin_reset(GPIOB, pinm3);
		asm volatile ("nop");
	}
}

/*!****************************************************************************
 *
 */
void spfd_lcdCmd(uint8_t data){
    uint16_t buf = data;
    gppin_reset(GP_SSD_CS);;    //формирование уровня cs - LOW
    buf |= (0<<8);
    sw_SPIsend(buf);
    gppin_set(GP_SSD_CS);;    	//формирование уровня cs - HI
}

/*!****************************************************************************
 *
 */
void spfd_lcdDat(uint8_t data){
    uint16_t buf = data;
    gppin_reset(GP_SSD_CS);;    //формирование уровня cs - LOW
    buf |= (1<<8);
    sw_SPIsend(buf);
    gppin_set(GP_SSD_CS);;    	//формирование уровня cs - HI
}

/*!****************************************************************************
 *
 */
void spfd_initSpiDMA(void){
	/************************************************
	 * DMA
	 */
	uint32_t dmaChannelTx = 0;
	DMA_Stream_TypeDef *pDmaStreamTx = DMA1_Stream7;

	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
    pDmaStreamTx->CR 	= 0;
    pDmaStreamTx->CR 	|= (uint32_t)((dmaChannelTx & 0x03) << 25);     //Channel selection
    pDmaStreamTx->CR 	|= DMA_SxCR_PL_1;                               //Priority level High
    pDmaStreamTx->CR 	|= DMA_SxCR_MSIZE_0;                             //Memory data size half-word (16-bit)
    pDmaStreamTx->CR 	|= DMA_SxCR_PSIZE_0;                             //Memory data size half-word (16-bit)
    pDmaStreamTx->CR 	|= DMA_SxCR_MINC;                               //Memory increment mode enabled
    pDmaStreamTx->CR 	&= ~DMA_SxCR_PINC;                              //Peripheral increment mode disabled
    pDmaStreamTx->CR 	|= DMA_SxCR_CIRC;                        		//Circular mode enable
    pDmaStreamTx->CR 	|= DMA_SxCR_DIR_0;                              //Direction Memory-to-peripheral
    pDmaStreamTx->NDTR   = sizeof(videoBff)/2;              				//Number of data
    pDmaStreamTx->PAR    = (uint32_t)&SPI3->DR;                			//Peripheral address
    pDmaStreamTx->M0AR   = (uint32_t)&videoBff[0];                      //Memory address
}

/*!****************************************************************************
 *
 */
void spfd_updateVideoStart(void){
	//spfd_gotoxy(0, 0, 159, 127);
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
	spfd_softSpiInit();

	gppin_set(GP_SSD_RES);
	for(uint32_t i = 0; i < 100000; i++)
		__NOP();
		;
	gppin_reset(GP_SSD_RES);
	for(uint32_t i = 0; i < 100000; i++)
		__NOP();
		;
	gppin_set(GP_SSD_RES);
	for(uint32_t i = 0; i < 100000; i++)
		__NOP();
		;

	spfd_lcdCmd(0xBA);
	spfd_lcdDat(0x07);
	spfd_lcdDat(0x15); //Порядок отсылки данных

	spfd_lcdCmd(0x25);
	spfd_lcdDat(0x3F);	//Контраст
	spfd_lcdCmd(0x11);	//Выход из режима sleep (sleepout)
	//lcd_cmd(0x14);

	spfd_lcdCmd(0x36);	//Memory Data Access Control
	//lcd_dat(0xA0);	//Альбомная ориентация
	spfd_lcdDat(0x60);	//Альбомная ориентация, повернутая на 180°

	spfd_lcdCmd(0x37);	//VSCROLL ADDR
	spfd_lcdDat(0x00);

	spfd_lcdCmd(0x3a);	//COLMOD pixel format
	spfd_lcdDat(0x05);	//COLMOD pixel format 4=12,5=16,6=18

	spfd_lcdCmd(0x29);	//DISPON
	spfd_lcdCmd(0x20);	//INVOFF
	spfd_lcdCmd(0x13);	//NORON

	spfd_gotoxy(0, 0, 159, 127);
	memset(videoBff, 0xFF, sizeof(videoBff));
	for(uint32_t i = 0; i < SPFD54124B_W * SPFD54124B_H; i++){
		spfd_setPixel(i % SPFD54124B_W, i / SPFD54124B_W, black);
	}

	//Hard SPI
	spfd_spiInit();
	spfd_initSpiDMA();
	spfd_updateVideoStart();
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

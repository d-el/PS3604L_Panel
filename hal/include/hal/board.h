/*!****************************************************************************
 * @file		base.h
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		01.01.2015
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		Global settings
 */

/**
 * TIM15_CH2 (PB15)		-> PWM Beep
 * TIM6					-> Time for Beep
 * TIM8_CH2 (PC7)		-> LED PWM

 * *******************
 *
 * DMA1_Stream7		-> SPI3_TX
 * DMA1_Stream0		-> SPI3_RX
 *
 * DMA1_Stream3		-> USART3_TX
 * DMA1_Stream1		-> USART3_RX
 *
 * DMA1_Stream4		-> UART4_TX
 * DMA1_Stream2		-> UART4_RX
 *
 * DMA1_Stream6		-> I2C1_TX
 * DMA1_Stream5		-> I2C1_RX
 *
 * DMA2_Stream7		-> USART1_TX
 * DMA2_Stream2		-> USART1_RX
 *
 * DMA2_Stream0		-> MEM2MEM in st7735 driver
 *
 */
#ifndef board_H
#define board_H

/*!****************************************************************************
 * Include
 */
#include "gpio.h"

/*!****************************************************************************
 * User define
 */
#define SYSTEM_FREQ			168000000	//[Hz]
#define APB1_FREQ			42000000	//[Hz]
#define APB1_TIM_FREQ		84000000	//[Hz]
#define APB2_FREQ			42000000	//[Hz]
#define APB2_TIM_FREQ		84000000	//[Hz]

/*!****************************************************************************
 * Macro functions
 */
#define LED_ON()			gppin_set(GP_LED_GP)
#define LED_OFF()			gppin_reset(GP_LED_GP)
#define LED_TOGGLE()		gppin_togle(GP_LED_GP)
#define PHY_RESET()			gppin_reset(GP_LED_GP)

/*!****************************************************************************
 * Prototypes for the functions
 */

#endif //board_H
/******************************** END OF FILE ********************************/

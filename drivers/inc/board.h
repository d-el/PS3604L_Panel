/*!****************************************************************************
 * @file		base.h
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		01.01.2015
 * @copyright	GNU Lesser General Public License v3
 * @brief		Global settings
 */

/**
 * TIM15_CH2 (PB15)		-> PWM Beep
 * TIM6					-> Time for Beep

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

/* MAC ADDRESS*/
extern uint8_t macAddress[6];
#define MAC_ADDR0		macAddress[0]
#define MAC_ADDR1		macAddress[1]
#define MAC_ADDR2		macAddress[2]
#define MAC_ADDR3		macAddress[3]
#define MAC_ADDR4		macAddress[4]
#define MAC_ADDR5		macAddress[5]

/*!****************************************************************************
 * Macro functions
 */
#define LED_ON()			gppin_set(GP_LED_GP)
#define LED_OFF()			gppin_reset(GP_LED_GP)
#define LED_TOGGLE()		gppin_togle(GP_LED_GP)

/*!****************************************************************************
 * Prototypes for the functions
 */

#endif //board_H
/*************** LGPL ************** END OF FILE *********** D_EL ************/

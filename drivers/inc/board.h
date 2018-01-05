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
#define SYSTEM_FREQ         168000000   //[Hz]
#define APB1_FREQ           42000000    //[Hz]
#define APB1_TIM_FREQ       84000000    //[Hz]
#define APB2_FREQ           42000000    //[Hz]
#define APB2_TIM_FREQ       84000000    //[Hz]

/* MAC ADDRESS*/
extern uint8_t macAddress[6];
#define MAC_ADDR0   	macAddress[0]
#define MAC_ADDR1   	macAddress[1]
#define MAC_ADDR2   	macAddress[2]
#define MAC_ADDR3   	macAddress[3]
#define MAC_ADDR4   	macAddress[4]
#define MAC_ADDR5   	macAddress[5]

///*Static IP ADDRESS*/
//#define IP_ADDR0   		192
//#define IP_ADDR1   		168
//#define IP_ADDR2   		1
//#define IP_ADDR3   		10
//
///*NETMASK*/
//#define NETMASK_ADDR0   255
//#define NETMASK_ADDR1   255
//#define NETMASK_ADDR2   255
//#define NETMASK_ADDR3   0
//
///*Gateway Address*/
//#define GW_ADDR0   		192
//#define GW_ADDR1   		168
//#define GW_ADDR2   		1
//#define GW_ADDR3   		1

/*!****************************************************************************
 * Macro functions
 */
#define LED_ON()            /*gppin_set(GP_LED0)*/
#define LED_OFF()           /*gppin_reset(GP_LED0)*/
#define LED_TOGGLE()        /*gppin_togle(GP_LED0)*/

/*!****************************************************************************
 * Prototypes for the functions
 */

#endif //board_H
/*************** LGPL ************** END OF FILE *********** D_EL ************/

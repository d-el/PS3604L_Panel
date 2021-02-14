/**
  ******************************************************************************
  * @file		stm32f4x7_eth_conf.h
  * @author		MCD Application Team
  * @version	V1.0.0
  * @date		31-October-2011
  * @brief		Configuration file for the STM32F4x7 Ethernet driver.
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4x7_ETH_CONF_H
#define __STM32F4x7_ETH_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "assert.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define assert_param assert

/* Uncomment the line below when using time stamping and/or IPv4 checksum offload */
#define USE_ENHANCED_DMA_DESCRIPTORS

/* Uncomment the line below if you want to use user defined Delay function
   (for precise timing), otherwise default _eth_delay_ function defined within
   the Ethernet driver is used (less precise timing) */
//#define USE_Delay

#ifdef USE_Delay
  #include "main.h"				   /* Header file where the Delay function prototype is exported */
  #define _eth_delay_	 Delay	   /* User can provide more timing precise _eth_delay_ function */
#else
  #define _eth_delay_	 ETH_Delay /* Default _eth_delay_ function with less precise timing */
#endif


/* Uncomment the line below to allow custom configuration of the Ethernet driver buffers */
//#define CUSTOM_DRIVER_BUFFERS_CONFIG

#ifdef	CUSTOM_DRIVER_BUFFERS_CONFIG
/* Redefinition of the Ethernet driver buffers size and count */
 #define ETH_RX_BUF_SIZE	ETH_MAX_PACKET_SIZE /* buffer size for receive */
 #define ETH_TX_BUF_SIZE	ETH_MAX_PACKET_SIZE /* buffer size for transmit */
 #define ETH_RXBUFNB		20					/* 20 Rx buffers of size ETH_RX_BUF_SIZE */
 #define ETH_TXBUFNB		5					/* 5  Tx buffers of size ETH_TX_BUF_SIZE */
#endif


/* PHY configuration section **************************************************/
/* PHY Reset delay */
#define PHY_RESET_DELAY			((uint32_t)0x00000FFF)
/* PHY Configuration delay */
#define PHY_CONFIG_DELAY		((uint32_t)0x00000FFF)

/* INTERRUPT SOURCE FLAG REGISTER */
#define PHY_ISFR				((uint16_t)29)

/* PHY Special Control/Status Register */
#define PHY_IMR					((uint16_t)30)
#define PHY_ISFR_LINK_DOWN_MASK	(0x0010)

/* The PHY status register value change from a PHY to another, so the user have
   to update this value depending on the used external PHY */
#define PHY_SR					((uint16_t)31) /* Value for LAN8720 PHY */

/* The Speed and Duplex mask values change from a PHY to another, so the user
   have to update this value depending on the used external PHY */
#define PHY_DUPLEX_SPEED_STATUS_MASK	((uint16_t)0x001C)
#define PHY_10M_HALF					(0x04)
#define PHY_10M_FULL					(0x14)
#define PHY_100BTX_HALF					(0x08)
#define PHY_100BTX_FULL					(0x18)

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif /* __STM32F4x7_ETH_CONF_H */


/*********** Portions COPYRIGHT 2012 Embest Tech. Co., Ltd.*****END OF FILE****/


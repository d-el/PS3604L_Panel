/*!****************************************************************************
  * @file		stm32f4x7_eth_bsp.h
  * @author		MCD Application Team
  * @version	V1.0.0
  * @date		31-October-2011
  * @brief		Header for stm32f4x7_eth_bsp.c file.
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4x7_ETH_BSP_H
#define __STM32F4x7_ETH_BSP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Exported types ------------------------------------------------------------*/
typedef void (*ETH_IRQHandler_type)(void *arg);

/* Exported constants --------------------------------------------------------*/
#define LAN8720_ADDRESS0	0x00
#define LAN8720_ADDRESS1	0x01
#define PHY_ADDRESS			LAN8720_ADDRESS0

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void  ETH_BSP_Config(void);
void  ETH_BSP_Deinit(void);
void  ETH_BSP_setHandler(ETH_IRQHandler_type h);
void  ETH_BSP_getMAC(uint8_t mac[6]);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F4x7_ETH_BSP_H */


/*********** Portions COPYRIGHT 2012 Embest Tech. Co., Ltd.*****END OF FILE****/

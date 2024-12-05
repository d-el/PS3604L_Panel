/*!****************************************************************************
 * @file		stm32f4x7_eth_bsp.c
 * @author		MCD Application Team
 * @version		V1.0.0
 * @date		31-October-2011
 * @brief		STM32F4x7 Ethernet hardware configuration.
 */
/* Includes ------------------------------------------------------------------*/
#include <stddef.h>
#include "stm32f4x7_eth.h"
#include "stm32f4x7_eth_bsp.h"
#include "gpio.h"
#include "uniqueDeviceId.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t EthInitStatus = 0;
ETH_IRQHandler_type eth_IRQHandler;

/* Private function prototypes -----------------------------------------------*/
static void ETH_GPIO_Config(void);
static void ETH_MACDMA_Config(void);
static void ETH_NVIC_Config(void);

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  ETH_BSP_Config
 * @param  None
 * @retval None
 */
void ETH_BSP_Config(void){
	/* Configure the GPIO ports for ethernet pins */
	ETH_GPIO_Config();

	/* Config NVIC for Ethernet */
	ETH_NVIC_Config();

	/* Configure the Ethernet MAC/DMA */
	ETH_MACDMA_Config();
}

void  ETH_BSP_Deinit(void){
	RCC->AHB1ENR &= ~RCC_AHB1ENR_ETHMACEN;
	RCC->AHB1ENR &= ~RCC_AHB1ENR_ETHMACTXEN;
	RCC->AHB1ENR &= ~RCC_AHB1ENR_ETHMACRXEN;
	_gppin_reset(GPIOC, pinm2);
}

/**
 * @brief  ETH_BSP_setHandler
 * @param  pointer to handler
 * @retval None
 */
void  ETH_BSP_setHandler(ETH_IRQHandler_type h){
	eth_IRQHandler = h;
}

/**
 * @brief  ETH_BSP_getMAC
 * @param  pointer to mac destination
 * @retval None
 */
void  ETH_BSP_getMAC(uint8_t mac[6]){
	mac[0] = 0x28;
	mac[1] = 0x05;
	uint32_t uid = uid_get();
	mac[2] = ( uid >> 0)  & 0xFF;
	mac[3] = ( uid >> 8)  & 0xFF;
	mac[4] = ( uid >> 16) & 0xFF;
	mac[5] = ( uid >> 24) & 0xFF;
}

/**
 * @brief  Configures the Ethernet Interface
 * @param  None
 * @retval None
 */
static void ETH_MACDMA_Config(void){
	ETH_InitTypeDef ETH_InitStructure;

	/* Enable ETHERNET clock  */
	RCC->AHB1ENR |= RCC_AHB1ENR_ETHMACEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_ETHMACTXEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_ETHMACRXEN;

	/* Reset ETHERNET on AHB Bus */
	RCC->AHB1RSTR |= RCC_AHB1RSTR_ETHMACRST;
	RCC->AHB1RSTR &= ~RCC_AHB1RSTR_ETHMACRST;

	/* Software reset */
	ETH_SoftwareReset();

	/* Wait for software reset */
	while(ETH_GetSoftwareResetStatus() == SET)
		;

	/* ETHERNET Configuration --------------------------------------------------*/
	/* Call ETH_StructInit if you don't like to configure all ETH_InitStructure parameter */
	ETH_StructInit(&ETH_InitStructure);

	/* Fill ETH_InitStructure parametrs */
	/*------------------------	 MAC   -----------------------------------*/
	ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;
	//ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Disable;
	//	ETH_InitStructure.ETH_Speed = ETH_Speed_10M;
	//	ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;

	ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;
	ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;
	ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
	ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;
	ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
	ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
	ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
	ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
#ifdef CHECKSUM_BY_HARDWARE
	ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable;
#endif

	/*------------------------	 DMA   -----------------------------------*/

	/* When we use the Checksum offload feature, we need to enable the Store and Forward mode:
	 the store and forward guarantee that a whole frame is stored in the FIFO, so the MAC can insert/verify the checksum,
	 if the checksum is OK the DMA can handle the frame otherwise the frame is dropped */
	ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable;
	ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;
	ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;

	ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;
	ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;
	ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;
	ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;
	ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;
	ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;
	ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;
	ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;

	/* Configure Ethernet */
	EthInitStatus = ETH_Init(&ETH_InitStructure, PHY_ADDRESS);

	/* Enable the Ethernet Rx Interrupt */
	ETH_DMAITConfig(ETH_DMA_IT_NIS | ETH_DMA_IT_R, ENABLE);
}

/**
 * @brief  Configures the different GPIO ports.
 * @param  None
 * @retval None
 */
void ETH_GPIO_Config(void){
	volatile uint32_t i;

	/* Enable SYSCFG clock */
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	/* MII/RMII Media interface selection --------------------------------------*/
	SYSCFG->PMC |= SYSCFG_PMC_MII_RMII_SEL;	//RMII PHY interface is selected

	/* Ethernet pins configuration ************************************************/
	gppin_init(GPIOA, 2, alternateFunctionPushPull, pullDisable, 0, 11);	//ETH_MDIO --------------> PA2
	gppin_init(GPIOC, 1, alternateFunctionPushPull, pullDisable, 0, 11);	//ETH_MDC ---------------> PC1
	gppin_init(GPIOA, 1, alternateFunctionPushPull, pullDisable, 0, 11);	//ETH_RMII_REF_CLK-------> PA1
	gppin_init(GPIOA, 7, alternateFunctionPushPull, pullDisable, 0, 11);	//ETH_RMII_CRS_DV -------> PA7
	gppin_init(GPIOB, 10, alternateFunctionPushPull, pullDisable, 0, 11);	//ETH_MII_RX_ER	  -------> PB10
	gppin_init(GPIOC, 4, alternateFunctionPushPull, pullDisable, 0, 11);	//ETH_RMII_RXD0	  -------> PC4
	gppin_init(GPIOC, 5, alternateFunctionPushPull, pullDisable, 0, 11);	//ETH_RMII_RXD1	  -------> PC5
	gppin_init(GPIOB, 11, alternateFunctionPushPull, pullDisable, 0, 11);	//ETH_RMII_TX_EN  -------> PB11
	gppin_init(GPIOB, 12, alternateFunctionPushPull, pullDisable, 0, 11);	//ETH_RMII_TXD0	  -------> PB12
	gppin_init(GPIOB, 13, alternateFunctionPushPull, pullDisable, 0, 11);	//ETH_RMII_TXD1	  -------> PB13
	gppin_init(GPIOC, 2, outPushPull, pullDisable, 0, 0);					//ETH_RST_PIN	  -------> PC2

	for(i = 0; i < 2000; i++);
	_gppin_set(GPIOC, pinm2);
}

/**
 * @brief  Configures and enable the Ethernet global interrupt.
 * @param  None
 * @retval None
 */
void ETH_NVIC_Config(void){
	/* Enable the Ethernet global Interrupt */
	#define ETH_InterruptPrior	15
	NVIC_EnableIRQ(ETH_IRQn);
	NVIC_SetPriority(ETH_IRQn, ETH_InterruptPrior);
}

/**
 * @brief  This function handles ethernet DMA interrupt request.
 * @param  None
 * @retval None
 */
void ETH_IRQHandler(void){
	/* Frame received */
	if(ETH_GetDMAFlagStatus(ETH_DMA_FLAG_R) == SET){
		if(eth_IRQHandler != 0){
			eth_IRQHandler(NULL);
		}
	}

	/* Clear the interrupt flags. */
	/* Clear the Eth DMA Rx IT pending bits */
	ETH_DMAClearITPendingBit(ETH_DMA_IT_R);
	ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);
}

/*********** Portions COPYRIGHT 2012 Embest Tech. Co., Ltd.*****END OF FILE****/

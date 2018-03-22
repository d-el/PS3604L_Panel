/*!****************************************************************************
* @file			startup_stm32f407xx.c
* @author		d_el - Storozhenko Roman
* @version		V1.0
* @date			07.07.2017
* @brief		Startup STM32F407xx
* @copyright	GNU Lesser General Public License v3
*/

/*!****************************************************************************
* Include
*/
#include "stdint.h"

extern void __libc_init_array(void);	///initialization c library array
extern void SystemInit(void);			///System initialization function
extern int main(void);					///Entry point for the application

extern uint32_t _estack;				///Highest address of the user mode stack
extern uint32_t _sdata;  				///RAM data start
extern uint32_t _edata;  				///RAM data end
extern uint32_t _sidata; 				///ROM data start
extern uint32_t _siccmdata;				///ROM ccmdata start
extern uint32_t _sccmdata;				///CCMRAM ccmdata start
extern uint32_t _eccmdata;				///CCMRAM ccmdata end
extern uint32_t _sbss;   				///RAM bss start
extern uint32_t _ebss;  				///RAM bss end
extern uint32_t _sccmbss;   			///CCMRAM bss start
extern uint32_t _eccmbss;  				///CCMRAM bss end

typedef void(*intVector_type)(void);	///Interrupt service routine typezz

/*!****************************************************************************
 * @brief  Initialization .bss section
 */
void __initializeBss(uint32_t *bssStart, uint32_t *bssEnd){
	uint32_t* pData = bssStart;

	///Verify align by 4
	if(((uint32_t) bssStart % 4) || ((uint32_t) bssEnd % 4)){
		while(1)
			;
	}

	while(pData < bssEnd){
		*pData++ = 0;
	}
}

/*!****************************************************************************
 * @brief  Initialization .data section
 */
void __initializeData(uint32_t *dataStart, uint32_t *dataEnd, uint32_t *src){
	uint32_t *pData = dataStart;

	///Verify align by 4
	if(((uint32_t) dataStart % 4) || ((uint32_t) dataEnd % 4)||((uint32_t) src % 4)){
		while(1)
			;
	}

	while(pData < dataEnd){
		*pData++ = *src++;
	}
}

/*!****************************************************************************
 * @brief	Program entry point
 */
void Reset_Handler(void){
	__initializeData(&_sdata, &_edata, &_sidata);	//.data
	__initializeData(&_sccmdata, &_eccmdata, &_siccmdata);	//.ccmdata
	__initializeBss(&_sbss, &_ebss);	//.bss
	__initializeBss(&_sccmbss, &_eccmbss);	//.ccmbss

	__libc_init_array();

	SystemInit();
	main();
}

///Cortex-M4 Processor Exceptions
__attribute__((weak)) void NMI_Handler					(void){while(1);}
__attribute__((weak)) void HardFault_Handler			(void){while(1);}
__attribute__((weak)) void MemManage_Handler			(void){while(1);}
__attribute__((weak)) void BusFault_Handler				(void){while(1);}
__attribute__((weak)) void UsageFault_Handler			(void){while(1);}
__attribute__((weak)) void SVC_Handler					(void){while(1);}
__attribute__((weak)) void DebugMon_Handler				(void){while(1);}
__attribute__((weak)) void PendSV_Handler				(void){while(1);}
__attribute__((weak)) void SysTick_Handler				(void){while(1);}
///STM32 specific Interrupt
__attribute__((weak)) void WWDG_IRQHandler              (void){while(1);}       /* Window WatchDog              */
__attribute__((weak)) void PVD_IRQHandler               (void){while(1);}       /* PVD through EXTI Line detection */
__attribute__((weak)) void TAMP_STAMP_IRQHandler        (void){while(1);}       /* Tamper and TimeStamps through the EXTI line */
__attribute__((weak)) void RTC_WKUP_IRQHandler          (void){while(1);}       /* RTC Wakeup through the EXTI line */
__attribute__((weak)) void FLASH_IRQHandler             (void){while(1);}       /* FLASH                        */
__attribute__((weak)) void RCC_IRQHandler               (void){while(1);}       /* RCC                          */
__attribute__((weak)) void EXTI0_IRQHandler             (void){while(1);}       /* EXTI Line0                   */
__attribute__((weak)) void EXTI1_IRQHandler             (void){while(1);}       /* EXTI Line1                   */
__attribute__((weak)) void EXTI2_IRQHandler             (void){while(1);}       /* EXTI Line2                   */
__attribute__((weak)) void EXTI3_IRQHandler             (void){while(1);}       /* EXTI Line3                   */
__attribute__((weak)) void EXTI4_IRQHandler             (void){while(1);}       /* EXTI Line4                   */
__attribute__((weak)) void DMA1_Stream0_IRQHandler      (void){while(1);}       /* DMA1 Stream 0                */
__attribute__((weak)) void DMA1_Stream1_IRQHandler      (void){while(1);}       /* DMA1 Stream 1                */
__attribute__((weak)) void DMA1_Stream2_IRQHandler      (void){while(1);}       /* DMA1 Stream 2                */
__attribute__((weak)) void DMA1_Stream3_IRQHandler      (void){while(1);}       /* DMA1 Stream 3                */
__attribute__((weak)) void DMA1_Stream4_IRQHandler      (void){while(1);}       /* DMA1 Stream 4                */
__attribute__((weak)) void DMA1_Stream5_IRQHandler      (void){while(1);}       /* DMA1 Stream 5                */
__attribute__((weak)) void DMA1_Stream6_IRQHandler      (void){while(1);}       /* DMA1 Stream 6                */
__attribute__((weak)) void ADC_IRQHandler               (void){while(1);}       /* ADC1, ADC2 and ADC3s         */
__attribute__((weak)) void CAN1_TX_IRQHandler           (void){while(1);}       /* CAN1 TX                      */
__attribute__((weak)) void CAN1_RX0_IRQHandler          (void){while(1);}       /* CAN1 RX0                     */
__attribute__((weak)) void CAN1_RX1_IRQHandler          (void){while(1);}       /* CAN1 RX1                     */
__attribute__((weak)) void CAN1_SCE_IRQHandler          (void){while(1);}       /* CAN1 SCE                     */
__attribute__((weak)) void EXTI9_5_IRQHandler           (void){while(1);}       /* External Line[9:5]s          */
__attribute__((weak)) void TIM1_BRK_TIM9_IRQHandler     (void){while(1);}       /* TIM1 Break and TIM9          */
__attribute__((weak)) void TIM1_UP_TIM10_IRQHandler     (void){while(1);}       /* TIM1 Update and TIM10        */
__attribute__((weak)) void TIM1_TRG_COM_TIM11_IRQHandler(void){while(1);}       /* TIM1 Trigger and Commutation and TIM11 */
__attribute__((weak)) void TIM1_CC_IRQHandler           (void){while(1);}       /* TIM1 Capture Compare         */
__attribute__((weak)) void TIM2_IRQHandler              (void){while(1);}       /* TIM2                         */
__attribute__((weak)) void TIM3_IRQHandler              (void){while(1);}       /* TIM3                         */
__attribute__((weak)) void TIM4_IRQHandler              (void){while(1);}       /* TIM4                         */
__attribute__((weak)) void I2C1_EV_IRQHandler           (void){while(1);}       /* I2C1 Event                   */
__attribute__((weak)) void I2C1_ER_IRQHandler           (void){while(1);}       /* I2C1 Error                   */
__attribute__((weak)) void I2C2_EV_IRQHandler           (void){while(1);}       /* I2C2 Event                   */
__attribute__((weak)) void I2C2_ER_IRQHandler           (void){while(1);}       /* I2C2 Error                   */
__attribute__((weak)) void SPI1_IRQHandler              (void){while(1);}       /* SPI1                         */
__attribute__((weak)) void SPI2_IRQHandler              (void){while(1);}       /* SPI2                         */
__attribute__((weak)) void USART1_IRQHandler            (void){while(1);}       /* USART1                       */
__attribute__((weak)) void USART2_IRQHandler            (void){while(1);}       /* USART2                       */
__attribute__((weak)) void USART3_IRQHandler            (void){while(1);}       /* USART3                       */
__attribute__((weak)) void EXTI15_10_IRQHandler         (void){while(1);}       /* External Line[15:10]s        */
__attribute__((weak)) void RTC_Alarm_IRQHandler         (void){while(1);}       /* RTC Alarm (A and B) through EXTI Line */
__attribute__((weak)) void OTG_FS_WKUP_IRQHandler       (void){while(1);}       /* USB OTG FS Wakeup through EXTI line */
__attribute__((weak)) void TIM8_BRK_TIM12_IRQHandler    (void){while(1);}       /* TIM8 Break and TIM12         */
__attribute__((weak)) void TIM8_UP_TIM13_IRQHandler     (void){while(1);}       /* TIM8 Update and TIM13        */
__attribute__((weak)) void TIM8_TRG_COM_TIM14_IRQHandler(void){while(1);}       /* TIM8 Trigger and Commutation and TIM14 */
__attribute__((weak)) void TIM8_CC_IRQHandler           (void){while(1);}       /* TIM8 Capture Compare         */
__attribute__((weak)) void DMA1_Stream7_IRQHandler      (void){while(1);}       /* DMA1 Stream7                 */
__attribute__((weak)) void FSMC_IRQHandler              (void){while(1);}       /* FSMC                         */
__attribute__((weak)) void SDIO_IRQHandler              (void){while(1);}       /* SDIO                         */
__attribute__((weak)) void TIM5_IRQHandler              (void){while(1);}       /* TIM5                         */
__attribute__((weak)) void SPI3_IRQHandler              (void){while(1);}       /* SPI3                         */
__attribute__((weak)) void UART4_IRQHandler             (void){while(1);}       /* UART4                        */
__attribute__((weak)) void UART5_IRQHandler             (void){while(1);}       /* UART5                        */
__attribute__((weak)) void TIM6_DAC_IRQHandler          (void){while(1);}       /* TIM6 and DAC1&2 underrun errors */
__attribute__((weak)) void TIM7_IRQHandler              (void){while(1);}       /* TIM7                         */
__attribute__((weak)) void DMA2_Stream0_IRQHandler      (void){while(1);}       /* DMA2 Stream 0                */
__attribute__((weak)) void DMA2_Stream1_IRQHandler      (void){while(1);}       /* DMA2 Stream 1                */
__attribute__((weak)) void DMA2_Stream2_IRQHandler      (void){while(1);}       /* DMA2 Stream 2                */
__attribute__((weak)) void DMA2_Stream3_IRQHandler      (void){while(1);}       /* DMA2 Stream 3                */
__attribute__((weak)) void DMA2_Stream4_IRQHandler      (void){while(1);}       /* DMA2 Stream 4                */
__attribute__((weak)) void ETH_IRQHandler               (void){while(1);}       /* Ethernet                     */
__attribute__((weak)) void ETH_WKUP_IRQHandler          (void){while(1);}       /* Ethernet Wakeup through EXTI line */
__attribute__((weak)) void CAN2_TX_IRQHandler           (void){while(1);}       /* CAN2 TX                      */
__attribute__((weak)) void CAN2_RX0_IRQHandler          (void){while(1);}       /* CAN2 RX0                     */
__attribute__((weak)) void CAN2_RX1_IRQHandler          (void){while(1);}       /* CAN2 RX1                     */
__attribute__((weak)) void CAN2_SCE_IRQHandler          (void){while(1);}       /* CAN2 SCE                     */
__attribute__((weak)) void OTG_FS_IRQHandler            (void){while(1);}       /* USB OTG FS                   */
__attribute__((weak)) void DMA2_Stream5_IRQHandler      (void){while(1);}       /* DMA2 Stream 5                */
__attribute__((weak)) void DMA2_Stream6_IRQHandler      (void){while(1);}       /* DMA2 Stream 6                */
__attribute__((weak)) void DMA2_Stream7_IRQHandler      (void){while(1);}       /* DMA2 Stream 7                */
__attribute__((weak)) void USART6_IRQHandler            (void){while(1);}       /* USART6                       */
__attribute__((weak)) void I2C3_EV_IRQHandler           (void){while(1);}       /* I2C3 event                   */
__attribute__((weak)) void I2C3_ER_IRQHandler           (void){while(1);}       /* I2C3 error                   */
__attribute__((weak)) void OTG_HS_EP1_OUT_IRQHandler    (void){while(1);}       /* USB OTG HS End Point 1 Out   */
__attribute__((weak)) void OTG_HS_EP1_IN_IRQHandler     (void){while(1);}       /* USB OTG HS End Point 1 In    */
__attribute__((weak)) void OTG_HS_WKUP_IRQHandler       (void){while(1);}       /* USB OTG HS Wakeup through EXTI */
__attribute__((weak)) void OTG_HS_IRQHandler            (void){while(1);}       /* USB OTG HS                   */
__attribute__((weak)) void DCMI_IRQHandler              (void){while(1);}       /* DCMI                         */

__attribute__((weak)) void HASH_RNG_IRQHandler          (void){while(1);}       /* Hash and Rng                 */
__attribute__((weak)) void FPU_IRQHandler               (void){while(1);}       /* FPU                          */

/*!****************************************************************************
* Interrupt vector table
*/
intVector_type intVector[] __attribute__ ((section (".isr_vector"))) = {
	///Stack
	(intVector_type)&_estack,
	///Cortex-M4 Processor Exceptions
	Reset_Handler,
	NMI_Handler,
	HardFault_Handler,
	MemManage_Handler,
	BusFault_Handler,
	UsageFault_Handler,
	0,
	0,
	0,
	0,
	SVC_Handler,
	DebugMon_Handler,
	0,
	PendSV_Handler,
	SysTick_Handler,
	///STM32 specific Interrupt
	WWDG_IRQHandler,
	PVD_IRQHandler,
	TAMP_STAMP_IRQHandler,
	RTC_WKUP_IRQHandler,
	FLASH_IRQHandler,
	RCC_IRQHandler,
	EXTI0_IRQHandler,
	EXTI1_IRQHandler,
	EXTI2_IRQHandler,
	EXTI3_IRQHandler,
	EXTI4_IRQHandler,
	DMA1_Stream0_IRQHandler,
	DMA1_Stream1_IRQHandler,
	DMA1_Stream2_IRQHandler,
	DMA1_Stream3_IRQHandler,
	DMA1_Stream4_IRQHandler,
	DMA1_Stream5_IRQHandler,
	DMA1_Stream6_IRQHandler,
	ADC_IRQHandler,
	CAN1_TX_IRQHandler,
	CAN1_RX0_IRQHandler,
	CAN1_RX1_IRQHandler,
	CAN1_SCE_IRQHandler,
	EXTI9_5_IRQHandler,
	TIM1_BRK_TIM9_IRQHandler,
	TIM1_UP_TIM10_IRQHandler,
	TIM1_TRG_COM_TIM11_IRQHandler,
	TIM1_CC_IRQHandler,
	TIM2_IRQHandler,
	TIM3_IRQHandler,
	TIM4_IRQHandler,
	I2C1_EV_IRQHandler,
	I2C1_ER_IRQHandler,
	I2C2_EV_IRQHandler,
	I2C2_ER_IRQHandler,
	SPI1_IRQHandler,
	SPI2_IRQHandler,
	USART1_IRQHandler,
	USART2_IRQHandler,
	USART3_IRQHandler,
	EXTI15_10_IRQHandler,
	RTC_Alarm_IRQHandler,
	OTG_FS_WKUP_IRQHandler,
	TIM8_BRK_TIM12_IRQHandler,
	TIM8_UP_TIM13_IRQHandler,
	TIM8_TRG_COM_TIM14_IRQHandler,
	TIM8_CC_IRQHandler,
	DMA1_Stream7_IRQHandler,
	FSMC_IRQHandler,
	SDIO_IRQHandler,
	TIM5_IRQHandler,
	SPI3_IRQHandler,
	UART4_IRQHandler,
	UART5_IRQHandler,
	TIM6_DAC_IRQHandler,
	TIM7_IRQHandler,
	DMA2_Stream0_IRQHandler,
	DMA2_Stream1_IRQHandler,
	DMA2_Stream2_IRQHandler,
	DMA2_Stream3_IRQHandler,
	DMA2_Stream4_IRQHandler,
	ETH_IRQHandler,
	ETH_WKUP_IRQHandler,
	CAN2_TX_IRQHandler,
	CAN2_RX0_IRQHandler,
	CAN2_RX1_IRQHandler,
	CAN2_SCE_IRQHandler,
	OTG_FS_IRQHandler,
	DMA2_Stream5_IRQHandler,
	DMA2_Stream6_IRQHandler,
	DMA2_Stream7_IRQHandler,
	USART6_IRQHandler,
	I2C3_EV_IRQHandler,
	I2C3_ER_IRQHandler,
	OTG_HS_EP1_OUT_IRQHandler,
	OTG_HS_EP1_IN_IRQHandler,
	OTG_HS_WKUP_IRQHandler,
	OTG_HS_IRQHandler,
	DCMI_IRQHandler,
	0,
	HASH_RNG_IRQHandler,
	FPU_IRQHandler,
};

/*************** LGPL ************** END OF FILE *********** D_EL ************/

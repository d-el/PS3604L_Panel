/*!****************************************************************************
* @file			startup_stm32f407xx.c
* @author		d_el - Storozhenko Roman
* @version		V1.0
* @date			07.07.2017
* @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
* @brief		Startup STM32F407xx
*/

/*!****************************************************************************
* Include
*/
#include "stdint.h"

extern void __libc_init_array(void);	///initialization c library array
extern void SystemInit(void);			///System initialization function
extern int main(void);					///Entry point for the application
void Reset_Handler(void);

extern uint32_t _estack;				///Highest address of the user mode stack
extern uint32_t _sdata;					///RAM data start
extern uint32_t _edata;					///RAM data end
extern uint32_t _sidata;				///ROM data start
extern uint32_t _siccmdata;				///ROM ccmdata start
extern uint32_t _sccmdata;				///CCMRAM ccmdata start
extern uint32_t _eccmdata;				///CCMRAM ccmdata end
extern uint32_t _sbss;					///RAM bss start
extern uint32_t _ebss;					///RAM bss end
extern uint32_t _sccmbss;				///CCMRAM bss start
extern uint32_t _eccmbss;				///CCMRAM bss end

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
 * @brief	Default interrupt handler
 */
void Default_Handler(void){
	/*
	 * IPSR bit assignments
	 * [8:0]	ISR_NUMBER
	 * This is the number of the current exception:
	 * 0 = Thread mode
	 * 1 = Reserved
	 * 2 = NMI
	 * ...
	 */
	while(1);
}

///Cortex-M4 Processor Exceptions
void NMI_Handler					(void) __attribute__((weak, alias ("Default_Handler")));
void HardFault_Handler				(void) __attribute__((weak, alias ("Default_Handler")));
void MemManage_Handler				(void) __attribute__((weak, alias ("Default_Handler")));
void BusFault_Handler				(void) __attribute__((weak, alias ("Default_Handler")));
void UsageFault_Handler				(void) __attribute__((weak, alias ("Default_Handler")));
void SVC_Handler					(void) __attribute__((weak, alias ("Default_Handler")));
void DebugMon_Handler				(void) __attribute__((weak, alias ("Default_Handler")));
void PendSV_Handler					(void) __attribute__((weak, alias ("Default_Handler")));
void SysTick_Handler				(void) __attribute__((weak, alias ("Default_Handler")));
///STM32 specific Interrupt
void WWDG_IRQHandler				(void) __attribute__((weak, alias ("Default_Handler")));		/* Window WatchDog			  */
void PVD_IRQHandler					(void) __attribute__((weak, alias ("Default_Handler")));		/* PVD through EXTI Line detection */
void TAMP_STAMP_IRQHandler			(void) __attribute__((weak, alias ("Default_Handler")));		/* Tamper and TimeStamps through the EXTI line */
void RTC_WKUP_IRQHandler			(void) __attribute__((weak, alias ("Default_Handler")));		/* RTC Wakeup through the EXTI line */
void FLASH_IRQHandler				(void) __attribute__((weak, alias ("Default_Handler")));		/* FLASH						  */
void RCC_IRQHandler					(void) __attribute__((weak, alias ("Default_Handler")));		/* RCC						  */
void EXTI0_IRQHandler				(void) __attribute__((weak, alias ("Default_Handler")));		/* EXTI Line0					  */
void EXTI1_IRQHandler				(void) __attribute__((weak, alias ("Default_Handler")));		/* EXTI Line1					  */
void EXTI2_IRQHandler				(void) __attribute__((weak, alias ("Default_Handler")));		/* EXTI Line2					  */
void EXTI3_IRQHandler				(void) __attribute__((weak, alias ("Default_Handler")));		/* EXTI Line3					  */
void EXTI4_IRQHandler				(void) __attribute__((weak, alias ("Default_Handler")));		/* EXTI Line4					  */
void DMA1_Stream0_IRQHandler		(void) __attribute__((weak, alias ("Default_Handler")));		/* DMA1 Stream 0				  */
void DMA1_Stream1_IRQHandler		(void) __attribute__((weak, alias ("Default_Handler")));		/* DMA1 Stream 1				  */
void DMA1_Stream2_IRQHandler		(void) __attribute__((weak, alias ("Default_Handler")));		/* DMA1 Stream 2				  */
void DMA1_Stream3_IRQHandler		(void) __attribute__((weak, alias ("Default_Handler")));		/* DMA1 Stream 3				  */
void DMA1_Stream4_IRQHandler		(void) __attribute__((weak, alias ("Default_Handler")));		/* DMA1 Stream 4				  */
void DMA1_Stream5_IRQHandler		(void) __attribute__((weak, alias ("Default_Handler")));		/* DMA1 Stream 5				  */
void DMA1_Stream6_IRQHandler		(void) __attribute__((weak, alias ("Default_Handler")));		/* DMA1 Stream 6				  */
void ADC_IRQHandler					(void) __attribute__((weak, alias ("Default_Handler")));		/* ADC1, ADC2 and ADC3s		  */
void CAN1_TX_IRQHandler				(void) __attribute__((weak, alias ("Default_Handler")));		/* CAN1 TX					  */
void CAN1_RX0_IRQHandler			(void) __attribute__((weak, alias ("Default_Handler")));		/* CAN1 RX0					  */
void CAN1_RX1_IRQHandler			(void) __attribute__((weak, alias ("Default_Handler")));		/* CAN1 RX1					  */
void CAN1_SCE_IRQHandler			(void) __attribute__((weak, alias ("Default_Handler")));		/* CAN1 SCE					  */
void EXTI9_5_IRQHandler				(void) __attribute__((weak, alias ("Default_Handler")));		/* External Line[9:5]s		  */
void TIM1_BRK_TIM9_IRQHandler		(void) __attribute__((weak, alias ("Default_Handler")));		/* TIM1 Break and TIM9		  */
void TIM1_UP_TIM10_IRQHandler		(void) __attribute__((weak, alias ("Default_Handler")));		/* TIM1 Update and TIM10		  */
void TIM1_TRG_COM_TIM11_IRQHandler	(void) __attribute__((weak, alias ("Default_Handler")));		/* TIM1 Trigger and Commutation and TIM11 */
void TIM1_CC_IRQHandler				(void) __attribute__((weak, alias ("Default_Handler")));		/* TIM1 Capture Compare		  */
void TIM2_IRQHandler				(void) __attribute__((weak, alias ("Default_Handler")));		/* TIM2						  */
void TIM3_IRQHandler				(void) __attribute__((weak, alias ("Default_Handler")));		/* TIM3						  */
void TIM4_IRQHandler				(void) __attribute__((weak, alias ("Default_Handler")));		/* TIM4						  */
void I2C1_EV_IRQHandler				(void) __attribute__((weak, alias ("Default_Handler")));		/* I2C1 Event					  */
void I2C1_ER_IRQHandler				(void) __attribute__((weak, alias ("Default_Handler")));		/* I2C1 Error					  */
void I2C2_EV_IRQHandler				(void) __attribute__((weak, alias ("Default_Handler")));		/* I2C2 Event					  */
void I2C2_ER_IRQHandler				(void) __attribute__((weak, alias ("Default_Handler")));		/* I2C2 Error					  */
void SPI1_IRQHandler				(void) __attribute__((weak, alias ("Default_Handler")));		/* SPI1						  */
void SPI2_IRQHandler				(void) __attribute__((weak, alias ("Default_Handler")));		/* SPI2						  */
void USART1_IRQHandler				(void) __attribute__((weak, alias ("Default_Handler")));		/* USART1						  */
void USART2_IRQHandler				(void) __attribute__((weak, alias ("Default_Handler")));		/* USART2						  */
void USART3_IRQHandler				(void) __attribute__((weak, alias ("Default_Handler")));		/* USART3						  */
void EXTI15_10_IRQHandler			(void) __attribute__((weak, alias ("Default_Handler")));		/* External Line[15:10]s		  */
void RTC_Alarm_IRQHandler			(void) __attribute__((weak, alias ("Default_Handler")));		/* RTC Alarm (A and B) through EXTI Line */
void OTG_FS_WKUP_IRQHandler			(void) __attribute__((weak, alias ("Default_Handler")));		/* USB OTG FS Wakeup through EXTI line */
void TIM8_BRK_TIM12_IRQHandler		(void) __attribute__((weak, alias ("Default_Handler")));		/* TIM8 Break and TIM12		  */
void TIM8_UP_TIM13_IRQHandler		(void) __attribute__((weak, alias ("Default_Handler")));		/* TIM8 Update and TIM13		  */
void TIM8_TRG_COM_TIM14_IRQHandler	(void) __attribute__((weak, alias ("Default_Handler")));		/* TIM8 Trigger and Commutation and TIM14 */
void TIM8_CC_IRQHandler				(void) __attribute__((weak, alias ("Default_Handler")));		/* TIM8 Capture Compare		  */
void DMA1_Stream7_IRQHandler		(void) __attribute__((weak, alias ("Default_Handler")));		/* DMA1 Stream7				  */
void FSMC_IRQHandler				(void) __attribute__((weak, alias ("Default_Handler")));		/* FSMC						  */
void SDIO_IRQHandler				(void) __attribute__((weak, alias ("Default_Handler")));		/* SDIO						  */
void TIM5_IRQHandler				(void) __attribute__((weak, alias ("Default_Handler")));		/* TIM5						  */
void SPI3_IRQHandler				(void) __attribute__((weak, alias ("Default_Handler")));		/* SPI3						  */
void UART4_IRQHandler				(void) __attribute__((weak, alias ("Default_Handler")));		/* UART4						  */
void UART5_IRQHandler				(void) __attribute__((weak, alias ("Default_Handler")));		/* UART5						  */
void TIM6_DAC_IRQHandler			(void) __attribute__((weak, alias ("Default_Handler")));		/* TIM6 and DAC1&2 underrun errors */
void TIM7_IRQHandler				(void) __attribute__((weak, alias ("Default_Handler")));		/* TIM7						  */
void DMA2_Stream0_IRQHandler		(void) __attribute__((weak, alias ("Default_Handler")));		/* DMA2 Stream 0				  */
void DMA2_Stream1_IRQHandler		(void) __attribute__((weak, alias ("Default_Handler")));		/* DMA2 Stream 1				  */
void DMA2_Stream2_IRQHandler		(void) __attribute__((weak, alias ("Default_Handler")));		/* DMA2 Stream 2				  */
void DMA2_Stream3_IRQHandler		(void) __attribute__((weak, alias ("Default_Handler")));		/* DMA2 Stream 3				  */
void DMA2_Stream4_IRQHandler		(void) __attribute__((weak, alias ("Default_Handler")));		/* DMA2 Stream 4				  */
void ETH_IRQHandler					(void) __attribute__((weak, alias ("Default_Handler")));		/* Ethernet					  */
void ETH_WKUP_IRQHandler			(void) __attribute__((weak, alias ("Default_Handler")));		/* Ethernet Wakeup through EXTI line */
void CAN2_TX_IRQHandler				(void) __attribute__((weak, alias ("Default_Handler")));		/* CAN2 TX					  */
void CAN2_RX0_IRQHandler			(void) __attribute__((weak, alias ("Default_Handler")));		/* CAN2 RX0					  */
void CAN2_RX1_IRQHandler			(void) __attribute__((weak, alias ("Default_Handler")));		/* CAN2 RX1					  */
void CAN2_SCE_IRQHandler			(void) __attribute__((weak, alias ("Default_Handler")));		/* CAN2 SCE					  */
void OTG_FS_IRQHandler				(void) __attribute__((weak, alias ("Default_Handler")));		/* USB OTG FS					  */
void DMA2_Stream5_IRQHandler		(void) __attribute__((weak, alias ("Default_Handler")));		/* DMA2 Stream 5				  */
void DMA2_Stream6_IRQHandler		(void) __attribute__((weak, alias ("Default_Handler")));		/* DMA2 Stream 6				  */
void DMA2_Stream7_IRQHandler		(void) __attribute__((weak, alias ("Default_Handler")));		/* DMA2 Stream 7				  */
void USART6_IRQHandler				(void) __attribute__((weak, alias ("Default_Handler")));		/* USART6						  */
void I2C3_EV_IRQHandler				(void) __attribute__((weak, alias ("Default_Handler")));		/* I2C3 event					  */
void I2C3_ER_IRQHandler				(void) __attribute__((weak, alias ("Default_Handler")));		/* I2C3 error					  */
void OTG_HS_EP1_OUT_IRQHandler		(void) __attribute__((weak, alias ("Default_Handler")));		/* USB OTG HS End Point 1 Out	  */
void OTG_HS_EP1_IN_IRQHandler		(void) __attribute__((weak, alias ("Default_Handler")));		/* USB OTG HS End Point 1 In	  */
void OTG_HS_WKUP_IRQHandler			(void) __attribute__((weak, alias ("Default_Handler")));		/* USB OTG HS Wakeup through EXTI */
void OTG_HS_IRQHandler				(void) __attribute__((weak, alias ("Default_Handler")));		/* USB OTG HS					  */
void DCMI_IRQHandler				(void) __attribute__((weak, alias ("Default_Handler")));		/* DCMI						  */

void HASH_RNG_IRQHandler			(void) __attribute__((weak, alias ("Default_Handler")));		/* Hash and Rng				  */
void FPU_IRQHandler					(void) __attribute__((weak, alias ("Default_Handler")));		/* FPU						  */

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

/*!****************************************************************************
 * @brief	Program entry point
 */
void Reset_Handler(void){
	volatile uint32_t* VTOR = (uint32_t*)0xE000ED08;
	*VTOR = (uint32_t)&intVector[0];
	__initializeData(&_sdata, &_edata, &_sidata);	//.data
	__initializeData(&_sccmdata, &_eccmdata, &_siccmdata);	//.ccmdata
	__initializeBss(&_sbss, &_ebss);	//.bss
	__initializeBss(&_sccmbss, &_eccmbss);	//.ccmbss

	__libc_init_array();

	SystemInit();
	main();
}

/******************************** END OF FILE ********************************/

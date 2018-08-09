/*!****************************************************************************
 * @file		clock.c
 * @author		d_el
 * @version		V1.0
 * @date		17.09.2017
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		MCU peripheral initialization
 */

/*!****************************************************************************
 * Include
 */
#include "stm32f4xx.h"
#include "clock.h"

#define HSI_VALUE			16000000	///< HSI VALUE [Hz]
//-----------------------------------------------------------------------------
#define HSE_VALUE			24000000	///< HSE VALUE [Hz]
//-----------------------------------------------------------------------------
#define HSE_STARTUP_TIMEOUT 1000000
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define PLL_M				12			///< PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N
//-----------------------------------------------------------------------------
#define PLL_N				168
//-----------------------------------------------------------------------------
#define PLL_P				2			///< SYSCLK = PLL_VCO / PLL_P
//-----------------------------------------------------------------------------
#define PLL_Q				7			///< USB OTG FS, SDIO and RNG Clock =  PLL_VCO / PLLQ
//-----------------------------------------------------------------------------
#define PLLI2S_N			50
//-----------------------------------------------------------------------------
#define PLLI2S_R			2
//-----------------------------------------------------------------------------
#define PLLI2S_Q			2

/**
 * @brief  Configures the System clock source, PLL Multiplier and Divider factors,
 *		   AHB/APBx prescalers and Flash settings
 * @Note   This function should be called only once the RCC clock configuration
 *		   is reset to the default reset state (done in SystemInit() function).
 * @param  None
 * @retval None
 */
clockState_type clock_init(void){
	/******************************************************************************/
	/*			  PLL (clocked by HSE) used as System clock source				  */
	/******************************************************************************/
	__IO uint32_t StartUpCounter = 0, HSEStatus = 0;
	clockState_type state;

	/* Enable HSE */
	RCC->CR |= ((uint32_t) RCC_CR_HSEON);

	/* Wait till HSE is ready and if Time out is reached exit */
	do{
		HSEStatus = RCC->CR & RCC_CR_HSERDY;
		StartUpCounter++;
	}while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

	if((RCC->CR & RCC_CR_HSERDY) != RESET){
		HSEStatus = (uint32_t) 0x01;
	}else{
		HSEStatus = (uint32_t) 0x00;
	}

	if(HSEStatus == (uint32_t) 0x01){
		/* Select regulator voltage output Scale 1 mode, System frequency up to 168 MHz */
		RCC->APB1ENR |= RCC_APB1ENR_PWREN;
		PWR->CR |= PWR_CR_VOS;

		/* HCLK = SYSCLK / 1*/
		RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

		/* PCLK2 = HCLK / 4*/
		RCC->CFGR |= RCC_CFGR_PPRE2_DIV4;

		/* PCLK1 = HCLK / 4*/
		RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;

		/* Configure the main PLL */
		RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) - 1) << 16) | (RCC_PLLCFGR_PLLSRC_HSE) | (PLL_Q << 24);

		/* Enable the main PLL */
		RCC->CR |= RCC_CR_PLLON;

		/* Wait till the main PLL is ready */
		while((RCC->CR & RCC_CR_PLLRDY) == 0){
		}

		/* Configure Flash prefetch, Instruction cache, Data cache and wait state */
		FLASH->ACR = FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS;

		/* Select the main PLL as system clock source */
		RCC->CFGR &= (uint32_t) ((uint32_t) ~(RCC_CFGR_SW));
		RCC->CFGR |= RCC_CFGR_SW_PLL;

		/* Wait till the main PLL is used as system clock source */
		while((RCC->CFGR & (uint32_t) RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL){
		}

		/*
		 * Config I2S Pll
		 */
		RCC->PLLI2SCFGR	= (PLLI2S_R << 28) | (PLLI2S_Q << 24) | (PLLI2S_N << 6);
		RCC->CR |= RCC_CR_PLLI2SON;
		/* Wait till the main PLL2 is ready */
		while((RCC->CR & RCC_CR_PLLI2SRDY) == 0){
		}

		state = clockOk;
	}else{ // If HSE fails to start-up, the application will have wrong clock configuration
		state = clockErorHse;
	}

	RCC->CFGR |= RCC_CFGR_MCO1_1;	//MCO1 - HSE clock selected
	RCC->CFGR |= RCC_CFGR_MCO2_0;	//MCO2 - PLLI2S clock selected

	SystemCoreClockUpdate();

	return state;
}

/******************************** END OF FILE ********************************/

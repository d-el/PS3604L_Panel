/*!****************************************************************************
 * @file		gpio.c
 * @author		d_el
 * @version		V1.1
 * @date		22.11.2026
 * @copyright	The MIT License (MIT). Copyright (c) 2026 Storozhenko Roman
 * @brief		gpio driver for stm32F4 microcontroller
 */

/*!****************************************************************************
 * Include
 */
#include "bitbanding.h"
#include "board.h"
#include "gpio.h"

/*!****************************************************************************
 * MEMORY
 */
const pinMode_type pinsMode[] = {
/*0 */makepin(GPIOE, 5,		digitalInput,	pullUp,					0, 0, speed2MHz), //bMode
/*1 */makepin(GPIOE, 2,		digitalInput,	pullUp,					0, 0, speed2MHz), //bOnOff
/*2 */makepin(GPIOE, 4,		digitalInput,	pullUp,					0, 0, speed2MHz), //bView
/*3 */makepin(GPIOE, 3,		digitalInput,	pullUp,					0, 0, speed2MHz), //bNext
/*4 */makepin(GPIOC, 6,		digitalInput,	pullUp,					0, 0, speed2MHz), //bZero
/*5 */makepin(GPIOD, 15,	digitalInput,	pullUp,					0, 0, speed2MHz), //bUp
/*6 */makepin(GPIOD, 14,	digitalInput,	pullUp,					0, 0, speed2MHz), //bDown

/*7 */makepin(GPIOD, 5,		outPushPull,	pullDisable,			0, 1, speed2MHz), //LCD_RST
/*8 */makepin(GPIOD, 6,		outPushPull,	pullDisable,			0, 1, speed25MHz), //LCD_CS
/*9 */makepin(GPIOD, 7,		outPushPull,	pullDisable,			0, 1, speed25MHz), //LCD_DC

/*10*/makepin(GPIOD, 10,	digitalInput,	pullUp,					0, 0, speed2MHz), //GP_GP0
/*11*/makepin(GPIOD, 11,	digitalInput,	pullUp,					0, 0, speed2MHz), //GP_GP1

/*12*/makepin(GPIOE, 13,	outPushPull,	pullDisable,			0, 0, speed2MHz), //LED_GP

/*13*/makepin(GPIOC, 3,		digitalInput,	pullUp,					0, 0, speed2MHz),	//LANnINT

/*14*/makepin(GPIOA, 8,		alternateFunctionPushPull, pullDisable, 0, 0, speed50MHz),	//MCO1
/*15*/makepin(GPIOC, 9,		alternateFunctionPushPull, pullDisable, 0, 0, speed50MHz),	//MCO2

};
const uint32_t pinNum = sizeof(pinsMode) / sizeof(pinMode_type);

/*!****************************************************************************
 * InitAllGpio
 */
void gpio_init(void){
	pinMode_type *pgpios;
	pinMode_type *pgpiosEnd;

	pgpios = (pinMode_type*)pinsMode;
	pgpiosEnd = pgpios + pinNum;

	while(pgpios < pgpiosEnd){
		gppin_init(pgpios->p, pgpios->npin, pgpios->mode, pgpios->pull, pgpios->iniState, pgpios->nAF, pgpios->ospeed);
		pgpios++;
	}
}

/*!****************************************************************************
 * InitAllGpio
 */
void gpio_compensationCellInit(void){
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;				// Enable SYSCFG clock
	SYSCFG->CMPCR |= SYSCFG_CMPCR_CMP_PD;				// I/O compensation cell enabled
	while((SYSCFG->CMPCR & SYSCFG_CMPCR_READY) == 0)	// Wait for compensation cell ready
	{}
}

/*!****************************************************************************
 *
 */
void gppin_init(GPIO_TypeDef *port, uint8_t npin, gpioMode_type mode, gpioPull_type pull, uint8_t iniState, uint8_t nAF, uint8_t ospeed){
	//Clock enable
		 if(port == GPIOA)	RCC->AHB1ENR	|= RCC_AHB1ENR_GPIOAEN;
	else if(port == GPIOB)	RCC->AHB1ENR	|= RCC_AHB1ENR_GPIOBEN;
	else if(port == GPIOC)	RCC->AHB1ENR	|= RCC_AHB1ENR_GPIOCEN;
	else if(port == GPIOD)	RCC->AHB1ENR	|= RCC_AHB1ENR_GPIODEN;
	else if(port == GPIOE)	RCC->AHB1ENR	|= RCC_AHB1ENR_GPIOEEN;
	else if(port == GPIOF)	RCC->AHB1ENR	|= RCC_AHB1ENR_GPIOFEN;
	else if(port == GPIOG)	RCC->AHB1ENR	|= RCC_AHB1ENR_GPIOGEN;
	else if(port == GPIOH)	RCC->AHB1ENR	|= RCC_AHB1ENR_GPIOHEN;
	else if(port == GPIOI)	RCC->AHB1ENR	|= RCC_AHB1ENR_GPIOIEN;

	//Set initial value
	if(iniState != 0){
		_gppin_set(port, 1 << npin);
	}
	else{
		_gppin_reset(port, 1 << npin);
	}

	/*
	* Clear bit field
	*/
	port->MODER			&= ~(0x03 << (2 * npin));
	port->OTYPER		&= ~(1<<npin);
	port->OSPEEDR		&= ~(0x03 << (2 * npin));
	port->PUPDR			&= ~(GPIO_RESERVED << (2 * npin));
	port->AFR[npin / 8] &= ~(GPIO_AFRL_AFSEL0_Msk << (4 * (npin % 8)));
	//Set number alternate function
	port->AFR[npin / 8] |= nAF << (4 * (npin % 8));

	//Set pull
	if(pull == pullUp){
		port->PUPDR |= GPIO_PULL_UP << (2 * npin);
	}else if(pull == pullDown){
		port->PUPDR |= GPIO_PULL_DOWN << (2 * npin);
	}

	//Set mode
	switch(mode){
		case analogMode:
			port->MODER		|= GPIO_ANALOG_MODE << (2 * npin);
			break;

		case digitalInput:
			port->MODER		&= ~(0x03 << (2 * npin));
			break;

		case outPushPull:
			port->MODER		|= GPIO_GP_OUT << (2 * npin);
			port->OTYPER	|= GPIO_PUSH_PULL << npin;
			port->OSPEEDR	|= ospeed << (2*npin);	// Speed
			break;

		case outOpenDrain:
			port->MODER		|= GPIO_GP_OUT << (2 * npin);
			port->OTYPER	|= GPIO_OPEN_DRAIN << npin;
			port->OSPEEDR	|= ospeed << (2*npin);	// Speed
			break;

		case alternateFunctionPushPull:
			port->MODER		|= GPIO_AF_MODE << (2 * npin);
			port->OTYPER	|= GPIO_PUSH_PULL << npin;
			port->OSPEEDR	|= ospeed << (2*npin);	// Speed
			break;

		case alternateFunctionOpenDrain:
			port->MODER		|= GPIO_AF_MODE << (2 * npin);
			port->OTYPER	|= GPIO_OPEN_DRAIN << npin;
			port->OSPEEDR	|= ospeed << (2*npin);	// Speed
			break;
	}
}

/******************************** END OF FILE ********************************/

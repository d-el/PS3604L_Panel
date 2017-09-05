/*!****************************************************************************
* @file     gpio.c
* @author   Storozhenko Roman - D_EL, 14.08.2016 - 4eef
* @version  V1.0
* @date     20.07.2016
* @date     02.08.2016  fix set nAF
* @brief    gpio driver for stm32F0
*/

/*!****************************************************************************
* Include
*/
#include "gpio.h"

/*!****************************************************************************
* MEMORY
*/
const pinMode_type   const pinsMode[] = {
/*1 */makepin(GPIOA, 0, 	digitalInput, 	pullUp, 		0, 0), //bOnOff
/*2 */makepin(GPIOA, 0, 	digitalInput, 	pullUp, 		0, 0), //bView
/*3 */makepin(GPIOA, 0, 	digitalInput, 	pullUp, 		0, 0), //bNext
/*4 */makepin(GPIOA, 0, 	digitalInput, 	pullUp, 		0, 0), //bZero
/*5 */makepin(GPIOA, 0, 	digitalInput, 	pullUp, 		0, 0), //bUp
/*6 */makepin(GPIOA, 0, 	digitalInput, 	pullUp, 		0, 0), //bDown

/*0 */makepin(GPIOD, 12, 	outPushPull,	pullDisable,    0,  0),  //LED0
/*1 */makepin(GPIOD, 13,	outPushPull,	pullDisable,    0,  0),  //LED1
/*2 */makepin(GPIOD, 14, 	outPushPull,	pullDisable,    0,  0),  //LED2
/*3 */makepin(GPIOD, 15, 	outPushPull,	pullDisable,    0,  0),  //LED3
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
        gppin_init(pgpios->p, pgpios->npin, pgpios->mode, pgpios->pull, pgpios->iniState, pgpios->nAF);
        pgpios++;
    }
}

/*!****************************************************************************
*
*/
#define GPIO_AFRL_AFSEL0_Pos                 (0U)
#define GPIO_AFRL_AFSEL0_Msk                 (0xFU << GPIO_AFRL_AFSEL0_Pos)    /*!< 0x0000000F */
#define GPIO_AFRL_AFSEL0                     GPIO_AFRL_AFSEL0_Msk
void gppin_init(GPIO_TypeDef *port, uint8_t npin, gpioMode_type mode, gpioPull_type pull, uint8_t iniState, uint8_t nAF){
    //Clock enable
         if(port == GPIOA)   RCC->AHB1ENR    |= RCC_AHB1ENR_GPIOAEN;
    else if(port == GPIOB)   RCC->AHB1ENR    |= RCC_AHB1ENR_GPIOBEN;
    else if(port == GPIOC)   RCC->AHB1ENR    |= RCC_AHB1ENR_GPIOCEN;
    else if(port == GPIOD)   RCC->AHB1ENR    |= RCC_AHB1ENR_GPIODEN;
    else if(port == GPIOE)   RCC->AHB1ENR    |= RCC_AHB1ENR_GPIOEEN;
    else if(port == GPIOF)   RCC->AHB1ENR    |= RCC_AHB1ENR_GPIOFEN;
    else if(port == GPIOG)   RCC->AHB1ENR    |= RCC_AHB1ENR_GPIOGEN;
    else if(port == GPIOH)   RCC->AHB1ENR    |= RCC_AHB1ENR_GPIOHEN;
    else if(port == GPIOI)   RCC->AHB1ENR    |= RCC_AHB1ENR_GPIOIEN;

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
    port->MODER         &= ~(0x03 << (2 * npin));
    port->OTYPER        &= ~(1<<npin);
    port->PUPDR         &= ~(GPIO_RESERVED << (2*npin));
    port->AFR[npin / 8] &= ~(GPIO_AFRL_AFSEL0_Msk << (4*(npin % 8)));

    //Set pull
    if(pull == pullUp){
        port->PUPDR |= GPIO_PULL_UP << (2*npin);
    }else if(pull == pullDown){
        port->PUPDR |= GPIO_PULL_DOWN << (2*npin);
    }

    //Set mode
    switch(mode){
        case analogMode:
            port->MODER |= GPIO_ANALOG_MODE << (2*npin);
            break;

        case digitalInput:
            port->MODER         &= ~(0x03 << (2 * npin));
            break;

        case outPushPull:
            port->MODER |= GPIO_GP_OUT << (2*npin);
            port->OTYPER |= GPIO_PUSH_PULL << npin;
            break;

        case outOpenDrain:
            port->MODER     |= GPIO_GP_OUT << (2*npin);
            port->OTYPER    |= GPIO_OPEN_DRAIN << npin;
            break;

       case alternateFunctionPushPull:
            port->MODER     |= GPIO_AF_MODE << (2*npin);
            port->OTYPER    |= GPIO_PUSH_PULL << npin;
            port->OSPEEDR   |= 3 << (2*npin);   //High speed
            break;

        case alternateFunctionOpenDrain:
            port->MODER     |= GPIO_AF_MODE << (2*npin);
            port->OTYPER    |= GPIO_OPEN_DRAIN << npin;
            break;
    }

    //Set number alternate function
    port->AFR[npin / 8] |= nAF << (4*(npin % 8));
}

/*************** (C) COPYRIGHT ************** END OF FILE ********* D_EL *****/

/*!****************************************************************************
 * @file		cm4.c
 * @author		d_el
 * @version		V1.0
 * @date		Feb 15, 2025
 * @copyright	License (MIT). Copyright (c) 2024 Storozhenko Roman
 * @brief
 */

/*!****************************************************************************
 * Include
 */
#include "stm32f4xx.h"
#include "cm4.h"

/*!****************************************************************************
 * MEMORY
 */

/*!****************************************************************************
 * @brief
 */
void cm4_executeprogram(uint32_t* exe){
	// Peripheral reset enable
	RCC->AHB1RSTR = 0xFFFFFFFF;
	RCC->AHB2RSTR = 0xFFFFFFFF;
	RCC->AHB3RSTR = 0xFFFFFFFF;
	RCC->APB1RSTR = 0xFFFFFFFF;
	RCC->APB2RSTR = 0xFFFFFFFF;

	// Peripheral  reset disable
	RCC->AHB1RSTR = 0;
	RCC->AHB2RSTR = 0;
	RCC->AHB3RSTR = 0;
	RCC->APB1RSTR = 0;
	RCC->APB2RSTR = 0;

	// Peripheral  disable clock
	RCC->AHB1ENR = 0;
	RCC->AHB2ENR = 0;
	RCC->AHB3ENR = 0;
	RCC->APB1ENR = 0;
	RCC->APB2ENR = 0;

	// Disable Systysk
	SysTick->CTRL = 0;
	SysTick->LOAD = 0;
	SysTick->VAL = 0;

	// Disable interrupt
	NVIC->ICER[0] = 0xFFFFFFFF;
	NVIC->ICER[1] = 0xFFFFFFFF;
	NVIC->ICER[2] = 0xFFFFFFFF;
	NVIC->ICER[3] = 0xFFFFFFFF;
	NVIC->ICER[4] = 0xFFFFFFFF;
	NVIC->ICER[5] = 0xFFFFFFFF;
	NVIC->ICER[6] = 0xFFFFFFFF;
	NVIC->ICER[7] = 0xFFFFFFFF;
	__DSB();
	__ISB();

	// Pending
	NVIC->ICPR[0] = 0xFFFFFFFF;
	NVIC->ICPR[1] = 0xFFFFFFFF;
	NVIC->ICPR[2] = 0xFFFFFFFF;
	NVIC->ICPR[3] = 0xFFFFFFFF;
	NVIC->ICPR[4] = 0xFFFFFFFF;
	NVIC->ICPR[5] = 0xFFFFFFFF;
	NVIC->ICPR[6] = 0xFFFFFFFF;
	NVIC->ICPR[7] = 0xFFFFFFFF;
	__DSB();
	__ISB();

	SCB->CPACR &= ~((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */

	SCB->VTOR = 0;
	__DSB();
	__ISB();

	// Get PC and SP of application region
	uint32_t msp  = exe[0];
	uint32_t psp  = 0;
	__set_CONTROL(0); // Change from PSP to MSP

	asm volatile (	"MSR msp, %0\n"	//Set the Main Stack Pointer
					"MSR psp, %1\n"	//Set the Process Stack Pointer
					"MOV LR, #0xFFFFFFFF\n" //Reset Link Register
					"MOV r0, %2\n"
					"LDR SP, [R0]\n" //Load new stack pointer address
					"MOV R1, #0\n"
					"MOV R2, #0\n"
					"MOV R3, #0\n"
					"MOV R4, #0\n"
					"MOV R5, #0\n"
					"MOV R6, #0\n"
					"MOV R7, #0\n"
					"MOV R8, #0\n"
					"MOV R9, #0\n"
					"MOV R10, #0\n"
					"MOV R11, #0\n"
					"MOV R12, #0\n"
					"DSB\n"
					"ISB\n"
					"LDR PC, [R0, #4]"//Load new program counter address
				:
				: "r" (msp), "r" (psp), "r" (exe)
				:
				);
}

/******************************** END OF FILE ********************************/

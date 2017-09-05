/*!****************************************************************************
 * @file		debugCore.h
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		15.01.2016
 * @copyright	GNU Lesser General Public License v3
 * @brief		Debug utils, HardFault_Handler tracer
 */

/*!****************************************************************************
 * Include
 */
#include "debugCore.h"

/*!****************************************************************************
 * Memory
 */

/*!****************************************************************************
 * @brief
 * @retval 1 - debug mode enable
 *         0 - debug mode disable
 */
uint32_t coreIsInDebugMode(void){
	if((CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk) != 0){
		return 1;   //Debug mode enable
	}else{
		return 0;   //Debug mode disable
	}
}

/*!****************************************************************************
 * hard fault handler in C,
 * with stack frame location as input parameter
 * called from HardFault_Handler
 */
void hardFaultHandlerC(unsigned int * stackedContextPtr){
	volatile uint32_t stacked_r0;
	volatile uint32_t stacked_r1;
	volatile uint32_t stacked_r2;
	volatile uint32_t stacked_r3;
	volatile uint32_t stacked_r12;
	volatile uint32_t stacked_lr;
	volatile uint32_t stacked_pc;
	volatile uint32_t stacked_psr;

	stacked_r0 = stackedContextPtr[0];
	stacked_r1 = stackedContextPtr[1];
	stacked_r2 = stackedContextPtr[2];
	stacked_r3 = stackedContextPtr[3];
	stacked_r12 = stackedContextPtr[4];
	stacked_lr = stackedContextPtr[5];
	stacked_pc = stackedContextPtr[6];
	stacked_psr = stackedContextPtr[7];

	debug("\n\n[GAME OVER]\n");
	debug("R0 = 0x%008X\n", stacked_r0);
	debug("R1 = 0x%008X\n", stacked_r1);
	debug("R2 = 0x%008X\n", stacked_r2);
	debug("R3 = 0x%008X\n", stacked_r3);
	debug("R12 = 0x%08X\n", stacked_r12);
	debug("LR [R14] = 0x%08X  subroutine call return address\n", stacked_lr);
	debug("PC [R15] = 0x%08X  program counter\n", stacked_pc);
	debug("PSR = 0x%08X\n", stacked_psr);

	// Configurable Fault Status Register
	// Consists of MMSR, BFSR and UFSR
	debug("CFSR = 0x%08X\n", SCB->CFSR);

	// Hard Fault Status Register
	debug("HFSR = 0x%08X\n", SCB->HFSR);

	// Debug Fault Status Register
	debug("DFSR = 0x%08X\n", SCB->DFSR);

	// Auxiliary Fault Status Register
	debug("AFSR = 0x%08X\n", SCB->AFSR);

	// Read the Fault Address Registers. These may not contain valid values.
	// Check BFARVALID/MMARVALID to see if they are valid values
	// MemManage Fault Address Register
	debug("MMFAR = 0x%08X\n", SCB->MMFAR);

	// Bus Fault Address Register
	debug("BFAR = 0x%08X\n", SCB->BFAR);

	asm("BKPT #1");
	while(1)
		;
}

/*!****************************************************************************
 *
 */
void HardFault_Handler(void){
	__asm volatile (
			" 		MOVS   R0, #4							\n" /* Determine if processor uses PSP or MSP by checking bit.4 at LR register.		*/
			"		MOV    R1, LR							\n"
			"		TST    R0, R1							\n"
			"		BEQ    _IS_MSP							\n" /* Jump to '_MSP' if processor uses MSP stack.									*/
			"_IS_PSP:                                       \n"
			"		MRS    R0, PSP							\n" /* Prepare PSP content as parameter to the calling function below.				*/
			"		BL	   hardFaultHandlerC      			\n" /* Call 'hardfaultGetContext' passing PSP content as stackedContextPtr value.	*/
			"_IS_MSP:										\n"
			"		MRS    R0, MSP							\n" /* Prepare MSP content as parameter to the calling function below.				*/
			"		BL	   hardFaultHandlerC		        \n" /* Call 'hardfaultGetContext' passing MSP content as stackedContextPtr value.	*/
			:: );
}

/*************** LGPL ************** END OF FILE *********** D_EL ************/

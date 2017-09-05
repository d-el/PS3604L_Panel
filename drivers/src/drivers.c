/*!****************************************************************************
 * @file		drivers.c
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		05-07-2013
 * @copyright	GNU Lesser General Public License v3
 * @brief		MCU peripheral initialization
 */

/*!****************************************************************************
 * Include
 */
#include "drivers.h"

/*!****************************************************************************
 * MCU peripheral initialization
 */
void hardInit(void){
	//clock_init();
	SystemCoreClockUpdate();
	gpio_init();

	ETH_BSP_Config();	//configure Ethernet (GPIOs, clocks, MAC, DMA)
	//sysTimeMeasEnable();
	//enco_init();
	//beep_init();
	//ledPwm_init();
	spfd_init();
	//rtc_init();
	//i2c_init(i2c2);
	uart_init(uart1, BR9600);
	//uart_init(uart2, BR38400);
	//pvd_init();
}

/*************** LGPL ************** END OF FILE *********** D_EL ************/

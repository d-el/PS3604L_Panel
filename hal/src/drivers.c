/*!****************************************************************************
 * @file		drivers.с
 * @author		d_el
 * @version		V1.0
 * @date		05.07.2013
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 */

/*!****************************************************************************
 * Include
 */
#include "i2c.h"
#include "pvd.h"
#include "htimer.h"
#include "stm32f4x7_eth_bsp.h"
#include "beep.h"
#include "uart.h"
#include "enco.h"
#include "clock.h"
#include "ledpwm.h"
#include "rtc.h"
#include "sysTimeMeas.h"
#include "gpio.h"
#include "rng.h"
#include "uniqueDeviceId.h"

clockState_type gClockState;
bool gRtcOscillatorInit;

/*!****************************************************************************
 * MCU peripheral initialization
 */
void hardInit(void){
	gClockState = clock_init();
	htimer_init();
	gpio_init();
	ETH_BSP_Config();			//Configure Ethernet (GPIOs, clocks, MAC, DMA)
	sysTimeMeasEnable();
	enco_init();
	beep_init();
	ledPwm_init();
	gRtcOscillatorInit = rtc_init();
	rng_init();
	pvd_init();
}

/******************************** END OF FILE ********************************/

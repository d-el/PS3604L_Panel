/*!****************************************************************************
 * @file		st7735.с
 * @author		d_el
 * @version		V1.0
 * @date		05.07.2013
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 */

/*!****************************************************************************
 * Include
 */
#include "../inc/i2c22.h"
#include "pvd.h"
#include "stm32f4x7_eth_bsp.h"
#include "beep.h"
#include "st7735.h"
#include "uart.h"
#include "enco.h"
#include "clock.h"
#include "ledpwm.h"
#include "rtc.h"
#include "sysTimeMeas.h"
#include "gpio.h"
#include "rng.h"
#include "systemTSK.h"
#include "uniqueDeviceId.h"
#include "24AAxx.h"

uint8_t macAddress[6];

/*!****************************************************************************
 * @brief	Generate MAC address from MCU uID
 */
void macAddressGen(void){
	uint32_t mach = makeID(getDid());
	macAddress[0] = 0x28;
	macAddress[1] = 0x05;
	macAddress[2] = ( mach >> 0)  & 0xFF;
	macAddress[3] = ( mach >> 8)  & 0xFF;
	macAddress[4] = ( mach >> 16) & 0xFF;
	macAddress[5] = ( mach >> 24) & 0xFF;
}

/*!****************************************************************************
 * MCU peripheral initialization
 */
void hardInit(void){
	clockState_type clockState = clock_init();
	if(clockState == clockOk){
		fp.state.mainOscillatorError = 0;
	}else{
		fp.state.mainOscillatorError = 1;
	}
	gpio_init();
	uart_init(uart1, BR38400);
	uart_init(uart3, BR57600);
	uart_init(uart4, BR1M);		//Debug out
	macAddressGen();
	ETH_BSP_Config();			//Configure Ethernet (GPIOs, clocks, MAC, DMA)
	sysTimeMeasEnable();
	enco_init();
	beep_init();
	ledPwm_init();
	initR(INITR_BLACKTAB);	 	//Initialize 1.8" TFT - ST7735S chip, black tab
	rtcStatus_type rtcStatus = rtc_init();
	if((rtcStatus == rtc_Ok)&&(rtcStatus == rtc_wasOn)){
		fp.state.rtcOscillatorError = 0;
	}else{
		fp.state.rtcOscillatorError = 1;
	}
	i2c_init(i2c1);
	rng_init();
	eep_init();
	pvd_init();
}

/******************************** END OF FILE ********************************/

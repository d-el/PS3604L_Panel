/*!****************************************************************************
 * @file		systemTSK.c
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		14.09.2015
 * @copyright	GNU Lesser General Public License v3
 * @brief		System control task
 */

/*!****************************************************************************
 * Include
 */
#include "systemTSK.h"

/*!****************************************************************************
 * Memory
 */
TaskHandle_t 	windowTskHandle;
frontPanel_type fp;		///< Data structure front panel
struct netif xnetif; 	///< Network interface structure

/*!****************************************************************************
 * Local prototypes for the functions
 */
void loadParameters(void);
void LwIP_Init(const uint8_t *ipaddr, const uint8_t *netmask, const uint8_t *gateway);

/*!****************************************************************************
 * @brief
 */
void systemTSK(void *pPrm){
	TickType_t 		xLastWakeTime = xTaskGetTickCount();

	selWindow_type 	selWindowPrev = noneWindow;
	BaseType_t 		Result = pdTRUE;

	loadParameters();
	pvd_setSuplyFaultCallBack(shutdown);

	// Initilaize the LwIP stack
	uint8_t ip[4] 		= {192	,168	,1		,10};
	uint8_t netmask[4]	= {255	,255	,255	,0};
	uint8_t gateway[4]	= {192	,168	,1		,1};
	LwIP_Init(ip, netmask, gateway);
	ping_init();
	sntp_init();
	Result &= xTaskCreate(httpServerTSK, "httpServerTSK", HTTP_TSK_SZ_STACK, NULL, HTTP_TSK_PRIO, NULL);

	while(1){
		if(selWindowPrev != fp.currentSelWindow){
			if(windowTskHandle != NULL){
				vTaskDelete(windowTskHandle);	//Удаляем текущее окно
			}

			switch(fp.currentSelWindow){
				case startupWindow:
					Result = xTaskCreate(startupTSK, "startupTSK", STARTUP_TSK_SZ_STACK, NULL, STARTUP_TSK_PRIO, &windowTskHandle);
					break;
				case settingWindow:
					Result = xTaskCreate(settingTSK, "settingTSK", SETT_TSK_SZ_STACK, NULL, SETT_TSK_PRIO, &windowTskHandle);
					break;
				case baseWindow:
					Result = xTaskCreate(baseTSK, "baseTSK", BASE_TSK_SZ_STACK, NULL, BASE_TSK_PRIO, &windowTskHandle);
					break;
				case chargerWindow:
					Result = xTaskCreate(chargeTSK, "chargeTSK", CHARG_TSK_SZ_STACK, NULL, CHARG_TSK_PRIO, &windowTskHandle);
					break;
				case cube3dWindow:
					Result = xTaskCreate(cube3dTSK, "cube3dTSK", CUBE_TSK_SZ_STACK, NULL, CUBE_TSK_PRIO, &windowTskHandle);
					break;
				default:
					while(1)
						;	//Fail windowTskHandle
			}

			if(Result != pdTRUE){
				while(1)
					;
			}

			selWindowPrev = fp.currentSelWindow;
		}

		/*********************
		 *
		 */
		if(wlan.wlanRxActive != 0){
			if(windowTskHandle != NULL){
				vTaskDelete(windowTskHandle);	//Удаляем текущее окно
			}

			lcd_setColor(black, white);
			lcd_fillScreen(black);

			while(1){
				if(wlan.wlanRxActive){
					if(wlan.task.request != setNone){
						sendCommand(wlan.task.request);
					}
					fp.tf.task.u = wlan.task.u;
					fp.tf.task.i = wlan.task.i;
					fp.tf.task.mode = wlan.task.mode;

					wlan.wlanRxActive = 0;
				}
				uint32_t 				measV;	//[uV]
				uint32_t 				measI;	//[uA]

				/**************************************
				 * Перекладываем измеренные данные
				 */
				measV = fp.tf.meas.u;
				if(measV > 99999999){
					measV = 99999999;
				}
				measI = fp.tf.meas.i;
				if(measI > 9999999){
					measI = 9999999;
				}

				//Печать значения напряжения
				if(fp.tf.state.bit.switchIsON != 0){
					sprintf(str, "%02u.%03u", measV / 1000000, (measV / 1000) % 1000);
				}else{
					sprintf(str, "%02u.%03u", fp.tf.task.u / 1000000, (fp.tf.task.u / 1000) % 1000);
				}
				lcd_setColor(black, ui.color.voltage);
				lcd_putStr(16, 0, &dSegBold, 6, str);
				lcd_putChar(150, 18, &font8x12, 'V');

				//Печать значения тока
				lcd_setColor(black, ui.color.current);
				if(fp.tf.state.bit.switchIsON != 0){
					if(measI < 99000){
						sprintf(str, "%2u.%03u", measI / 1000, measI % 1000);
						lcd_putChar(150, 36, &font8x12, 'm');
						lcd_putChar(150, 49, &font8x12, 'A');
					}else{
						sprintf(str, "%2u.%03u", measI / 1000000, (measI / 1000) % 1000);
						lcd_putChar(150, 36, &font8x12, ' ');
						lcd_putChar(150, 49, &font8x12, 'A');
					}

				}else{
					strcpy(str, "--.---");
					lcd_putChar(150, 36, &font8x12, ' ');
					lcd_putChar(150, 49, &font8x12, ' ');
				}
				lcd_putStr(16, 36, &dSegBold, 6, str);

				sprintf(str, "u= %08u", wlan.task.u);
				lcd_putStr(0, 80, &font8x12, 0, str);
				sprintf(str, "i= %08u", wlan.task.i);
				lcd_putStr(0, 100, &font8x12, 0, str);

				vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(SYSTEM_TSK_PERIOD));
			}


			/*sprintf(str, "t= %08u", wlan.task.time);
			lcd_putStr(0, 30, &font8x12, 0, str);
			sprintf(str, "m= %08u", wlan.task.mode);
			lcd_putStr(0, 30, &font8x12, 0, str);*/
		}

		/**************************************
		 * Вызов периодических функций
		 */
		static uint8_t ledCount = 0;
		/*if(ledCount++ == 10){
			LED_ON();
			ledCount = 0;
		}
		if(ledCount == 1){
			LED_OFF();
		}
		if((ledCount == 2) && (uartTsk.state == uartConnect)){
			LED_ON();
		}
		if(ledCount == 3){
			LED_OFF();
		}*/

		//Link Down
		if(gppin_get(GP_LANnINT) == 0){
			ETH_ReadPHYRegister(1, PHY_BSR);
			if(fp.state.lanLink != 0){
				netif_set_down(&xnetif);
				fp.state.lanLink = 0;
			}
		}

		//Link Up
		if(fp.state.lanLink == 0){
			if(ETH_AutoNegotiation(1, NULL) == ETH_SUCCESS){
				netif_set_up(&xnetif);	//When the netif is fully configured this function must be called
				fp.state.lanLink = 1;
			}
		}

		/*************************************/
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(SYSTEM_TSK_PERIOD));
	}
}

/*!****************************************************************************
 * @brief	Load parameters from memory
 */
void loadParameters(void){
	nvMem_state_type nvMemState;

	prm_loadDefault();

	nvMem_init(&userConfRegion);
	nvMem_init(&userReservConfRegion);
	nvMem_init(&systemSettingRegion);

	nvMemState = nvMem_loadPrm(&systemSettingRegion);
	if(nvMemState != nvMem_ok){
		fp.state.sysSettingLoadDefault = 1;
	}
	nvMemState = nvMem_loadPrm(&userConfRegion);
	if(nvMemState != nvMem_ok){
		//Попробуем считать резервную копию
		nvMemState = nvMem_loadPrm(&userReservConfRegion);
		if(nvMemState != nvMem_ok){
			fp.state.userSettingLoadDefault = 1;
		}
	}else{
		nvMem_savePrm(&userReservConfRegion);
	}
}

/*!****************************************************************************
 * @brief	Select window task & wait selectable
 * 			This function need call from current GUI window
 * @param 	window	window task identifier
 */
void selWindow(selWindow_type window){
	fp.currentSelWindow = window;
	//vTaskSuspend(NULL);
	while(windowTskHandle != NULL){
		vTaskDelay(1000);
	}
}

/*!****************************************************************************
 * Выключение
 */
void shutdown(void){
	pvd_disable();
	setLcdBrightness(0);
	LED_OFF();
	nvMem_savePrm(&userConfRegion);
	spfd_disable();
	BeepTime(ui.beep.goodbye.time, ui.beep.goodbye.freq);
	LED_ON();
	//delay_ms(10000);
	NVIC_SystemReset();
}

/*!****************************************************************************
 * @param ip
 * @param netmask
 * @param gateway
 */
void LwIP_Init(const uint8_t *ipaddr, const uint8_t *netmask, const uint8_t *gateway){
	ip_addr_t l_ipaddr;
	ip_addr_t l_netmask;
	ip_addr_t l_gateway;

	memcpy(&l_ipaddr.addr, ipaddr, sizeof(l_ipaddr.addr));
	memcpy(&l_netmask.addr, netmask, sizeof(l_netmask.addr));
	memcpy(&l_gateway.addr, gateway, sizeof(l_gateway.addr));

	/* Create tcp_ip stack thread */
	tcpip_init( NULL, NULL);

	/* - netif_add(struct netif *netif, struct ip_addr *ipaddr,
	 struct ip_addr *netmask, struct ip_addr *gw,
	 void *state, err_t (* init)(struct netif *netif),
	 err_t (* input)(struct pbuf *p, struct netif *netif))

	 Adds your network interface to the netif_list. Allocate a struct
	 netif and pass a pointer to this structure as the first argument.
	 Give pointers to cleared ip_addr structures when using DHCP,
	 or fill them with sane numbers otherwise. The state pointer may be NULL.

	 The init function pointer must point to a initialization function for
	 your ethernet netif interface. The following code illustrates it's use.*/

	netif_add(&xnetif, &l_ipaddr, &l_netmask, &l_gateway, NULL, &ethernetif_init, &tcpip_input);

	/*  Registers the default network interface. */
	netif_set_default(&xnetif);

	//netif_set_up(&xnetif);
}

/*************** LGPL ************** END OF FILE *********** D_EL ************/

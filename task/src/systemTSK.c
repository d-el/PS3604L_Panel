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
frontPanel_type	fp;					///< Data structure front panel
struct netif	xnetif; 			///< Network interface structure
TaskHandle_t	windowTskHandle;	///< Program task handler

/*!****************************************************************************
 * Local prototypes for the functions
 */
void loadParameters(void);
void LwIP_Init(const uint32_t ipaddr, const uint32_t netmask, const uint32_t gateway);

/*!****************************************************************************
 * @brief
 */
void systemTSK(void *pPrm){
	TickType_t 		xLastWakeTime = xTaskGetTickCount();

	selWindow_type 	selWindowPrev = noneWindow;
	BaseType_t 		Result = pdTRUE;

	loadParameters();												// Load panel settings and user parameters
	pvd_setSupplyFaultCallBack(shutdown);							// Setup callback for Supply Fault
	LwIP_Init(fp.fpSet.ipadr, fp.fpSet.netmask, fp.fpSet.gateway);	// Initialize the LwIP stack
	ping_init();													// Initialize service ping protocol
	sntp_init();													// Initialize service SNTP
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
				while(1)	// Create task failed
					;
			}

			selWindowPrev = fp.currentSelWindow;
		}

		/**************************************
		 * Вызов периодических функций
		 */
		static uint8_t ledCount = 0;
		if(ledCount++ == 10){
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
		}

		/*
		 * link management
		 */
		if(gppin_get(GP_LANnINT) == 0){	//Detect by GPIO
			ETH_ReadPHYRegister(1, PHY_BSR);
			if(fp.state.lanLink != 0){
				netif_set_down(&xnetif);
				fp.state.lanLink = 0;
			}
		}
		if(fp.state.lanLink == 0){	//Detect by read status register
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
	while(windowTskHandle != NULL){
		 vTaskSuspend(NULL);	//Suspend current window
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
 * @param ip:		 Internet Protocol
 * @param netmask:
 * @param gateway
 */
void LwIP_Init(const uint32_t ipaddr, const uint32_t netmask, const uint32_t gateway){
	ip_addr_t l_ipaddr;
	ip_addr_t l_netmask;
	ip_addr_t l_gateway;

	//With convert 32-bits host order to network order
	l_ipaddr.addr = htonl(ipaddr);
	l_netmask.addr = htonl(netmask);
	l_gateway.addr = htonl(gateway);

	debug("ip %s\n", ipaddr_ntoa(&l_ipaddr));
	debug("nm: %s\n", ipaddr_ntoa(&l_netmask));
	debug("gw: %s\n", ipaddr_ntoa(&l_gateway));

	/* Create tcp_ip stack thread */
	tcpip_init( NULL, NULL);

	/*
	 * Adds your network interface to the netif_list. Allocate a struct
	 * netif and pass a pointer to this structure as the first argument.
	 * Give pointers to cleared ip_addr structures when using DHCP,
	 * or fill them with sane numbers otherwise. The state pointer may be NULL.
	 *
	 * The init function pointer must point to a initialization function for
	 * your ethernet netif interface. The following code illustrates it's use.
	 */
	netif_add(&xnetif, &l_ipaddr, &l_netmask, &l_gateway, NULL, &ethernetif_init, &tcpip_input);

	/*  Registers the default network interface. */
	netif_set_default(&xnetif);

	/*
	 * Далее производится детектирование Link и Negotiation, перенастройка
	 * MAC под текущее физическое подключение, netif_set_up
	 */
}

/*************** LGPL ************** END OF FILE *********** D_EL ************/

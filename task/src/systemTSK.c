/*!****************************************************************************
 * @file		systemTSK.c
 * @author		d_el
 * @version		V1.0
 * @date		14.09.2015
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		System control task
 */

/*!****************************************************************************
 * Include
 */
#include <inttypes.h>
#include <stdlib.h>
#include <assert.h>
#include "plog.h"
#include "prmSystem.h"
#include "pvd.h"
#include "board.h"
#include "ledpwm.h"
#include "sntp.h"
#include "stm32f4x7_eth.h"
#include "ethernetif.h"
#include "tcpip.h"
#include "lwip/dns.h"
#include "regulatorConnTSK.h"
#include "startupTSK.h"
#include "settingTSK.h"
#include "cube3dTSK.h"
#include "bubblesTSK.h"
#include "chargeTSK.h"
#include "httpServerTSK.h"
#include "baseTSK.h"
#include "monitorTSK.h"
#include "plog.h"
#include "write.h"

/*!****************************************************************************
 * Memory
 */
frontPanel_type	fp;					///< Data structure front panel
TaskHandle_t	windowTskHandle;	///< Program task handler
struct netif	xnetif; 			///< Network interface structure

/*!****************************************************************************
 * Local prototypes for the functions
 */
int _write(int fd, const void *buf, size_t count);
void loadParameters(void);
void LwIP_Init(const uint32_t ipaddr, const uint32_t netmask, const uint32_t gateway);
void netSettingUpdate(void);
void shutdown(void);

/**
 * SYS_DEBUG_LEVEL: Enable debugging for system task
 */
#define TASK_MONITOR_EN	0
#if(TASK_MONITOR_EN == 0)
	#define LOG_LOCAL_LEVEL P_LOG_VERBOSE
#else
	#define LOG_LOCAL_LEVEL P_LOG_NONE
#endif
static char *logTag = "SYS";

/*!****************************************************************************
 * @brief
 */
void systemTSK(void *pPrm){
	(void)pPrm;
	TickType_t 		xLastWakeTime = xTaskGetTickCount();
	selWindow_type 	selWindowPrev = noneWindow;

	//Init log system
	plog_setVprintf(vsprintf);
	plog_setWrite(_write);
	plog_setTimestamp(xTaskGetTickCount);
	plog_setWriteFd(write_uart); // write_semihost, write_uart

	P_LOGI(logTag, "Started systemTSK");

	loadParameters();												// Load panel settings and user parameters
	timezoneUpdate();
	pvd_setSupplyFaultCallBack(shutdown);							// Setup callback for Supply Fault

	LwIP_Init(fp.fpSet.ipadr, fp.fpSet.netmask, fp.fpSet.gateway);	// Initialize the LwIP stack
	sntp_init();													// Initialize service SNTP

	BaseType_t osres = xTaskCreate(regulatorConnTSK, "regulatorConnTSK", UART_TSK_SZ_STACK, NULL, UART_TSK_PRIO, NULL);
	assert(osres == pdTRUE);
	P_LOGI(logTag, "Started uartTSK");

	osres = xTaskCreate(httpServerTSK, "httpServerTSK", HTTP_TSK_SZ_STACK, NULL, HTTP_TSK_PRIO, NULL);
	assert(osres == pdTRUE);
	P_LOGI(logTag, "Started httpServerTSK");

#if(TASK_MONITOR_EN > 0)
	osres = xTaskCreate(monitorTSK, "monitorTSK", OSMONITOR_TSK_SZ_STACK, NULL, OSMONITOR_TSK_PRIO, NULL);
	assert(osres == pdTRUE);
	P_LOGI(logTag, "Started monitorTSK");
#endif

	selWindow(startupWindow);

	while(1){
		if(fp.tf.state.bit.lowInputVoltage != 0){
			shutdown();
		}

		if(selWindowPrev != fp.currentSelWindow){
			if(windowTskHandle != NULL){
				assert(osres == pdTRUE);	//Fail windowTskHandle
				P_LOGI(logTag, "Stopped %s", pcTaskGetName(windowTskHandle));
				vTaskDelete(windowTskHandle);
			}

			switch(fp.currentSelWindow){
				case noneWindow:
					break;
				case startupWindow:
					osres = xTaskCreate(startupTSK, "startupTSK", STARTUP_TSK_SZ_STACK, NULL, STARTUP_TSK_PRIO, &windowTskHandle);
					break;
				case settingWindow:
					osres = xTaskCreate(settingTSK, "settingTSK", SETT_TSK_SZ_STACK, NULL, SETT_TSK_PRIO, &windowTskHandle);
					break;
				case baseWindow:
					osres = xTaskCreate(baseTSK, "baseTSK", BASE_TSK_SZ_STACK, NULL, BASE_TSK_PRIO, &windowTskHandle);
					break;
				case chargerWindow:
					osres = xTaskCreate(chargeTSK, "chargeTSK", CHARG_TSK_SZ_STACK, NULL, CHARG_TSK_PRIO, &windowTskHandle);
					break;
				case cube3dWindow:
					osres = xTaskCreate(cube3dTSK, "cube3dTSK", CUBE_TSK_SZ_STACK, NULL, CUBE_TSK_PRIO, &windowTskHandle);
					break;
				case bubblesWindow:
					osres = xTaskCreate(bubblesTSK, "bubblesTSK", BUBLE_TSK_SZ_STACK, NULL, BUBLE_TSK_PRIO, &windowTskHandle);
					break;
				default:
					assert(!"Fail selector");
			}
			assert(osres == pdTRUE);	//Fail windowTskHandle
			P_LOGI(logTag, "Started %s", pcTaskGetName(windowTskHandle));
			selWindowPrev = fp.currentSelWindow;
		}

		/*
		 * Led blink
		 */
		static uint8_t ledCount = 0;
		if(ledCount++ == 100){
			LED_ON();
			ledCount = 0;
		}
		if(ledCount == 10){
			LED_OFF();
		}
		if((ledCount == 20) && (uartTsk.state == uartConnect)){
			LED_ON();
		}
		if(ledCount == 30){
			LED_OFF();
		}

		/*
		 * Link management
		 */
		static uint8_t linkCount = 0;
		if(linkCount++ == (LINK_DETECT_PERIOD / SYSTEM_TSK_PERIOD)){
			if(gppin_get(GP_LANnINT) == 0){	//Detect by GPIO
				ETH_ReadPHYRegister(1, PHY_BSR);
				if(fp.state.lanLink != 0){
					netif_set_link_down(&xnetif);
					netif_set_down(&xnetif);
					P_LOGI(logTag, "LAN link Down");
					fp.state.lanLink = 0;
				}
			}
			if(fp.state.lanLink == 0){		//Detect by read status register
				if(ETH_AutoNegotiation(1, NULL) == ETH_SUCCESS){
					netif_set_link_up(&xnetif);
					netif_set_up(&xnetif);	//When the netif is fully configured this function must be called
					P_LOGI(logTag, "LAN link Up");
					fp.state.lanLink = 1;
				}
			}

			linkCount = 0;
		}
		static uint32_t linkRequest = 0;
		if(linkRequest != httpServer.numberRequest){
			fp.state.lanActive = 1;
			linkRequest = httpServer.numberRequest;
		}

		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(SYSTEM_TSK_PERIOD));
	}
}

/*!****************************************************************************
 * @brief	Load parameters from memory
 */
void loadParameters(void){
	prm_state_type stat;

	stat = prm_load(SYSEEPADR, prmEepSys);
	if(stat == prm_ok){
		P_LOGI(logTag, "System settings load ok");
	}else{
		P_LOGE(logTag, "System settings load error: %u", stat);
		prm_loadDefault(prmEepSys);
		fp.state.sysSettingLoadDefault = 1;
	}

	stat = prm_load(USEREEPADR, prmEep);
	if(stat == prm_ok){
		P_LOGI(logTag, "User settings load ok");
	}else{
		P_LOGE(logTag, "User settings load error: %u", stat);
		prm_loadDefault(prmEep);
		fp.state.userSettingLoadDefault = 1;
	}
}

/*!****************************************************************************
 * @brief	Select window task & wait selected
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

	prm_state_type stat;
	stat = prm_store(USEREEPADR, prmEep);
	if(stat == prm_ok){
		P_LOGI(logTag, "System settings store ok");
	}else{
		P_LOGE(logTag, "System settings store error: %u", stat);
	}

	BeepTime(ui.beep.goodbye.time, ui.beep.goodbye.freq);
	LED_ON();
	if(windowTskHandle != NULL){
		vTaskDelete(windowTskHandle);	//Удаляем текущее окно
	}
	vTaskDelay(pdMS_TO_TICKS(10000));
	NVIC_SystemReset();
}

/*!****************************************************************************
 * @param ip:		 Internet Protocol address
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
	 * Set DNS server address
	 */
	ip_addr_t ipaddrs;
	IP4_ADDR(&ipaddrs, 8, 8, 8, 8);
	dns_setserver(0, &ipaddrs);
	IP4_ADDR(&ipaddrs, 1, 1, 1, 1);
	dns_setserver(1, &ipaddrs);

	/*
	 * Далее производится детектирование Link и Negotiation, перенастройка
	 * MAC под текущее физическое подключение, netif_set_up
	 */
}

/*!****************************************************************************
 */
void netSettingUpdate(void){
	ip_addr_t l_ipaddr;
	ip_addr_t l_netmask;
	ip_addr_t l_gateway;

	//With convert 32-bits host order to network order
	l_ipaddr.addr = htonl(fp.fpSet.ipadr);
	l_netmask.addr = htonl(fp.fpSet.netmask);
	l_gateway.addr = htonl(fp.fpSet.gateway);

	netif_set_addr(&xnetif, &l_ipaddr, &l_netmask, &l_gateway);
}

/*!****************************************************************************
 */
void timezoneUpdate(void){
	char str[12];
	snprintf(str, sizeof(str), "TZ=GMT%i", -fp.fpSet.timezone);
	putenv(str);
	tzset();
}

/******************************** END OF FILE ********************************/

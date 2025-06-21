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
#include <time.h>

#include <plog.h>
#include <prmSystem.h>
#include <sntp.h>
#include <stm32f4x7_eth_bsp.h>
#include <stm32f4x7_eth.h>
#include <ethernetif.h>
#include <lwip/tcpip.h>
#include <lwip/dns.h>
#include <display.h>
#include <beep.h>
#include <pvd.h>
#include <ledpwm.h>
#include <board.h>
#include <ui.h>
#include <startupTSK.h>
#include <settingTSK.h>
#include <chargeTSK.h>
#include <baseTSK.h>
#include <monitorTSK.h>
#include <24AAxx.h>
#include "regulatorConnTSK.h"
#include "systemTSK.h"
#include <write.h>
#include <httpServerTSK.h>
#include <modbusServerTSK.h>

/*!****************************************************************************
 * Memory
 */
frontPanel_type fp;					///< Data structure front panel
static TaskHandle_t windowTskHandle;	///< Program task handler
static struct netif xnetif; 			///< Network interface structure
static SemaphoreHandle_t lowPowerSem;

/*!****************************************************************************
 * Local prototypes for the functions
 */
static void loadParameters(void);
static void pvdCallback(void);
static void LwIP_Init(const uint32_t ipaddr, const uint32_t netmask, const uint32_t gateway);

extern "C" int _write(int fd, const void *buf, size_t count);

/**
 * SYS_DEBUG_LEVEL: Enable debugging for system task
 */
#define TASK_MONITOR_EN	0
#define LOG_LOCAL_LEVEL P_LOG_INFO
static const char *logTag = "SYS";

/*!****************************************************************************
 * @brief
 */
void systemTSK(void *pPrm){
	(void)pPrm;
	selWindow_type 	selWindowPrev = noneWindow;

	//Init log system
	plog_setWrite(_write);
	plog_setTimestamp([]() -> uint32_t { return xTaskGetTickCount(); });

	P_LOGI(logTag, "\n\nStarted systemTSK");

	loadParameters();												// Load panel settings and user parameters
	timezoneUpdate(Prm::timezone.val);
	pvd_setSupplyFaultCallBack(pvdCallback);						// Setup callback for Supply Fault
	disp_init();
	LwIP_Init(Prm::ipadr.val, Prm::netmask.val, Prm::gateway.val);	// Initialize the LwIP stack
	uint64_t mac = 0;
	memcpy(&mac, xnetif.hwaddr, xnetif.hwaddr_len);
	Prm::mac0.val = mac;
	sntp_init();													// Initialize service SNTP

	lowPowerSem = xSemaphoreCreateBinary();
	assert(lowPowerSem != NULL);

	assert(pdTRUE == xTaskCreate(regulatorConnTSK, "regulatorConnTSK", REG_TSK_SZ_STACK, NULL, REG_TSK_PRIO, NULL));
	P_LOGI(logTag, "Started regulatorConnTSK");

	assert(pdTRUE == xTaskCreate(modbusServerTSK, "modbusServerTSK", TCPMODBUS_TSK_SZ_STACK, NULL, TCPMODBUS_TSK_PRIO, NULL));
	P_LOGI(logTag, "Started httpServerTSK");

	assert(pdTRUE == xTaskCreate(httpServerTSK, "httpServerTSK", HTTP_TSK_SZ_STACK, NULL, HTTP_TSK_PRIO, NULL));
	P_LOGI(logTag, "Started httpServerTSK");

	vTaskDelay(1);
	P_LOGI(logTag, "Set regulator wire resistance");
	reg_setWireResistance(Prm::wirecompensateOnOff.val ? Prm::wireResistance.val : 0);

#if(TASK_MONITOR_EN > 0)
	osres = xTaskCreate(monitorTSK, "monitorTSK", OSMONITOR_TSK_SZ_STACK, NULL, OSMONITOR_TSK_PRIO, NULL);
	assert(osres == pdTRUE);
#endif

	selWindow(startupWindow);
	BaseType_t osres = pdTRUE;
	while(1){
		regState_t state;
		bool regulatorConnected = reg_getState(&state);

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
				default:
					osres = pdTRUE;
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
		if((ledCount == 20) && regulatorConnected){
			LED_ON();
		}
		if(ledCount == 30){
			LED_OFF();
		}

		static uint32_t linkRequest = 0;
		if(linkRequest != httpServer.numberRequest){
			fp.state.httpactiv = 1;
			linkRequest = httpServer.numberRequest;
		}

		BaseType_t res = xSemaphoreTake(lowPowerSem, pdMS_TO_TICKS(SYSTEM_TSK_PERIOD));

		if(Prm::reboot.val == Prm::mask_reboot::reboot_do){
			vTaskDelay(pdMS_TO_TICKS(2));
			NVIC_SystemReset();
		}

		if((regulatorConnected && state.status.m_lowInputVoltage) || res == pdTRUE){
			P_LOGD(logTag, "System saveparameters");
			saveparametersUser();
			BeepTime(ui.beep.shutdown.time, ui.beep.shutdown.freq);
			LED_ON();
			if(windowTskHandle != NULL){
				vTaskDelete(windowTskHandle);	//Delete window
			}
			vTaskDelay(pdMS_TO_TICKS(10000));
			NVIC_SystemReset();
		}
	}
}

/*!****************************************************************************
 * @brief	Load parameters from memory
 */
static void loadParameters(void){
	size_t size = Prm::getSerialSize(Prm::Save::savesys);
	if(size){
		uint8_t buffer[size];
		const uint16_t systemSettingsAddress = 0;
		if(eep_read(buffer, systemSettingsAddress, size) == eepOk){
			if(!Prm::deserialize(Prm::Save::savesys, buffer, size)){
				P_LOGW(logTag, "System settings load error");
				fp.state.sysSettingLoadDefault = 1;
			}
		}
	}

	size = Prm::getSerialSize(Prm::Save::saveuse);
	if(size){
		uint8_t buffer[size];
		const uint16_t userSettingsAddress = 512;
		if(eep_read(buffer, userSettingsAddress, size) == eepOk){
			if(!Prm::deserialize(Prm::Save::saveuse, buffer, size)){
				P_LOGW(logTag, "User settings load error");
				fp.state.userSettingLoadDefault = 1;
			}
		}
	}
}

/*!****************************************************************************
 * @brief	Save parameters to memory
 */
void saveparametersSystem(void){
	const uint16_t systemSettingsAddress = 0;
	size_t size = Prm::getSerialSize(Prm::Save::savesys);
	if(size){
		uint8_t buffer[size];
		Prm::serialize(Prm::Save::savesys, buffer);
		eep_write(systemSettingsAddress, buffer, size);
	}
}

/*!****************************************************************************
 * @brief	Save parameters to memory
 */
void saveparametersUser(void){
	const uint16_t userSettingsAddress = 512;
	size_t size = Prm::getSerialSize(Prm::Save::saveuse);
	if(size){
		uint8_t buffer[size];
		Prm::serialize(Prm::Save::saveuse, buffer);
		eep_write(userSettingsAddress, buffer, size);
	}
}

/*!****************************************************************************
 * Shutdown
 */
static void pvdCallback(void){
	setLcdBrightness(0);
	LED_OFF();
	ETH_BSP_Deinit();

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(lowPowerSem, &xHigherPriorityTaskWoken);
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
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
	P_LOGI(logTag, "set IP: %s", ipaddr_ntoa(&l_ipaddr) );
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

	auto linkcallback = [](struct netif *netif){
		fp.state.lanLink = netif_is_link_up(netif) ? 1 : 0;
		P_LOGI(logTag, "LAN link %s", fp.state.lanLink ? "Up" : "Down");
	};
	netif_set_link_callback(&xnetif, linkcallback);
	netif_set_up(&xnetif);
}

/*!****************************************************************************
 */
void netSettingUpdate(void){
	ip_addr_t l_ipaddr = { htonl(Prm::ipadr.val) };
	ip_addr_t l_netmask = { htonl(Prm::netmask.val) };
	ip_addr_t l_gateway = { htonl(Prm::gateway.val) };
	P_LOGI(logTag, "update IP: %s", ipaddr_ntoa(&l_ipaddr) );
	netif_set_addr(&xnetif, &l_ipaddr, &l_netmask, &l_gateway);
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
 */
void timezoneUpdate(int8_t timezone){
	char str[100];
	if(Prm::dstOnOff.val){
		snprintf(str, sizeof(str), "TZ=CEST%iCET%i,M%u.%u.%u/%02u:%02u:%02u,M%u.%u.%u/%02u:%02u:%02u",
				//									| month
				//										| Week
				//											| Day
				//												| Time
				timezone, timezone - 1,
				Prm::DSTSMon.val,
				Prm::DSTSWeek.val,
				Prm::DSTSDay.val,
				Prm::DSTSHour.val,
				Prm::DSTSMin.val,
				Prm::DSTSSec.val,
				Prm::DSTEMon.val,
				Prm::DSTEWeek.val,
				Prm::DSTEDay.val,
				Prm::DSTEHour.val,
				Prm::DSTEMin.val,
				Prm::DSTESec.val
				);
	}else{
		snprintf(str, sizeof(str), "TZ=GMT%i", timezone);
	}
	putenv(str);
	tzset();
}

/*!****************************************************************************
 * @brief Init operating system
 */
void OSinit(void){
	BaseType_t Result = pdTRUE;

	Result &= xTaskCreate(systemTSK, "systemTSK", SYSTEM_TSK_SZ_STACK, NULL, SYSTEM_TSK_PRIO, NULL);
	assert(Result == pdTRUE);
	vTaskStartScheduler();
}

/******************************** END OF FILE ********************************/

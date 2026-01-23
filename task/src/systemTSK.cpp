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
#include <hal/stm32f4x7_eth_bsp.h>
#include <hal/stm32f4x7_eth.h>
#include <hal/beep.h>
#include <hal/pvd.h>
#include <hal/board.h>
#include <hal/drivers.h>
#include <hal/ledpwm.h>
#include <driver/i2c.h>
#include <ethernetif.h>
#include <lwip/tcpip.h>
#include <lwip/dns.h>
#include <lwip/dhcp.h>
#include <dev/st7735.h>
#include <dev/24AAxx.h>
#include <display.h>
#include <ui.h>
#include <write.h>
#include <startupTSK.h>
#include <settingTSK.h>
#include <chargeTSK.h>
#include <baseTSK.h>
#include <httpServerTSK.h>
#include <modbusServerTSK.h>
#include "regulatorConnTSK.h"
#include "systemTSK.h"
#include "monitorTSK.h"

/*!****************************************************************************
 * Memory
 */
frontPanel_type fp;						///< Data structure front panel
static TaskHandle_t windowTskHandle;	///< Program task handler
static struct netif xnetif; 			///< Network interface structure
static SemaphoreHandle_t lowPowerSem;
static Eep24AA eeprom;

/*!****************************************************************************
 * Local prototypes for the functions
 */
static void loadParameters(void);
static void pvdCallback(void);
static void LwIP_Init(void);

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

	// Init log system
	plog_setWrite(_write);
	plog_setTimestamp([]() -> uint32_t { return xTaskGetTickCount(); });

	P_LOGI(logTag, "\n\nStarted systemTSK");

	fp.state.mainOscillatorError = gClockState == clockOk;
	fp.state.rtcOscillatorError = gRtcOscillatorInit;

	i2c(1).init();
	eeprom.setI2c(&i2c(1));
	loadParameters();												// Load panel settings and user parameters
	timezoneUpdate(Prm::timezone.val);
	pvd_setSupplyFaultCallBack(pvdCallback);						// Setup callback for Supply Fault
	st7735_init(JD_T18003, ST7735_R270);
	Disp disp = Disp(&st7735_driver);
	disp.init();
	LwIP_Init();	// Initialize the LwIP stack
	uint64_t mac = 0;
	memcpy(&mac, xnetif.hwaddr, xnetif.hwaddr_len);
	Prm::mac0.val = mac;
	sntp_init();													// Initialize service SNTP

	lowPowerSem = xSemaphoreCreateBinary();
	assert(lowPowerSem != NULL);

	BaseType_t osres = xTaskCreate(regulatorConnTSK, "regulatorConnTSK", REG_TSK_SZ_STACK, NULL, REG_TSK_PRIO, NULL);
	assert(osres == pdTRUE);
	P_LOGI(logTag, "Started regulatorConnTSK");

	osres = xTaskCreate(modbusServerTSK, "modbusServerTSK", TCPMODBUS_TSK_SZ_STACK, NULL, TCPMODBUS_TSK_PRIO, NULL);
	assert(osres == pdTRUE);
	P_LOGI(logTag, "Started modbusServerTSK");

	osres = xTaskCreate(httpServerTSK, "httpServerTSK", HTTP_TSK_SZ_STACK, NULL, HTTP_TSK_PRIO, NULL);
	assert(osres == pdTRUE);
	P_LOGI(logTag, "Started httpServerTSK");

	vTaskDelay(pdMS_TO_TICKS(2));

	// Regulator initialization
	bool regres = reg_wireResistanceSet(Prm::wirecompensateOnOff.val ? Prm::wireResistance.val : 0);
	if(!regres){
		P_LOGE(logTag, "Fail set regulator wire resistance");
	}
	regres = reg_crangeSet(Prm::crange_set.val ? reg_crange_auto : reg_crange_hi);
	if(!regres){
		P_LOGE(logTag, "Fail set regulator crangeSet");
	}
	regres = reg_vFilterSizeSet(Prm::vfilter.val);
	regres = regres & reg_iFilterSizeSet(Prm::ifilter.val);
	regres = regres & reg_vIntegrationSizeSet(Prm::vintegration.val);
	regres = regres & reg_iIntegrationSizeSet(Prm::iintegration.val);
	if(!regres){
		P_LOGE(logTag, "Fail set regulator acquisition settings");
	}

#if(TASK_MONITOR_EN > 0)
	osres = xTaskCreate(monitorTSK, "monitorTSK", OSMONITOR_TSK_SZ_STACK, NULL, OSMONITOR_TSK_PRIO, NULL);
	assert(osres == pdTRUE);
#endif

	selWindow(startupWindow);
	osres = pdTRUE;
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
					osres = xTaskCreate(startupTSK, "startupTSK", STARTUP_TSK_SZ_STACK, &disp, STARTUP_TSK_PRIO, &windowTskHandle);
					break;
				case settingWindow:
					osres = xTaskCreate(settingTSK, "settingTSK", SETT_TSK_SZ_STACK, &disp, SETT_TSK_PRIO, &windowTskHandle);
					break;
				case baseWindow:
					osres = xTaskCreate(baseTSK, "baseTSK", BASE_TSK_SZ_STACK, &disp, BASE_TSK_PRIO, &windowTskHandle);
					break;
				case chargerWindow:
					osres = xTaskCreate(chargeTSK, "chargeTSK", CHARG_TSK_SZ_STACK, &disp, CHARG_TSK_PRIO, &windowTskHandle);
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
			BeepTime(Prm::bpWelcomeOnOff ? ui.beep.shutdown.time : 0, ui.beep.shutdown.freq);
			LED_ON();
			setLcdBrightness(0);
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
		if(eeprom.read(buffer, systemSettingsAddress, size)){
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
		if(eeprom.read(buffer, userSettingsAddress, size)){
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
		eeprom.write(systemSettingsAddress, buffer, size);
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
		eeprom.write(userSettingsAddress, buffer, size);
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
 *
 */
void dhcp_set_ntp_servers(u8_t num_ntp_servers, const ip4_addr_t* ntp_server_addrs){
	for(uint8_t i = 0; i < num_ntp_servers; i++){
		P_LOGI(logTag, "set NTP[%u]: %s", i, ipaddr_ntoa(&ntp_server_addrs[i]));
	}
}

/*!****************************************************************************
 *
 */
void netif_status_callback(struct netif *nif) {
	if(netif_is_up(nif) && !ip4_addr_isany(&nif->ip_addr)){
		char cip[16], cnm[16], cgw[16];
		ip4addr_ntoa_r(&nif->ip_addr, cip, sizeof(cip));
		ip4addr_ntoa_r(&nif->netmask, cnm, sizeof(cnm));
		ip4addr_ntoa_r(&nif->gw, cgw, sizeof(cgw));
		P_LOGI(logTag, "DHCP supplied address\n"
				"\tIP: %s\n"
				"\tNM: %s\n"
				"\tGW: %s",
				cip, cnm, cgw
				);
		Prm::currIpadr.val = ntohl(netif_ip_addr4(&xnetif)->addr);
		Prm::currNetmask.val = ntohl(netif_ip_netmask4(&xnetif)->addr);
		Prm::currGateway.val = ntohl(netif_ip_gw4(&xnetif)->addr);
	}else if(!netif_is_up(nif)) {
		P_LOGI(logTag, "Network Interface is DOWN");
	}
}

/*!****************************************************************************
 * @param ip:		 Internet Protocol address
 * @param netmask:
 * @param gateway
 */
void LwIP_Init(void){
	/* Create tcp_ip stack thread */
	tcpip_init(NULL, NULL);
	ip_addr_t l_ipaddr = { .addr = htonl(Prm::dhcpOnOff.val ? IPADDR_ANY : Prm::ipadr.val) };
	ip_addr_t l_netmask = { .addr = htonl(Prm::dhcpOnOff.val ? IPADDR_ANY : Prm::netmask.val) };
	ip_addr_t l_gateway = { .addr = htonl(Prm::dhcpOnOff.val ? IPADDR_ANY : Prm::gateway.val) };
	ip_addr_t l_dns = { .addr = htonl(Prm::dhcpOnOff.val ? IPADDR_ANY : Prm::dnsServip.val) };
	netif_add(&xnetif, &l_ipaddr, &l_netmask, &l_gateway, NULL, &ethernetif_init, &tcpip_input); // Adds network interface to the netif_list
	netif_set_default(&xnetif); // Registers the default network interface
	if(!Prm::dhcpOnOff.val){
		P_LOGI(logTag, "set static IP: %s", ipaddr_ntoa(&l_ipaddr));
		dns_setserver(0, &l_dns); // Set DNS server address
	}

	// Link UP callback
	auto linkcallback = [](struct netif *netif){
		fp.state.lanLink = netif_is_link_up(netif) ? 1 : 0;
		P_LOGI(logTag, "LAN link %s", fp.state.lanLink ? "Up" : "Down");
		if(!fp.state.lanLink){
			ip_addr_t ipaddr = { .addr = htonl(IPADDR_ANY) };
			ip_addr_t netmask = { .addr = htonl(IPADDR_NONE) };
			ip_addr_t gateway  = { .addr = htonl(IPADDR_ANY) };
			netif_set_addr(&xnetif, &ipaddr, &netmask, &gateway);
			Prm::currIpadr.val = 0;
			Prm::currNetmask.val = 0;
			Prm::currGateway.val = 0;
		}
	};

	netif_set_link_callback(&xnetif, linkcallback);
	netif_set_up(&xnetif);

	dhcp_network_changed_link_up(&xnetif);
	P_LOGI(logTag, "DHCP start");
	err_t err = dhcp_start(&xnetif);
	if(err != ERR_OK){
		P_LOGW(logTag, "Error DHCP start");
	}

	netif_set_status_callback(&xnetif, netif_status_callback);
}

/*!****************************************************************************
 */
void netSettingUpdate(void){
	if(!Prm::dhcpOnOff.val){
		ip_addr_t l_ipaddr = { htonl(Prm::ipadr.val) };
		ip_addr_t l_netmask = { htonl(Prm::netmask.val) };
		ip_addr_t l_gateway = { htonl(Prm::gateway.val) };
		P_LOGI(logTag, "update IP: %s", ipaddr_ntoa(&l_ipaddr) );
		netif_set_addr(&xnetif, &l_ipaddr, &l_netmask, &l_gateway);
		ip_addr_t l_dns = { htonl(Prm::dnsServip.val) };
		dns_setserver(0, &l_dns); // Set DNS server address
		dhcp_inform(&xnetif);
	}else{
		err_t err = dhcp_renew(&xnetif);
		if(err != ERR_OK){
			P_LOGW(logTag, "Error DHCP renew");
		}
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

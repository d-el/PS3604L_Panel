#include <task/inc/httpServerTSK.h>
#include <task/inc/tcpEchoServerTSK.h>
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stm32f4x7_eth_bsp.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/dhcp.h"
#include "ethernetif.h"
#include "main.h"
#include "tcpip.h"
#include "netif.h"
#include <stdio.h>
#include "gpio.h"

struct netif xnetif; /* network interface structure */

void LwIP_Init(void){
	ip_addr_t ipaddr;
	ip_addr_t netmask;
	ip_addr_t gw;

	/* Create tcp_ip stack thread */
	tcpip_init( NULL, NULL);

	/* IP address setting & display on STM32_evalboard LCD*/
#ifdef USE_DHCP
	ipaddr.addr = 0;
	netmask.addr = 0;
	gw.addr = 0;
#else
	IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
	IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
	IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
#endif

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

	netif_add(&xnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);

	/*  Registers the default network interface. */
	netif_set_default(&xnetif);

	/*  When the netif is fully configured this function must be called.*/
	netif_set_up(&xnetif);
}

/*!****************************************************************************
 *
 */
void main(void){
	hardInit();
	/* Initilaize the LwIP stack */
	LwIP_Init();
	//vTaskStartScheduler();
	OSinit();
	while(1);
}

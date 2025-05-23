/*******************************************************************************
  * @file		lwipopts.h
  * @author	 	Storozhenko Roman
  * @version	V1.0
  * @date		22.04.2018
  * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
  * @brief		lwIP Options Configuration.
  *				This file is based on Utilities\lwip_v1.3.2\src\include\lwip\opt.h
  *				and contains the lwIP configuration for the STM32F4x7 demonstration.
  */

#ifndef LWIPOPTS_L
#define LWIPOPTS_H

#include "FreeRTOSConfig.h"
#include <sys/errno.h>


/*-------------------------------------
   -------------- NO SYS --------------
   ------------------------------------
*/

/**
 * NO_SYS==1: Provides VERY minimal functionality. Otherwise,
 * use lwIP facilities.
 */
#define NO_SYS					0

/*-------------------------------------
   ----------- Core locking -----------
   ------------------------------------
*/

/**
 * LWIP_TCPIP_CORE_LOCKING_INPUT: when LWIP_TCPIP_CORE_LOCKING is enabled,
 * this lets tcpip_input() grab the mutex for input packets as well,
 * instead of allocating a message and passing it to tcpip_thread.
 *
 * ATTENTION: this does not work when tcpip_input() is called from
 * interrupt context!
 */
#define LWIP_TCPIP_CORE_LOCKING_INPUT	1

/**
 * SYS_LIGHTWEIGHT_PROT==1: if you want inter-task protection for certain
 * critical regions during buffer allocation, deallocation and memory
 * allocation and deallocation.
 */
#define SYS_LIGHTWEIGHT_PROT	0

/*-------------------------------------
   ---------- Memory options ----------
   ------------------------------------
*/

/**
 * Memory options
 * MEM_ALIGNMENT: should be set to the alignment of the CPU for which
 * lwIP is compiled. 4 byte alignment -> define MEM_ALIGNMENT to 4, 2
 *	byte alignment -> define MEM_ALIGNMENT to 2
 */
#define MEM_ALIGNMENT			4


/**
 * Use Glibc malloc()/free()
 */
#define MEM_LIBC_MALLOC			1

/**
 * MEM_SIZE: the size of the heap memory. If the application will send
 * a lot of data that needs to be copied, this should be set high
 */
#define MEM_SIZE				1024

/*-------------------------------------------------
   ---------- Internal Memory Pool Sizes ----------
   ------------------------------------------------*/

/**
 * MEMP_NUM_PBUF: the number of memp struct pbufs. If the application
 * sends a lot of data out of ROM (or other static memory), this
 * should be set high
 */
#define MEMP_NUM_PBUF			16

/**
 * MEMP_NUM_RAW_PCB: Number of raw connection PCBs
 * (requires the LWIP_RAW option)
 */
#define MEMP_NUM_RAW_PCB		1

/**
 * MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One
 * per active UDP "connection"
 */
#define MEMP_NUM_UDP_PCB		4

/**
 * MEMP_NUM_TCP_PCB: the number of simulatenously active TCP connections
 */
#define MEMP_NUM_TCP_PCB		8

/**
 *	MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP connections
 */
#define MEMP_NUM_TCP_PCB_LISTEN 3

/**
 * MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP segments
 */
#define MEMP_NUM_TCP_SEG		6

/**
 * MEMP_NUM_SYS_TIMEOUT: the number of simulateously active timeouts
 */
#define MEMP_NUM_SYS_TIMEOUT	16

/**
 * MEMP_NUM_NETBUF: the number of struct netbufs.
 * (only needed if you use the sequential API, like api_lib.c)
 */
#define MEMP_NUM_NETBUF			4

/**
 * MEMP_NUM_NETCONN: the number of struct netconns.
 * (only needed if you use the sequential API, like api_lib.c)
 */
#define MEMP_NUM_NETCONN		4

/**
 * MEMP_NUM_TCPIP_MSG_API: the number of struct tcpip_msg, which are used
 * for callback/timeout API communication.
 * (only needed if you use tcpip.c)
 */
#define MEMP_NUM_TCPIP_MSG_API			2

/**
 * MEMP_NUM_TCPIP_MSG_INPKT: the number of struct tcpip_msg, which are used
 * for incoming packets.
 * (only needed if you use tcpip.c)
 */
#define MEMP_NUM_TCPIP_MSG_INPKT		6

/**
 * Pbuf options
 * PBUF_POOL_SIZE: the number of buffers in the pbuf pool
 */
#define PBUF_POOL_SIZE			16

/**--------------------------------
   ---------- IP options ----------
   --------------------------------*/

/**
 * IP_REASSEMBLY==1: Reassemble incoming fragmented IP packets. Note that
 * this option does not affect outgoing packet sizes, which can be controlled
 * via IP_FRAG.
 */
#define IP_REASSEMBLY			0

/**
 * IP_FRAG==1: Fragment outgoing IP packets if their size exceeds MTU. Note
 * that this option does not affect incoming packet sizes, which can be
 * controlled via IP_REASSEMBLY.
 */
#define IP_FRAG					0

/**
 * IP_DEFAULT_TTL: Default value for Time-To-Live used by transport layers.
 */
#define IP_DEFAULT_TTL					255

/**----------------------------------
   ---------- ICMP options ----------
   ----------------------------------*/

/**
 * ICMP options
 */
#define LWIP_ICMP				1

/**
 * LWIP_BROADCAST_PING==1: respond to broadcast pings (default is unicast only)
 */
#define LWIP_BROADCAST_PING				0

/**
 * LWIP_MULTICAST_PING==1: respond to multicast pings (default is unicast only)
 */
#define LWIP_MULTICAST_PING				0

/**---------------------------------
   ---------- RAW options ----------
   ---------------------------------*/

/**
 * LWIP_RAW==1: Enable application layer to hook into the IP layer itself.
 */
#define LWIP_RAW						1

/**----------------------------------
   ---------- DHCP options ----------
   ----------------------------------*/

/**
 * DHCP options
 * Define LWIP_DHCP to 1 if you want DHCP configuration of
 * interfaces. DHCP is not implemented in lwIP 0.5.1, however, so
 * turning this on does currently not work.
 */
#define LWIP_DHCP					1

/**
 * LWIP_DHCP_DOES_ACD_CHECK==1: Perform address conflict detection on the dhcp address.
 */
#define LWIP_DHCP_DOES_ACD_CHECK	0

/**
 * LWIP_DHCP_GETS_NTP==1: Request NTP servers with discover/select. For each
 * response packet, an callback is called, which has to be provided by the port:
 * void dhcp_set_ntp_servers(u8_t num_ntp_servers, ip_addr_t* ntp_server_addrs);
*/
#define LWIP_DHCP_GET_NTP_SRV		0

/**----------------------------------
   ---------- DNS options -----------
   ----------------------------------*/

/**
 * LWIP_DNS==1: Turn on DNS module. UDP must be available for DNS
 * transport.
 */
#define LWIP_DNS				1

/**---------------------------------
   ---------- UDP options ----------
   ---------------------------------*/

/**
 * UDP options
 */
#define LWIP_UDP				1

/**---------------------------------
   ---------- TCP options ----------
   ---------------------------------*/

/**
 * TCP options
 */
#define LWIP_TCP				1

/**
 * TCP receive window
 */
#define TCP_WND					(2 * TCP_MSS)

/**
 * Controls if TCP should queue segments that arrive out of
 * order. Define to 0 if your device is low on memory
 */
#define TCP_QUEUE_OOSEQ			0

/**
 * TCP Maximum segment size
 */
#define TCP_MSS					(1500 - 40)	  /* TCP_MSS = (Ethernet MTU - IP header size - TCP header size) */

/**
 * TCP sender buffer space (bytes)
 */
#define TCP_SND_BUF				(4 * TCP_MSS)

/**
 * TCP_SND_QUEUELEN: TCP sender buffer space (pbufs). This must be at least
 * as much as (2 * TCP_SND_BUF/TCP_MSS) for things to work
 */
#define TCP_SND_QUEUELEN		(4 * TCP_SND_BUF/TCP_MSS)

#define LWIP_DISABLE_TCP_SANITY_CHECKS 1

/**----------------------------------
   ---------- Pbuf options ----------
   ----------------------------------*/

/**
 * PBUF_POOL_BUFSIZE: the size of each pbuf in the pbuf pool
 */
#define PBUF_POOL_BUFSIZE		256


/*-------------------------------------
   ---------- Thread options ----------
   ------------------------------------
*/

/**
 * TCPIP_THREAD_STACKSIZE: The stack size used by the main tcpip thread.
 * The stack size value itself is platform-dependent, but is passed to
 * sys_thread_new() when the thread is created.
 */
#define TCPIP_THREAD_STACKSIZE			TCPIP_SZ_STACK

/**
 * TCPIP_THREAD_PRIO: The priority assigned to the main tcpip thread.
 * The priority value itself is platform-dependent, but is passed to
 * sys_thread_new() when the thread is created.
 */
#define TCPIP_THREAD_PRIO				TCPIP_TSK_PRIO

/**
 *
 */
#define netifINTERFACE_TASK_STACK_SIZE	NETIF_SZ_STACK
#define netifINTERFACE_TASK_PRIORITY	NETIF_TSK_PRIO

/**
 * TCPIP_MBOX_SIZE: The mailbox size for the tcpip thread messages
 * The queue size value itself is platform-dependent, but is passed to
 * sys_mbox_new() when tcpip_init is called.
 */
#define TCPIP_MBOX_SIZE					512

/**
 * DEFAULT_UDP_RECVMBOX_SIZE: The mailbox size for the incoming packets on a
 * NETCONN_UDP. The queue size value itself is platform-dependent, but is passed
 * to sys_mbox_new() when the recvmbox is created.
 */
#define DEFAULT_UDP_RECVMBOX_SIZE		512

/**
 * DEFAULT_TCP_RECVMBOX_SIZE: The mailbox size for the incoming packets on a
 * NETCONN_TCP. The queue size value itself is platform-dependent, but is passed
 * to sys_mbox_new() when the recvmbox is created.
 */
#define DEFAULT_TCP_RECVMBOX_SIZE		512

/**
 * DEFAULT_ACCEPTMBOX_SIZE: The mailbox size for the incoming connections.
 * The queue size value itself is platform-dependent, but is passed to
 * sys_mbox_new() when the acceptmbox is created.
 */
#define DEFAULT_ACCEPTMBOX_SIZE			512


/*-----------------------------------------------
   ---------- Sequential layer options ----------
   ----------------------------------------------*/

/**
 * Sequential layer options
 * LWIP_NETCONN==1: Enable Netconn API (require to use api_lib.c)
 */
#define LWIP_NETCONN					1

/**
 * LWIP_SO_RCVTIMEO==1: Enable receive timeout for sockets/netconns and
 * SO_RCVTIMEO processing.
 */
#define LWIP_SO_RCVTIMEO				1

/*-------------------------------------
   ---------- Socket options ----------
   ------------------------------------
*/

/**
 * LWIP_SOCKET==1: Enable Socket API (require to use sockets.c)
 */
#define LWIP_SOCKET						0

/*-----------------------------------------
   ---------- MQTT options ----------
   ----------------------------------------*/
#define LWIP_MQTT_APP					LWIP_TCP
#define MQTT_OUTPUT_RINGBUF_SIZE		512
#define MQTT_REQ_MAX_IN_FLIGHT			8


/*-----------------------------------------
   ---------- Statistics options ----------
   ----------------------------------------*/

/**
 * LWIP_STATS==1: Enable statistics collection in lwip_stats.
 */
#define LWIP_STATS						0

/**
 * LWIP_STATS_DISPLAY==1: Compile in the statistics output functions.
 */
#define LWIP_STATS_DISPLAY				0

/**
 * X_STATS==1: Enable link stats.
 */
#define LINK_STATS						0
#define ETHARP_STATS					0
#define IP_STATS						0
#define IPFRAG_STATS					0
#define ICMP_STATS						0
#define IGMP_STATS						0
#define UDP_STATS						0
#define TCP_STATS						0
#define MEM_STATS						0
#define MEMP_STATS						0
#define SYS_STATS						0
#define IP6_STATS						0
#define ICMP6_STATS						0
#define IP6_FRAG_STATS					0
#define MLD6_STATS						0
#define ND6_STATS						0
#define MIB2_STATS						0

/*----------------------------------------
   ---------- Debugging options ----------
   ---------------------------------------*/
#define LWIP_DEBUG						1

/**
 * LWIP_DBG_TYPES_ON: A mask that can be used to globally enable/disable
 * debug messages of certain types.
 * @see debugging_levels
 */
#define LWIP_DBG_TYPES_ON				LWIP_DBG_ON

/**
 * LWIP_DBG_MIN_LEVEL: After masking, the value of the debug is
 * compared against this value. If it is smaller, then debugging
 * messages are written.
 * @see debugging_levels
 */
#define LWIP_DBG_MIN_LEVEL				LWIP_DBG_LEVEL_ALL

/**
 * Enable debugging in X
 */
#define ETHARP_DEBUG					LWIP_DBG_OFF
#define NETIF_DEBUG						LWIP_DBG_OFF
#define PBUF_DEBUG						LWIP_DBG_OFF
#define API_LIB_DEBUG					LWIP_DBG_OFF
#define API_MSG_DEBUG					LWIP_DBG_OFF
#define SOCKETS_DEBUG					LWIP_DBG_OFF
#define ICMP_DEBUG						LWIP_DBG_OFF
#define IGMP_DEBUG						LWIP_DBG_OFF
#define INET_DEBUG						LWIP_DBG_OFF
#define IP_DEBUG						LWIP_DBG_OFF
#define IP_REASS_DEBUG					LWIP_DBG_OFF
#define RAW_DEBUG						LWIP_DBG_OFF
#define MEM_DEBUG						LWIP_DBG_OFF
#define MEMP_DEBUG						LWIP_DBG_OFF
#define SYS_DEBUG						LWIP_DBG_OFF
#define TIMERS_DEBUG					LWIP_DBG_OFF
#define TCP_DEBUG						LWIP_DBG_OFF
#define TCP_INPUT_DEBUG					LWIP_DBG_OFF
#define TCP_FR_DEBUG					LWIP_DBG_OFF
#define TCP_RTO_DEBUG					LWIP_DBG_OFF
#define TCP_CWND_DEBUG					LWIP_DBG_OFF
#define TCP_WND_DEBUG					LWIP_DBG_OFF
#define TCP_OUTPUT_DEBUG				LWIP_DBG_OFF
#define TCP_RST_DEBUG					LWIP_DBG_OFF
#define TCP_QLEN_DEBUG					LWIP_DBG_OFF
#define UDP_DEBUG						LWIP_DBG_OFF
#define TCPIP_DEBUG						LWIP_DBG_OFF
#define SLIP_DEBUG						LWIP_DBG_OFF
#define DHCP_DEBUG						LWIP_DBG_OFF
#define AUTOIP_DEBUG					LWIP_DBG_OFF
#define DNS_DEBUG						LWIP_DBG_OFF
#define IP6_DEBUG						LWIP_DBG_OFF
#define MQTT_DEBUG						LWIP_DBG_OFF

/**
 * LWIP_NETIF_LINK_CALLBACK==1: Support a callback function from an interface
 * whenever the link changes (i.e., link down)
 */
#define LWIP_NETIF_LINK_CALLBACK		1

/*---------------------------------------
   ---------- Checksum options ----------
   --------------------------------------*/

/**
 * Checksum options
 *
 * The STM32F4x7 allows computing and verifying the IP, UDP, TCP and ICMP checksums by hardware:
 * - To use this feature let the following define uncommented.
 * - To disable it and process by CPU comment the  the checksum.
 */

#define CHECKSUM_BY_HARDWARE

/**
 * CHECKSUM_GEN_X==1: Generate checksums in software for outgoing X packets.
 */
#define CHECKSUM_GEN_IP					0
#define CHECKSUM_GEN_UDP				0
#define CHECKSUM_GEN_TCP				0
#define CHECKSUM_GEN_ICMP				0
#define CHECKSUM_GEN_ICMP6				0
#define CHECKSUM_CHECK_IP				0
#define CHECKSUM_CHECK_UDP				0
#define CHECKSUM_CHECK_TCP				0
#define CHECKSUM_CHECK_ICMP				0
#define CHECKSUM_CHECK_ICMP6			0

#endif /*LWIPOPTS_H */
/******************************** END OF FILE ********************************/

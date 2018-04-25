/******************************************************************************
  * @file	 lwipopts.h
  * @author	 Storozhenko Roman
  * @version V1.0
  * @date	 22.04.2018
  * @brief	 lwIP Options Configuration.
  *			 This file is based on Utilities\lwip_v1.3.2\src\include\lwip\opt.h
  *			 and contains the lwIP configuration for the STM32F4x7 demonstration.
  */

#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

#include "OSinit.h"
#include <sys/errno.h>

/*!****************************************************************************
 * SYS_LIGHTWEIGHT_PROT==1: if you want inter-task protection for certain
 * critical regions during buffer allocation, deallocation and memory
 * allocation and deallocation.
 */
#define SYS_LIGHTWEIGHT_PROT	0
#define ETHARP_TRUST_IP_MAC		0
#define IP_REASSEMBLY			0
#define IP_FRAG					0
#define ARP_QUEUEING			0

/**
 * LWIP_TCPIP_CORE_LOCKING_INPUT: when LWIP_TCPIP_CORE_LOCKING is enabled,
 * this lets tcpip_input() grab the mutex for input packets as well,
 * instead of allocating a message and passing it to tcpip_thread.
 *
 * ATTENTION: this does not work when tcpip_input() is called from
 * interrupt context!
 */
#define LWIP_TCPIP_CORE_LOCKING_INPUT	1

/*!****************************************************************************
 * NO_SYS==1: Provides VERY minimal functionality. Otherwise,
 * use lwIP facilities.
 */
#define NO_SYS					0

/*!****************************************************************************
 * Memory options
 * MEM_ALIGNMENT: should be set to the alignment of the CPU for which
 * lwIP is compiled. 4 byte alignment -> define MEM_ALIGNMENT to 4, 2
 *	byte alignment -> define MEM_ALIGNMENT to 2
 */
#define MEM_ALIGNMENT			4

/*!****************************************************************************
 * MEM_SIZE: the size of the heap memory. If the application will send
 * a lot of data that needs to be copied, this should be set high
 */
#define MEM_SIZE				1024

/*!****************************************************************************
 * MEMP_NUM_PBUF: the number of memp struct pbufs. If the application
 * sends a lot of data out of ROM (or other static memory), this
 * should be set high
 */
#define MEMP_NUM_PBUF			16

/**
 * MEMP_NUM_NETBUF: the number of struct netbufs.
 * (only needed if you use the sequential API, like api_lib.c)
 */
#define MEMP_NUM_NETBUF			1

/**
 * MEMP_NUM_NETCONN: the number of struct netconns.
 * (only needed if you use the sequential API, like api_lib.c)
 */
#define MEMP_NUM_NETCONN		6

/**
 * MEMP_NUM_RAW_PCB: Number of raw connection PCBs
 * (requires the LWIP_RAW option)
 */
#define MEMP_NUM_RAW_PCB		2

/*!****************************************************************************
 * MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One
 * per active UDP "connection"
 */
#define MEMP_NUM_UDP_PCB		2

/*!****************************************************************************
 * MEMP_NUM_TCP_PCB: the number of simulatenously active TCP connections
 */
#define MEMP_NUM_TCP_PCB		10

/*!****************************************************************************
 *	MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP connections
 */
#define MEMP_NUM_TCP_PCB_LISTEN 1

/*!****************************************************************************
 * MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP segments
 */
#define MEMP_NUM_TCP_SEG		16

/*!****************************************************************************
 * MEMP_NUM_SYS_TIMEOUT: the number of simulateously active timeouts
 */
#define MEMP_NUM_SYS_TIMEOUT	6

/*!****************************************************************************
 * Pbuf options
 * PBUF_POOL_SIZE: the number of buffers in the pbuf pool
 */
#define PBUF_POOL_SIZE			15

/*!****************************************************************************
 * PBUF_POOL_BUFSIZE: the size of each pbuf in the pbuf pool
 */
#define PBUF_POOL_BUFSIZE		256

/*!****************************************************************************
 * TCP options
 */
#define LWIP_TCP				1

/**
 * TCP_TTL: Default Time-To-Live value
 */
#define TCP_TTL					64

/*!****************************************************************************
 * Controls if TCP should queue segments that arrive out of
 * order. Define to 0 if your device is low on memory
 */
#define TCP_QUEUE_OOSEQ			0

/*!****************************************************************************
 * TCP Maximum segment size
 */
#define TCP_MSS					(1500 - 40)	  /* TCP_MSS = (Ethernet MTU - IP header size - TCP header size) */

/*!****************************************************************************
 * TCP sender buffer space (bytes)
 */
#define TCP_SND_BUF				(4 * TCP_MSS)

/*!****************************************************************************
 * TCP_SND_QUEUELEN: TCP sender buffer space (pbufs). This must be at least
 * as much as (2 * TCP_SND_BUF/TCP_MSS) for things to work
 */
#define TCP_SND_QUEUELEN		(4 * TCP_SND_BUF/TCP_MSS)

/*!****************************************************************************
 * TCP receive window
 */
#define TCP_WND					(2 * TCP_MSS)

/*!****************************************************************************
 * ICMP options
 */
#define LWIP_ICMP				1

/*!****************************************************************************
 * DHCP options
 * Define LWIP_DHCP to 1 if you want DHCP configuration of
 * interfaces. DHCP is not implemented in lwIP 0.5.1, however, so
 * turning this on does currently not work.
 */
#define LWIP_DHCP				0

/*!****************************************************************************
 * UDP options
 */
#define LWIP_UDP				1
#define UDP_TTL					64

/*!****************************************************************************
 * Checksum options
 *
 * The STM32F4x7 allows computing and verifying the IP, UDP, TCP and ICMP checksums by hardware:
 * - To use this feature let the following define uncommented.
 * - To disable it and process by CPU comment the  the checksum.
 */
#define CHECKSUM_BY_HARDWARE

#ifdef CHECKSUM_BY_HARDWARE
  /* CHECKSUM_GEN_IP==0: Generate checksums by hardware for outgoing IP packets.*/
  #define CHECKSUM_GEN_IP				  0
  /* CHECKSUM_GEN_UDP==0: Generate checksums by hardware for outgoing UDP packets.*/
  #define CHECKSUM_GEN_UDP				  0
  /* CHECKSUM_GEN_TCP==0: Generate checksums by hardware for outgoing TCP packets.*/
  #define CHECKSUM_GEN_TCP				  0
  /* CHECKSUM_CHECK_IP==0: Check checksums by hardware for incoming IP packets.*/
  #define CHECKSUM_CHECK_IP				  0
  /* CHECKSUM_CHECK_UDP==0: Check checksums by hardware for incoming UDP packets.*/
  #define CHECKSUM_CHECK_UDP			  0
  /* CHECKSUM_CHECK_TCP==0: Check checksums by hardware for incoming TCP packets.*/
  #define CHECKSUM_CHECK_TCP			  0
#else
  /* CHECKSUM_GEN_IP==1: Generate checksums in software for outgoing IP packets.*/
  #define CHECKSUM_GEN_IP				  1
  /* CHECKSUM_GEN_UDP==1: Generate checksums in software for outgoing UDP packets.*/
  #define CHECKSUM_GEN_UDP				  1
  /* CHECKSUM_GEN_TCP==1: Generate checksums in software for outgoing TCP packets.*/
  #define CHECKSUM_GEN_TCP				  1
  /* CHECKSUM_CHECK_IP==1: Check checksums in software for incoming IP packets.*/
  #define CHECKSUM_CHECK_IP				  1
  /* CHECKSUM_CHECK_UDP==1: Check checksums in software for incoming UDP packets.*/
  #define CHECKSUM_CHECK_UDP			  1
  /* CHECKSUM_CHECK_TCP==1: Check checksums in software for incoming TCP packets.*/
  #define CHECKSUM_CHECK_TCP			  1
#endif

/*!****************************************************************************
 * Sequential layer options
 * LWIP_NETCONN==1: Enable Netconn API (require to use api_lib.c)
 */
#define LWIP_NETCONN					1

/**
 * MEMP_NUM_TCPIP_MSG_API: the number of struct tcpip_msg, which are used
 * for callback/timeout API communication.
 * (only needed if you use tcpip.c)
 */
#define MEMP_NUM_TCPIP_MSG_API			1

/**
 * MEMP_NUM_TCPIP_MSG_INPKT: the number of struct tcpip_msg, which are used
 * for incoming packets.
 * (only needed if you use tcpip.c)
 */
#define MEMP_NUM_TCPIP_MSG_INPKT		6

/*!****************************************************************************
 * Socket options
 * LWIP_SOCKET==1: Enable Socket API (require to use sockets.c)
 */
#define LWIP_SOCKET						0

/**
 * LWIP_RAW==1: Enable application layer to hook into the IP layer itself.
 */
#define LWIP_RAW						1

/**
 * LWIP_DNS==1: Turn on DNS module. UDP must be available for DNS
 * transport.
 */
#define LWIP_DNS						1

/*!****************************************************************************
 * DEBUG options
 */
//#define LWIP_DEBUG					0
#define LWIP_DBG_TYPES_ON				LWIP_DBG_TRACE
#define LWIP_DBG_MIN_LEVEL				LWIP_DBG_LEVEL_ALL

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

/*!****************************************************************************
 * Statistics options
 */
#define LWIP_STATS_DISPLAY				0
#define LWIP_STATS						1
#define LINK_STATS						1
#define IP_STATS						1
#define IPFRAG_STATS					1
#define ICMP_STATS						1
#define IGMP_STATS						1
#define UDP_STATS						1
#define TCP_STATS						1
#define MEM_STATS						1
#define MEMP_STATS						1
#define SYS_STATS						1

/*!****************************************************************************
 * OS options
 */
#define TCPIP_MBOX_SIZE					512
#define DEFAULT_UDP_RECVMBOX_SIZE		512
#define DEFAULT_TCP_RECVMBOX_SIZE		512
#define DEFAULT_ACCEPTMBOX_SIZE			512

/// tcpip_thread
#define TCPIP_THREAD_STACKSIZE			TCPIP_SZ_STACK
#define TCPIP_THREAD_PRIO				TCPIP_TSK_PRIO

/// Eth_if
#define netifINTERFACE_TASK_STACK_SIZE	NETIF_SZ_STACK
#define netifINTERFACE_TASK_PRIORITY	NETIF_TSK_PRIO

#endif /* __LWIPOPTS_H__ */
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

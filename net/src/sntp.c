/*!****************************************************************************
 * @file		sntp.c
 * @author		Author: Simon Goldschmidt (lwIP raw API part), d_el - Storozhenko Roman
 * @version		V1.0
 * @date		24.09.2017
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		SNTP client module
 */

/*!****************************************************************************
 * Include
 */
#include <inttypes.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "plog.h"
#include "rtc.h"
#include "lwipopts.h"
#include "lwip/timeouts.h"
#include "lwip/udp.h"
#include "lwip/dns.h"
#include "lwip/ip_addr.h"
#include "lwip/pbuf.h"

/**
 * SNTP_DEBUG_LEVEL: Enable debugging for SNTP
 */
#define LOG_LOCAL_LEVEL P_LOG_INFO

/**
 * SNTP_SINGLESYNC: Enable single sync
 */
#define SNTP_SINGLESYNC				1	//1 - single, 0 - periodical

/** SNTP server port */
#ifndef SNTP_PORT
#define SNTP_PORT                   123
#endif

/** SNTP server address:
 * - as IPv4 address in "u32_t" format
 * - as a DNS name if SNTP_SERVER_DNS is set to 1
 * May contain multiple server names (e.g. "pool.ntp.org","second.time.server")
 */
#define SNTP_SERVER_ADDRESS         "1.pool.ntp.org", "2.pool.ntp.org", "3.pool.ntp.org", "second.time.server"

/** SNTP receive timeout - in milliseconds
 * Also used as retry timeout - this shouldn't be too low.
 * Default is 3 seconds.
 */
#define SNTP_RECV_TIMEOUT           5000

/** Default retry timeout (in milliseconds) if the response
 * received is invalid.
 * This is doubled with each retry until SNTP_RETRY_TIMEOUT_MAX is reached.
 */
#define SNTP_RETRY_TIMEOUT          5000

/** SNTP update delay - in milliseconds
 * Default is 1 hour.
 * SNTPv4 RFC 4330 enforces a minimum update time of 15 seconds!
 */
#define SNTP_UPDATE_DELAY           30000

#define SNTP_RECEIVE_TIME_SIZE      1

#define SNTP_ERR_KOD                1

/* SNTP protocol defines */
#define SNTP_MSG_LEN                48

#define SNTP_OFFSET_LI_VN_MODE      0
#define SNTP_LI_MASK                0xC0
#define SNTP_LI_NO_WARNING          0x00
#define SNTP_LI_LAST_MINUTE_61_SEC  0x01
#define SNTP_LI_LAST_MINUTE_59_SEC  0x02
#define SNTP_LI_ALARM_CONDITION     0x03 /* (clock not synchronized) */

#define SNTP_VERSION_MASK           0x38
#define SNTP_VERSION                (4/* NTP Version 4*/<<3)

#define SNTP_MODE_MASK              0x07
#define SNTP_MODE_CLIENT            0x03
#define SNTP_MODE_SERVER            0x04
#define SNTP_MODE_BROADCAST         0x05

#define SNTP_OFFSET_STRATUM         1
#define SNTP_STRATUM_KOD            0x00

#define SNTP_OFFSET_ORIGINATE_TIME  24
#define SNTP_OFFSET_RECEIVE_TIME    32
#define SNTP_OFFSET_TRANSMIT_TIME   40

/* number of seconds between 1900 and 1970 */
#define DIFF_SEC_1900_1970         (2208988800UL)

/**
 * SNTP packet format (without optional fields)
 * Timestamps are coded as 64 bits:
 * - 32 bits seconds since Jan 01, 1970, 00:00
 * - 32 bits seconds fraction (0-padded)
 * For future use, if the MSB in the seconds part is set, seconds are based
 * on Feb 07, 2036, 06:28:16.
 */
#ifdef PACK_STRUCT_USE_INCLUDES
#include "arch/bpstruct.h"
#endif
PACK_STRUCT_BEGIN
struct sntp_msg{
	PACK_STRUCT_FIELD(u8_t li_vn_mode);
	PACK_STRUCT_FIELD(u8_t stratum);
	PACK_STRUCT_FIELD(u8_t poll);
	PACK_STRUCT_FIELD(u8_t precision);
	PACK_STRUCT_FIELD(u32_t root_delay);
	PACK_STRUCT_FIELD(u32_t root_dispersion);
	PACK_STRUCT_FIELD(u32_t reference_identifier);
	PACK_STRUCT_FIELD(u32_t reference_timestamp[2]);
	PACK_STRUCT_FIELD(u32_t originate_timestamp[2]);
	PACK_STRUCT_FIELD(u32_t receive_timestamp[2]);
	PACK_STRUCT_FIELD(u32_t transmit_timestamp[2]);
}PACK_STRUCT_STRUCT;
PACK_STRUCT_END
#ifdef PACK_STRUCT_USE_INCLUDES
#include "arch/epstruct.h"
#endif

/*!****************************************************************************
 * MEMORY
 */
static char *logTag = "SNTP";

/* function prototypes */
static void sntp_request(void *arg);
/** The UDP pcb used by the SNTP client */
static struct udp_pcb* sntp_pcb;
/** Addresses of servers */
static char* sntp_server_addresses[] = { SNTP_SERVER_ADDRESS };
/** The currently used server (initialized to 0) */
static u8_t sntp_current_server;
static u8_t sntp_num_servers = sizeof(sntp_server_addresses) / sizeof(char*);

/**
 * SNTP processing of received timestamp
 */
static void sntp_process(u32_t *receive_timestamp){
	/* convert SNTP time (1900-based) to unix GMT time (1970-based)
	 * @todo: if MSB is 1, SNTP time is 2036-based!
	 */
	time_t t = ntohl(receive_timestamp[0]) - DIFF_SEC_1900_1970;
	P_LOGI(logTag, "sntp_recv: set time: unix GMT time (1970-based) %"PRIu32"", (uint32_t)t);
	rtc_setTimeUnix(t);
}

/**
 * Initialize request struct to be sent to server.
 */
static void sntp_initialize_request(struct sntp_msg *req){
	memset(req, 0, SNTP_MSG_LEN);
	req->li_vn_mode = SNTP_LI_NO_WARNING | SNTP_VERSION | SNTP_MODE_CLIENT;
}

/**
 * If Kiss-of-Death is received (or another packet parsing error),
 * try the next server or retry the current server and increase the retry
 * timeout if only one server is available.
 *
 * @param arg is unused (only necessary to conform to sys_timeout)
 */
static void sntp_try_next_server(void* arg){
	LWIP_UNUSED_ARG(arg);

	if(sntp_num_servers > 1){
		/* new server: reset retry timeout */
		sntp_current_server++;
		if(sntp_current_server >= sntp_num_servers){
			sntp_current_server = 0;
		}

		P_LOGD(logTag, "sntp_try_next_server: Sending request to server %"U16_F"",
						(u16_t)sntp_current_server);
		/* instantly send a request to the next server */
		sntp_request(NULL);
	}else{
		sys_timeout(SNTP_RETRY_TIMEOUT, sntp_request, NULL);
	}
}

/** UDP recv callback for the sntp pcb */
static void sntp_recv(void *arg, struct udp_pcb* pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port){
	(void)addr;
	u8_t mode;
	u8_t stratum;
	u32_t receive_timestamp[SNTP_RECEIVE_TIME_SIZE];
	err_t err;

	LWIP_UNUSED_ARG(arg);
	LWIP_UNUSED_ARG(pcb);

	/* packet received: stop retry timeout  */
	sys_untimeout(sntp_try_next_server, NULL);
	sys_untimeout(sntp_request, NULL);

	err = ERR_ARG;
	/* check server address and port */
	if(port == SNTP_PORT){
		/* process the response */
		if(p->tot_len == SNTP_MSG_LEN){
			pbuf_copy_partial(p, &mode, 1, SNTP_OFFSET_LI_VN_MODE);
			mode &= SNTP_MODE_MASK;
			/* if this is a SNTP response... */
			if((mode == SNTP_MODE_SERVER) || (mode == SNTP_MODE_BROADCAST)){
				pbuf_copy_partial(p, &stratum, 1, SNTP_OFFSET_STRATUM);
				if(stratum == SNTP_STRATUM_KOD){
					/* Kiss-of-death packet. Use another server or increase UPDATE_DELAY. */
					err = SNTP_ERR_KOD;
					P_LOGD(logTag, "sntp_recv: Received Kiss-of-Death");
				}else{
					/* correct answer */
					err = ERR_OK;
					pbuf_copy_partial(p, &receive_timestamp, SNTP_RECEIVE_TIME_SIZE * 4, SNTP_OFFSET_RECEIVE_TIME);
				}
			}else{
				P_LOGD(logTag, "sntp_recv: Invalid mode in response: %"U16_F"", (u16_t)mode);
			}
		}else{
			P_LOGD(logTag, "sntp_recv: Invalid packet length: %"U16_F"", p->tot_len);
		}
	}
	pbuf_free(p);
	if(err == ERR_OK){
		P_LOGI(logTag, "sntp_recv: receive success");
		sntp_process(receive_timestamp);
	if(SNTP_SINGLESYNC > 0)
		udp_remove(pcb);
	else{
		/* Set up timeout for next request */
		sys_timeout((u32_t) SNTP_UPDATE_DELAY, sntp_request, NULL);
		P_LOGD(logTag, "sntp_recv: Scheduled next time request: %"PRIu32" ms", (u32_t)SNTP_UPDATE_DELAY);
	}
	}else if(err == SNTP_ERR_KOD){
		/* Kiss-of-death packet. Use another server or increase UPDATE_DELAY. */
		sntp_try_next_server(NULL);
	}else{
		/* another error, try the same server again */
		sys_timeout(SNTP_RETRY_TIMEOUT, sntp_request, NULL);
	}
}

/** Actually send an sntp request to a server.
 *
 * @param server_addr resolved IP address of the SNTP server
 */
static void sntp_send_request(const ip_addr_t *server_addr){
	struct pbuf* p;
	p = pbuf_alloc(PBUF_TRANSPORT, SNTP_MSG_LEN, PBUF_RAM);
	if(p != NULL){
		struct sntp_msg *sntpmsg = (struct sntp_msg *) p->payload;
		P_LOGD(logTag, "sntp_send_request: Sending request to server");
		/* initialize request message */
		sntp_initialize_request(sntpmsg);
		/* send request */
		udp_sendto(sntp_pcb, p, server_addr, SNTP_PORT);
		/* set up receive timeout: try next server or retry on timeout */
		sys_timeout((u32_t) SNTP_RECV_TIMEOUT, sntp_try_next_server, NULL);
	}else{
		P_LOGD(logTag, "sntp_send_request: Out of memory, trying again in %"PRIu16" ms", SNTP_RETRY_TIMEOUT);
		/* out of memory: set up a timer to send a retry */
		sys_timeout(SNTP_RETRY_TIMEOUT, sntp_request, NULL);
	}
}

/**
 * DNS found callback when using DNS names as server address.
 */
static void sntp_dns_found(const char* hostname, const ip_addr_t *ipaddr, void *arg){
	LWIP_UNUSED_ARG(hostname);
	LWIP_UNUSED_ARG(arg);

	if(ipaddr != NULL){
		/* Address resolved, send request */
		P_LOGD(logTag, "sntp_dns_found: Server address resolved, sending request");
		P_LOGI(logTag, "sntp_server_address: %s", ipaddr_ntoa(ipaddr));
		sntp_send_request(ipaddr);
	}else{
		/* DNS resolving failed -> try another server */
		P_LOGD(logTag, "sntp_dns_found: Failed to resolve server address resolved, trying next server");
		sntp_try_next_server(NULL);
	}
}

/**
 * Send out an sntp request via raw API.
 *
 * @param arg is unused (only necessary to conform to sys_timeout)
 */
static void sntp_request(void *arg){
	ip_addr_t sntp_server_address;
	err_t err;

	LWIP_UNUSED_ARG(arg);

	/* initialize SNTP server address */
	P_LOGI(logTag, "dns_gethostbyname, number server: %u, url: %s", sntp_current_server, sntp_server_addresses[sntp_current_server]);
	err = dns_gethostbyname(sntp_server_addresses[sntp_current_server], &sntp_server_address, sntp_dns_found, NULL);

	if(err == ERR_INPROGRESS){
		/* DNS request sent, wait for sntp_dns_found being called */
		P_LOGD(logTag, "sntp_request: Waiting for server address to be resolved");
		return;
	}

	if(err == ERR_OK){
		P_LOGD(logTag, "sntp_server_address: %s", ipaddr_ntoa(&sntp_server_address));
		sntp_send_request(&sntp_server_address);
	}else{
		/* address conversion failed, try another server */
		//debug("sntp_request: Invalid server address, trying next server.\n");
		sys_timeout(SNTP_RETRY_TIMEOUT, sntp_try_next_server, NULL);
	}
}

/**
 * Initialize this module when using raw API.
 * Send out request instantly or after SNTP_STARTUP_DELAY.
 */
void sntp_init(void){
	sntp_pcb = udp_new();
	if(sntp_pcb != NULL){
		udp_recv(sntp_pcb, sntp_recv, NULL);
		sntp_request(NULL);
	}else{
		P_LOGE(logTag, "Error create sntp_pcb");
	}
}

/******************************** END OF FILE ********************************/

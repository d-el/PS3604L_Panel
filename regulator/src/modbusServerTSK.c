/*!****************************************************************************
 * @file		httpServerTSK.c
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		06.09.2017
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		HTTP Server
 */

/*!****************************************************************************
 * Include
 */
#include "stdio.h"
#include "stdbool.h"
#include "string.h"
#include "assert.h"
#include "plog.h"
#include "lwip/api.h"
#include "lwip/ip.h"
#include "regulatorConnTSK.h"

#define LOG_LOCAL_LEVEL P_LOG_INFO

/*!****************************************************************************
 * MEMORY
 */
static char *logTag = "ModBusTCP";
uint32_t numberRequest;

typedef struct __attribute__((packed)){
	uint16_t transactionIdentifier;
	uint16_t ProtocolIdentifier;
	uint16_t Length;
	uint8_t UnitIdentifier;
}MBAPheader_t;

typedef struct __attribute__((packed)){
	MBAPheader_t MBAPheader;
	uint8_t PDU[253/*PDU*/ + 2/*reserve expansion for CRC*/];
}tcpModbusPacket_t;

/*!****************************************************************************
 * @param[in] conn - netconn descriptor
 */
void modbus_server_serve(struct netconn *conn){
	struct netbuf *inbuf;
	err_t res;

	while(1){
		res = netconn_recv(conn, &inbuf);
		if(res != ERR_OK){
			P_LOGW(logTag, "Error in netconn_recv (%i), %s", res, lwip_strerr(res));
			break;
		}
		else{ //res == ERR_OK
			uint8_t *readdata = NULL;
			u16_t buflen;

			netbuf_data(inbuf, (void**)&readdata, &buflen);
			P_LOGI(logTag, "Netbuf_data: %p (%"PRIu16")", readdata, buflen);
			if(LOG_LOCAL_LEVEL >= P_LOG_DEBUG) hexdumpcolumn(readdata, buflen, 32);
			tcpModbusPacket_t tcpModbusPacket;
			memcpy(&tcpModbusPacket, readdata, buflen);

			uint16_t len = __builtin_bswap16/*ntoh*/(tcpModbusPacket.MBAPheader.Length);
			if(reg_modbusRequest((uint8_t*)&tcpModbusPacket.MBAPheader.UnitIdentifier, &len)){
				size_t lentowrite = 6 + len;
				tcpModbusPacket.MBAPheader.Length = __builtin_bswap16(len);
				P_LOGI(logTag, "send response to client (%"PRIu16")", lentowrite);
				if(LOG_LOCAL_LEVEL >= P_LOG_DEBUG) hexdumpcolumn(&tcpModbusPacket, lentowrite, 32);
				len = __builtin_bswap16(len); // hton
				netconn_write(conn, &tcpModbusPacket, lentowrite, NETCONN_NOCOPY);
			}
		}

		/* Delete the buffer (netconn_recv gives us ownership,
			 so we have to make sure to deallocate the buffer) */
		netbuf_delete(inbuf);
	}
}

/*!****************************************************************************
 *
 */
void modbusServerTSK(void *pPrm){
	(void)pPrm;
	struct netconn *conn, *newconn;
	err_t err;

	/* Create a new TCP connection handle */
	conn = netconn_new(NETCONN_TCP);
	if(conn == NULL){
		P_LOGE(logTag, "Error create netconn");
		vTaskDelete(NULL);
	}

	/* Bind to port with default IP address */
	err = netconn_bind(conn, NULL, 502);
	if(err != ERR_OK){
		P_LOGE(logTag, "Error netconn bind");
		vTaskDelete(NULL);
	}

	P_LOGD(logTag, "Put the connection into LISTEN state");
	netconn_listen(conn);

	while(1){
		P_LOGD(logTag, "Accept any icoming connection");
		err = netconn_accept(conn, &newconn);

		if(err != ERR_OK){
			P_LOGW(logTag, "Error %i", err);
		}
		else{
			numberRequest++;
			P_LOGI(logTag, "Connection %"PRIu32", Remote IP address: %s", numberRequest, ipaddr_ntoa(&newconn->pcb.ip->remote_ip));
			reg_setremote(true);
			modbus_server_serve(newconn);
			reg_setremote(false);
			P_LOGD(logTag, "Delete connection\n");
			netconn_delete(newconn);
		}
	}
}

/******************************** END OF FILE ********************************/

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
#include "string.h"
#include "assert.h"
#include "plog.h"
#include "lwip/api.h"
#include "lwip/ip.h"
#include "htmlPage.h"
#include "httpServerTSK.h"

#define LEN				1024
#define LOG_LOCAL_LEVEL	P_LOG_WARN

/*!****************************************************************************
 * MEMORY
 */
static char *logTag = "HTTP";

httpServer_type httpServer;
char pageData[LEN];
const char http_200[]				= "HTTP/1.1 200 OK\r\n";
const char http_404[]				= "HTTP/1.1 404 Not Found\r\n";
const char http_server[]			= "Server:STM32 based\r\n";
const char http_connectionClose[] 	= "Connection: close\r\n";
const char http_headerEnd[]			= "\r\n";
const char *http_contentType[urlDataTypeNumber] = {
	[urlDataType_html]	= "Content-type: text/html\r\n",
	[urlDataType_css]	= "Content-type: text/css\r\n",
	[urlDataType_bin]	= "Content-type: text/plain\r\n",
	[urlDataType_js]	= "Content-type: application/x-javascript\r\n",
	[urlDataType_ico]	= "Content-type: image/vnd.microsoft.icon\r\n"
};

/*!****************************************************************************
 *
 */
uint32_t httpStrcmp(char *s1, char *s2){
	return !strncmp(s1, s2, strlen(s2));
}

/*!****************************************************************************
 * @param[in] conn - netconn descriptor
 */
void http_server_serve(struct netconn *conn){
	struct netbuf *inbuf;
	char *data = pageData;

	err_t res = netconn_recv(conn, &inbuf);
	if(res != ERR_OK){
		P_LOGE(logTag, "Error in netconn_recv, %i", res);
	}
	else{ //res == ERR_OK
		char* buf;
		u16_t buflen;
		netbuf_data(inbuf, (void**)&buf, &buflen);
		P_LOGD(logTag, "Netbuf_data: %p (%" PRIu16 ")", buf, buflen);
		P_LOGD(logTag, "Netbuf_data: %s", buf);

		if(httpStrcmp(buf, "GET /")){
			const httpResource_type *urlres = NULL;
			for(uint32_t i = 0; i < getUrlNumber; i++){
				if(httpStrcmp(buf + strlen("GET "), httpResource[i].url)){
					urlres = &httpResource[i];
				}
			}
			if(urlres != NULL){
				urlData_type urlData;

				if(urlres->handler != NULL){
					urlData = urlres->handler();
				}else{
					urlData = urlres->data;
				}

				strcpy(data, http_200);
				strcat(data, http_server);
				strcat(data, http_connectionClose);
				strcat(data, http_contentType[urlData.type]);
				strcat(data, http_headerEnd);
				uint32_t size = strlen(data);
				netconn_write(conn, data, size, NETCONN_NOCOPY);
				P_LOGD(logTag, "Netconn_write (%"PRIu32")", size);

				if(urlData.size != 0){
					size = urlData.size;
				}else{
					size = strlen(urlData.payload);
				}
				netconn_write(conn, urlData.payload, size, NETCONN_NOCOPY);
				P_LOGD(logTag, "Netconn_write (%"PRIu32")", size);
			}else{
				P_LOGE(logTag, "http_404");
				strcpy(data, http_404);
				strcat(data, http_server);
				strcat(data, http_connectionClose);
				strcat(data, http_contentType[httpResource[getUrlNumber - 1].data.type]);
				strcat(data, http_headerEnd);
				uint32_t size = strlen(data);
				netconn_write(conn, data, size, NETCONN_NOCOPY);
				P_LOGD(logTag, "Netconn_write (%"PRIu32")", size);

				if(httpResource[getUrlNumber - 1].data.size != 0){
					size = httpResource[getUrlNumber - 1].data.size;
				}else{
					size = strlen(httpResource[getUrlNumber - 1].data.payload);
				}

				netconn_write(conn, httpResource[getUrlNumber - 1].data.payload, size, NETCONN_NOCOPY);
				P_LOGD(logTag, "Netconn_write (%"PRIu32")", size);
			}
		}

		else if(httpStrcmp(buf, "POST /\r\n")){
			P_LOGD(logTag, "POST /");
		}
	}

	/* Delete the buffer (netconn_recv gives us ownership,
	 so we have to make sure to deallocate the buffer) */
	netbuf_delete(inbuf);

	/* Close the connection (server closes in HTTP) */
	netconn_close(conn);
}

/*!****************************************************************************
 *
 */
void httpServerTSK(void *pPrm){
	(void)pPrm;

	while(!netif_ip4_addr(netif_default)->addr){
		vTaskDelay(pdMS_TO_TICKS(100));
	}

	/* Create a new TCP connection handle */
	struct netconn *conn = netconn_new(NETCONN_TCP);
	if(conn == NULL){
		P_LOGE(logTag, "Error create netconn");
		vTaskDelete(NULL);
	}

	/* Bind to port 80 (HTTP) with default IP address */
	err_t err = netconn_bind(conn, NULL, 80);
	if(err != ERR_OK){
		P_LOGE(logTag, "Error netconn bind");
		vTaskDelete(NULL);
	}

	P_LOGD(logTag, "Put the connection into LISTEN state");
	netconn_listen(conn);

	while(1){
		P_LOGD(logTag, "Accept any icoming connection");
		struct netconn *newconn;
		err = netconn_accept(conn, &newconn);
		netconn_set_recvtimeout(newconn, 3000);

		if(err != ERR_OK){
			P_LOGW(logTag, "Error %i", err);
		}
		else{
			httpServer.numberRequest++;
			P_LOGD(logTag, "Connection %"PRIu32", Remote IP address: %s", httpServer.numberRequest, ipaddr_ntoa(&newconn->pcb.ip->remote_ip));
			http_server_serve(newconn);

			P_LOGD(logTag, "Delete connection\n");
			netconn_delete(newconn);
		}
	}
}

/******************************** END OF FILE ********************************/

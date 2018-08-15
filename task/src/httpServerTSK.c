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
#define LOG_LOCAL_LEVEL P_LOG_ERROR

/*!****************************************************************************
 * MEMORY
 */
static char *logTag = "HTTP";

httpServer_type httpServer;
char pageData[LEN];
const char http_200[] 				= "HTTP/1.1 200 OK\n";
const char http_404[] 				= "HTTP/1.1 404 Not Found\n";
const char http_server[] 			= "Server: by Storozhenko Roman\n";
const char http_connectionClose[] 	= "Connection: close\n";
const char http_headerEnd[] 		= "\n";
const char *http_contentType[urlDataTypeNumber] = {
	[urlDataType_html]	= "Content-type: text/html\n",
	[urlDataType_css]	= "Content-type: text/css\n",
	[urlDataType_bin]	= "Content-type: text/plain\n",
	[urlDataType_js]	= "Content-type: application/x-javascript\n",
	[urlDataType_ico]	= "Content-type: image/vnd.microsoft.icon\n"
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
	err_t res;
	char* buf;
	u16_t buflen;
	char *data = pageData;

	res = netconn_recv(conn, &inbuf);
	if(res != ERR_OK){
		P_LOGE(logTag, "Error in netconn_recv, %d", res);
	}
	else{ //res == ERR_OK
		netbuf_data(inbuf, (void**)&buf, &buflen);
		P_LOGD(logTag, "Netbuf_data: 0x%p (%i)", buf, buflen);

		if(httpStrcmp(buf, "GET /")){
			const url_type *url = NULL;
			for(uint32_t i = 0; i < getUrlNumber; i++){
				if(httpStrcmp(buf + strlen("GET "), getUrlTable[i].url)){
					url = &getUrlTable[i];
				}
			}
			if(url != NULL){
				urlData_type urlData;

				if(url->handler != NULL){
					urlData = url->handler();
				}else{
					urlData = url->data;
				}

				strcpy(data, http_200);
				strcat(data, http_server);
				strcat(data, http_connectionClose);
				strcat(data, http_contentType[urlData.type]);
				strcat(data, http_headerEnd);
				uint32_t size = strlen(data);
				netconn_write(conn, data, size, NETCONN_NOCOPY);
				P_LOGD(logTag, "Netconn_write (%lu)", size);

				if(urlData.size != 0){
					size = urlData.size;
				}else{
					size = strlen(urlData.data.html);
				}
				netconn_write(conn, urlData.data.html, size, NETCONN_NOCOPY);
				P_LOGD(logTag, "Netconn_write (%lu)", size);
			}else{
				strcpy(data, http_404);
				strcat(data, http_server);
				strcat(data, http_connectionClose);
				strcat(data, http_contentType[getUrlTable[getUrlNumber - 1].data.type]);
				strcat(data, http_headerEnd);
				uint32_t size = strlen(data);
				netconn_write(conn, data, size, NETCONN_NOCOPY);
				P_LOGD(logTag, "Netconn_write (%lu)", size);

				if(getUrlTable[getUrlNumber - 1].data.size != 0){
					size = getUrlTable[getUrlNumber - 1].data.size;
				}else{
					size = strlen(getUrlTable[getUrlNumber - 1].data.data.html);
				}

				netconn_write(conn, getUrlTable[getUrlNumber - 1].data.data.html, size, NETCONN_NOCOPY);
				P_LOGD(logTag, "Netconn_write (%lu)", size);
			}
		}

		else if(httpStrcmp(buf, "POST /\n")){
			P_LOGD(logTag, "POST /");
		}
	}

	/* Close the connection (server closes in HTTP) */
	netconn_close(conn);

	/* Delete the buffer (netconn_recv gives us ownership,
	 so we have to make sure to deallocate the buffer) */
	netbuf_delete(inbuf);
}

/*!****************************************************************************
 *
 */
void httpServerTSK(void *pPrm){
	(void)pPrm;
	struct netconn *conn, *newconn;
	err_t err;

	/* Create a new TCP connection handle */
	conn = netconn_new(NETCONN_TCP);
	assert(conn != NULL);

	/* Bind to port 80 (HTTP) with default IP address */
	err = netconn_bind(conn, NULL, 80);
	assert(err == ERR_OK);

	P_LOGD(logTag, "Put the connection into LISTEN state");
	netconn_listen(conn);

	while(1){
		P_LOGD(logTag, "Accept any icoming connection");
		err = netconn_accept(conn, &newconn);

		if(err != ERR_OK){
			P_LOGW(logTag, "Error %i", err);
		}
		else{
			httpServer.numberRequest++;
			P_LOGD(logTag, "Connection %lu, Remote IP address: %s", httpServer.numberRequest, ipaddr_ntoa(&newconn->pcb.ip->remote_ip));
			http_server_serve(newconn);

			P_LOGD(logTag, "Delete connection\n");
			netconn_delete(newconn);
		}
	}
}

/*************** LGPL ************** END OF FILE *********** D_EL ************/

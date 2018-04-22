/*!****************************************************************************
 * @file		httpServerTSK.c
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		06.09.2017
 * @copyright	GNU Lesser General Public License v3
 * @brief		HTTP Server
 */

/*!****************************************************************************
 * Include
 */
#include "assert.h"
#include "printp.h"
#include "lwip/api.h"
#include "lwip/ip.h"
#include "stdio.h"
#include "string.h"
#include "htmlPage.h"
#include "httpServerTSK.h"

/**
 * HTTP_DEBUG_LEVEL: Enable debugging for http server
 */
#define HTTP_DEBUG_LEVEL	2	//0 - No printed
								//1 - Error
								//2 - Connected IP Address
								//3 - All
#define HTTP_DEBUG_ERR		(HTTP_DEBUG_LEVEL >= 1)
#define HTTP_DEBUG_IP		(HTTP_DEBUG_LEVEL >= 2)
#define HTTP_DEBUG_ALL		(HTTP_DEBUG_LEVEL >= 3)

#define LEN					1024

/*!****************************************************************************
 * MEMORY
 */
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
	size_t len;
	ip_addr_t *remote_addr;
	char *data = pageData;

	res = netconn_recv(conn, &inbuf);
	if(res != ERR_OK){
		report(HTTP_DEBUG_ERR, "[HTTP] Error in netconn_recv, %d\n", res);
	}
	else{ //res == ERR_OK
		netbuf_data(inbuf, (void**)&buf, &buflen);
		report(HTTP_DEBUG_ALL, "[HTTP] netbuf_data: 0x%x (%u)\n", buf, buflen);

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
				report(HTTP_DEBUG_ALL, "[HTTP] netconn_write (%u)\n", size);

				if(urlData.size != 0){
					size = urlData.size;
				}else{
					size = strlen(urlData.data.html);
				}
				netconn_write(conn, urlData.data.html, size, NETCONN_NOCOPY);
				report(HTTP_DEBUG_ALL, "[HTTP] netconn_write (%u)\n", size);
			}else{
				strcpy(data, http_404);
				strcat(data, http_server);
				strcat(data, http_connectionClose);
				strcat(data, http_contentType[getUrlTable[getUrlNumber - 1].data.type]);
				strcat(data, http_headerEnd);
				uint32_t size = strlen(data);
				netconn_write(conn, data, size, NETCONN_NOCOPY);
				report(HTTP_DEBUG_ALL, "[HTTP] netconn_write (%u)\n", size);

				if(getUrlTable[getUrlNumber - 1].data.size != 0){
					size = getUrlTable[getUrlNumber - 1].data.size;
				}else{
					size = strlen(getUrlTable[getUrlNumber - 1].data.data.html);
				}

				netconn_write(conn, getUrlTable[getUrlNumber - 1].data.data.html, size, NETCONN_NOCOPY);
				report(HTTP_DEBUG_ALL, "[HTTP] netconn_write (%u)\n", size);
			}
		}

		else if(httpStrcmp(buf, "POST /\n")){
			report(HTTP_DEBUG_ALL, "[HTTP] POST /\n");
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
	struct netconn *conn, *newconn;
	err_t err;

	/* Create a new TCP connection handle */
	conn = netconn_new(NETCONN_TCP);
	assert(conn != NULL);

	/* Bind to port 80 (HTTP) with default IP address */
	err = netconn_bind(conn, NULL, 80);
	assert(err == ERR_OK);

	report(HTTP_DEBUG_ALL, "[HTTP] Put the connection into LISTEN state\n");
	netconn_listen(conn);

	while(1){
		report(HTTP_DEBUG_ALL, "[HTTP] Accept any icoming connection\n");
		err = netconn_accept(conn, &newconn);

		if(err != ERR_OK){
			report(HTTP_DEBUG_ERR, "[HTTP] Error %i\n", err);
		}
		else{
			httpServer.numberRequest++;
			report(HTTP_DEBUG_IP, "[HTTP] Connection %u, Remote IP address: %s\n", httpServer.numberRequest, ipaddr_ntoa(&newconn->pcb.ip->remote_ip));
			http_server_serve(newconn);

			report(HTTP_DEBUG_ALL, "[HTTP] Delete connection\n");
			netconn_delete(newconn);
		}
	}
}

/*************** LGPL ************** END OF FILE *********** D_EL ************/

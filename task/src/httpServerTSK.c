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
#include "httpServerTSK.h"

/**
 * HTTP_SERVER_DEBUG: Enable debugging for http server
 */
#define HTTP_DEBUG	0	//1 - ON, 0 - OFF

/*!****************************************************************************
 * MEMORY
 */
#define 	LEN 1024

char		pageData[LEN];
const char http_200[] 				= "HTTP/1.1 200 OK\r\n";
const char http_404[] 				= "HTTP/1.1 404 Not Found\r\n";
const char http_server[] 			= "Server: by Storozhenko Roman\r\n";
const char http_contentTypeHtml[]	= "Content-type: text/html\r\n";
const char http_connectionClose[] 	= "Connection: close\r\n";
const char http_contentLength[] 	= "Content-Length: ";
const char http_lineBreak[] 		= "\r\n";
const char http_headerEnd[] 		= "\r\n\r\n";

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

	if(res == ERR_OK){
		netbuf_data(inbuf, (void**)&buf, &buflen);

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

				if(urlData.type == urlDataTypeHtml){
					strcpy(data, http_200);
					strcat(data, http_server);
					strcat(data, http_connectionClose);
					strcat(data, http_contentTypeHtml);
					strcat(data, http_headerEnd);
					netconn_write(conn, data, strlen(data), NETCONN_NOCOPY);
					uint32_t size;
					if(urlData.size != 0){
						size = urlData.size;
					}else{
						size = strlen(urlData.data.html);
					}
					netconn_write(conn, urlData.data.html, size, NETCONN_NOCOPY);;
				}else{
					netconn_write(conn, urlData.data.bin, urlData.size, NETCONN_NOCOPY);
				}
			}else{
				strcpy(data, http_404);
				strcat(data, http_server);
				strcat(data, http_connectionClose);
				strcat(data, http_contentTypeHtml);
				strcat(data, http_headerEnd);
				netconn_write(conn, data, strlen(data), NETCONN_NOCOPY);
				uint32_t size;
				if(getUrlTable[getUrlNumber - 1].data.size != 0){
					size = getUrlTable[getUrlNumber - 1].data.size;
				}else{
					size = strlen(getUrlTable[getUrlNumber - 1].data.data.html);
				}

				netconn_write(conn, getUrlTable[getUrlNumber - 1].data.data.html, size, NETCONN_NOCOPY);
			}
		}

		else if(httpStrcmp(buf, "POST /")){
			printdmsg(HTTP_DEBUG, ("POST /"));
		}

		//printdmsg(HTTP_DEBUG, ("Transmit page data to client, length %u bytes\n", strlen(pageData)));
		//netconn_write(conn, pageData, strlen(pageData), NETCONN_NOCOPY);
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
void httpServerError(){
	vTaskDelete(NULL);	// Delete this task
}

/*!****************************************************************************
 *
 */
void httpServerTSK(void *pPrm){
	struct netconn *conn, *newconn;
	err_t err;

	/* Create a new TCP connection handle */
	conn = netconn_new(NETCONN_TCP);
	stopif(conn == NULL, httpServerError(), "can not create netconn");

	/* Bind to port 80 (HTTP) with default IP address */
	err = netconn_bind(conn, NULL, 80);
	stopif(err != ERR_OK, httpServerError(), "can not bind netconn");

	printdmsg(HTTP_DEBUG, ("Put the connection into LISTEN state\n"));
	netconn_listen(conn);

	while(1){
		printdmsg(HTTP_DEBUG, ("Accept any icoming connection\n"));
		err = netconn_accept(conn, &newconn);

		if(err != ERR_OK){
			print("Error %i", err);
		}
		fp.state.lanActive = 1;

		printdmsg(HTTP_DEBUG, ("Serve connection\n"));
		printdmsg(HTTP_DEBUG, ("Remote IP address: %s\n", ipaddr_ntoa(&newconn->pcb.ip->remote_ip)));
		http_server_serve(newconn);

		printdmsg(HTTP_DEBUG, ("Delete connection\n\n"));
		netconn_delete(newconn);
	}
}

/*************** LGPL ************** END OF FILE *********** D_EL ************/

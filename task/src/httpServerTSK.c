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

/*!****************************************************************************
 * MEMORY
 */
#define 	LEN 4096
char 		pageData[LEN];
char 		data2[LEN];
char 		debugString[LEN];
uint32_t 	call_get = 0, call_post = 0;

const char http_200[] 				= "HTTP/1.1 200 OK\r\n";
const char http_404[] 				= "HTTP/1.0 404 Not Found\r\n";
const char http_server[] 			= "Server: self-made\r\n";
const char http_contentTypeHtml[]	= "Content-type: text/html\r\n";
const char http_connectionClose[] 	= "Connection: close\r\n";
const char http_contentLength[] 	= "Content-Length: ";
const char http_lineBreak[] 		= "\r\n";
const char http_headerEnd[] 		= "\r\n\r\n";
const char html404[] = {
	"<!DOCTYPE html>"
	"<html lang=\"en-US\" style=\"height: 100%;\">"
	"<head>"
		"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">"
		"<title>Page Not Found (404)</title>"
		"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
	"</head>"
	"<body>"
	"<div align=\"center\">"
		"<p>"
			"<font face=\"Cursive\" size=\"7\" color=\"black\">404 </font>"
			"<font face=\"Cursive\" size=\"6\" color=\"black\">error</font>"
		"</p>"
		"<hr align=\"center\" width=\"70%\" size=\"1\" color=\"black\" />"
		"<p><font face=\"Cursive\" size=\"6\" color=\"black\">Page not found</font></p>"
	"</div>"
	"</body>"
	"</html>"
};

const uint8_t favicon[] = {
	0x42,0x4d,0xbe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3e,0x00,0x00,0x00,0x28,
	0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x01,0x00,0x01,0x00,
	0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0xc3,0x0e,0x00,0x00,0xc3,0x0e,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,
	0xff,0x00,0xff,0xff,0xff,0xff,0xff,0xe0,0x0f,0xff,0xff,0x00,0x03,0xff,0xfe,
	0x00,0x00,0xff,0xfc,0x0f,0xe0,0x7f,0xf8,0x7f,0xfc,0x3f,0xf0,0xff,0xfe,0x1f,
	0xe1,0xff,0xff,0x0f,0xe1,0xcf,0xf7,0x8f,0xc3,0xc7,0xc7,0x87,0xc7,0xe3,0x8f,
	0xc7,0xc7,0xe8,0x2f,0xe3,0x8f,0xe6,0x4f,0xe3,0x8f,0xf7,0xcf,0xe3,0x8f,0xe7,
	0xcf,0xe3,0x8f,0xc7,0xc7,0xe3,0x8f,0x8f,0xf3,0xe3,0x8f,0x03,0x81,0xe3,0x8e,
	0x01,0x00,0x63,0xc7,0xfd,0x3f,0xe3,0xc7,0xfd,0x7f,0xc7,0xc3,0xfc,0x7f,0x87,
	0xe3,0xfe,0x7f,0x8f,0xe1,0xfe,0xff,0x0f,0xf0,0xfe,0xfe,0x1f,0xf8,0x7f,0xfc,
	0x3f,0xfc,0x0f,0xf0,0x7f,0xfe,0x00,0x00,0xff,0xff,0x00,0x03,0xff,0xff,0xe0,
	0x0f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
};

/*!****************************************************************************
 * @param[in] conn - netconn descriptor
 */
void send404(char *data){
	strcat(data, html404);
}

/*!****************************************************************************
 * @param[in] conn - netconn descriptor
 */
void sendHtmlIco(struct netconn *conn){
	netconn_write(conn, favicon, sizeof(favicon), NETCONN_NOCOPY);
}

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

		/*debugn(	"\r\n======================\r\n"
				"Receive\r\n"
				);
		SH_SendString(buf);*/

		/*ip_addr_t *remote_addr = &conn->pcb.ip->remote_ip;
		ipcnt_type *i = ipFind(remote_addr->addr);
		if(i != NULL){
			i->cnt++;
		}else{
			ipInsert(remote_addr->addr);
		}*/

		if(httpStrcmp(buf, "GET /")){
			if(httpStrcmp(buf, "GET / ")){	//Main page
				strcpy(data, http_200);
				strcat(data, http_server);
				strcat(data, http_connectionClose);
				strcat(data, http_contentTypeHtml);
				strcat(data, http_headerEnd);
				sendHtmlPage(data);
			}
			else if(httpStrcmp(buf, "GET /favicon.ico")){
				sendHtmlIco(conn);
			}
			else{
				strcpy(data, http_404);
				strcat(data, http_server);
				strcat(data, http_connectionClose);
				strcat(data, http_contentTypeHtml);
				strcat(data, http_headerEnd);
				send404(data);
			}
		}

		else if(httpStrcmp(buf, "POST /")){

		}

		// Transmit page data to client
		netconn_write(conn, pageData, strlen(pageData), NETCONN_NOCOPY);
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
	if(conn == NULL){
		debugn("can not create netconn");
		httpServerError();
	}

	/* Bind to port 80 (HTTP) with default IP address */
	err = netconn_bind(conn, NULL, 80);
	if(err != ERR_OK){
		debugn("can not bind netconn");
		httpServerError();
	}

	/* Put the connection into LISTEN state */
	netconn_listen(conn);

	while(1){
		/* accept any icoming connection */
		err = netconn_accept(conn, &newconn);

		/* serve connection */
		http_server_serve(newconn);

		/* delete connection */
		netconn_delete(newconn);
	}
}

/*************** LGPL ************** END OF FILE *********** D_EL ************/

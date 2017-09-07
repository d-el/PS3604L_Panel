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
#define LEN 1024
char 		data[LEN];
char 		debugString[LEN];

const static char http_html_header[] = {
	"HTTP/1.1 200 OK\r\nContent-type: text/html"
	"\r\n\r\n"
};

const char *html = {
	"<!DOCTYPE html>"
	"<html lang=\"en-US\" style=\"height: 100%;\">"
	"<head>"
		"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">"
		"<title>PS3604L</title>"
		"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
	"</head>"
	"<body>"
		"<p><a href=\"https://www.wikipedia.org/\">Wiki</a></p>"
		"<p><a href=\"https://lurkmore.to/\">Lurk</a></p>"
		"<a href=\"LED_ON\"> <button>LED ON</button> </a>"
		"<a href=\"LED_OFF\"> <button>LED OFF</button> </a>"
		"<br><br>"
		"<a href=\"UART_BR9600\"> <button>Set 9600</button> </a>"
		"<a href=\"UART_BR38400\"> <button>Set 38400</button> </a>"
		"<a href=\"UART_BR115200\"> <button>Set 115200</button> </a>"
		"<br><br>"
		"<div id='content' style=\"height: 50px; width: 150px; font-size: 12pt; border: 1px solid #999; background-color: #B0E0E6;\" >"
		"<p>22.08.2017</p>"
		"<br><br>"
	"</body>"
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
 *
 */
void sendHtmlPage(struct netconn *conn){
	//Send the HTML header
	netconn_write(conn, http_html_header, sizeof(http_html_header) - 1, NETCONN_NOCOPY);
	//debugn("Send the HTML header, write bytes: %u", sizeof(http_html_header) - 1);

	//Send HTML page
	netconn_write(conn, html, strlen(html), NETCONN_NOCOPY);
	//debugn("Send our HTML page, write bytes: %u", strlen(html));
}

/*!****************************************************************************
 *
 */
void sendHtmlIco(struct netconn *conn){
	//Send the HTML header
	netconn_write(conn, favicon, sizeof(favicon), NETCONN_NOCOPY);
	//debugn("Send the ico, write bytes: %u", sizeof(favicon));
}

/*!****************************************************************************
 *
 */
void sendHtmlDebug(struct netconn *conn, char *string){
	static uint32_t call_get = 0;

	//Send debug text
	sprintf(data,
			"<textarea disabled>"
				"Debug output\n"
				"%s"
			"</textarea>",
			string);
	netconn_write(conn, data, strlen(data), NETCONN_NOCOPY);
	//debugn("Send our HTML page, write bytes: %u", strlen(data));
}

/*!****************************************************************************
 *
 */
uint32_t httpStrcmp(char *s1, char *s2){
	return !strncmp(s1, s2, strlen(s2));
}

/*!****************************************************************************
 *
 */
void http_server_serve(struct netconn *conn){
	struct netbuf *inbuf;
	err_t res;
	char* buf;
	u16_t buflen;
	size_t len;
	static u16_t call_get = 0, call_post = 0;

	res = netconn_recv(conn, &inbuf);

	if(res == ERR_OK){
		netbuf_data(inbuf, (void**)&buf, &buflen);

		/*debugn(	"\r\n======================\r\n"
				"Receive\r\n"
				);*/
		SH_SendString(buf);

		if(httpStrcmp(buf, "GET /")){
			debugn("\r\nGET / - number %u", ++call_get);

			if(httpStrcmp(buf, "GET / ")){	//Main page
				sendHtmlPage(conn);
			}
			else if(httpStrcmp(buf, "GET /favicon.ico")){
				sendHtmlIco(conn);
			}
			else if(httpStrcmp(buf, "GET /LED_ON")){
				gppin_set(GP_LED0);
				sendHtmlPage(conn);
			}
			else if(httpStrcmp(buf, "GET /LED_OFF")){
				gppin_reset(GP_LED0);
				sendHtmlPage(conn);
			}
			else if(httpStrcmp(buf, "GET /UART_BR9600")){
				uart_setBaud(uart1, BR9600);
				sendHtmlPage(conn);
			}
			else if(httpStrcmp(buf, "GET /UART_BR38400")){
				uart_setBaud(uart1, BR38400);
				sendHtmlPage(conn);
			}
			else if(httpStrcmp(buf, "GET /UART_BR115200")){
				uart_setBaud(uart1, BR115200);
				sendHtmlPage(conn);
			}

			sprintf(debugString, "HTTP \"GET\" calls: %u", call_get);
			sendHtmlDebug(conn, debugString);
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

		gppin_set(GP_LED1);

		/* serve connection */
		http_server_serve(newconn);

		gppin_reset(GP_LED1);

		/* delete connection */
		netconn_delete(newconn);
	}
}

/*************** LGPL ************** END OF FILE *********** D_EL ************/

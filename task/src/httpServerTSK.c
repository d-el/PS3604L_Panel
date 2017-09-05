#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include <stdio.h>
#include <string.h>
#include <task/inc/httpServerTSK.h>
#include "debugCore.h"
#include "gpio.h"
#include "uart.h"

#define LEN 1024
char data[LEN];

const static char http_html_header[] = {
	"HTTP/1.1 200 OK\r\nContent-type: text/html"
	"\r\n\r\n"
};

const char *html = {
	"<!DOCTYPE html>"
	"<html>"
	"<head>"
	"<title>PS3604L</title>"
	"<style media=\"handheld\"> "
		"body { width: 300; } "
	"</style>"
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
	"</html>"
};

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

		debugn(	"======================\r\n"
				"Receive\r\n"
				);
		SH_SendString(buf);

		if((buflen >= 5) && (strncmp(buf, "GET /", 5) == 0)){
			debugn("GET / - number %u", ++call_get);

			//LED Control
			if(strncmp(buf, "GET /LED_ON", 11) == 0){
				gppin_set(GP_LED2);
			}
			if(strncmp(buf, "GET /LED_OFF", 12) == 0){
				gppin_reset(GP_LED2);
			}
			//UART
			if(strncmp(buf, "GET /UART_BR9600", 16) == 0){
				uart_setBaud(uart1, BR9600);
			}
			if(strncmp(buf, "GET /UART_BR38400", 17) == 0){
				uart_setBaud(uart1, BR38400);
			}
			if(strncmp(buf, "GET /UART_BR115200", 18) == 0){
				uart_setBaud(uart1, BR115200);
			}

			//Send the HTML header
			netconn_write(conn, http_html_header, sizeof(http_html_header) - 1, NETCONN_NOCOPY);
			debugn("Send the HTML header, write bytes: %u", sizeof(http_html_header) - 1);

			//Send HTML page
			netconn_write(conn, html, strlen(html), NETCONN_NOCOPY);
			debugn("Send our HTML page, write bytes: %u", strlen(html));

			//Send HTML page
			sprintf(data,
					"<textarea disabled>"
						"Debug output\n"
						"HTTP \"GET\" calls: %u"
					"</textarea>"
					, call_get);
			netconn_write(conn, data, strlen(data), NETCONN_NOCOPY);
			debugn("Send our HTML page, write bytes: %u", strlen(html));

			debugn("");
		}

		if((buflen >= 5) && (strncmp(buf, "POST /", 5) == 0)){
			debugn("POST %u", call_post);
			call_post++;
		}
	}

	/* Close the connection (server closes in HTTP) */
	netconn_close(conn);

	/* Delete the buffer (netconn_recv gives us ownership,
	 so we have to make sure to deallocate the buffer) */
	netbuf_delete(inbuf);
}

void httpServerTSK(void *pPrm){
	struct netconn *conn, *newconn;
	err_t err;

	/* Create a new TCP connection handle */
	/*conn = netconn_new(NETCONN_TCP);*/
	conn = (struct netconn*)pPrm;

	if(conn != NULL){
		/* Bind to port 80 (HTTP) with default IP address */
		err = netconn_bind(conn, NULL, 80);

		if(err == ERR_OK){
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
		}else{
			debugn("can not bind netconn");
		}
	}else{
		debugn("can not create netconn");
	}
}

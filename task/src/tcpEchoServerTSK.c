#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include <stdio.h>
#include <string.h>
#include <task/inc/httpServerTSK.h>
#include "debugCore.h"

void tcpEchoServerError(){
	vTaskDelete(NULL);	// Delete this task
}

void tcpEchoServerTSK(void *pPrm){
	struct netconn *conn, *newconn;
	err_t err;

	// Create a new TCP connection handle
	//conn = netconn_new(NETCONN_TCP);
	conn = (struct netconn*)pPrm;
	if(conn == NULL){
		debugn("can not create netconn");
		tcpEchoServerError();
	}

	// Bind to port 7 with default IP address
	err = netconn_bind(conn, NULL, 7);
	if(err != ERR_OK){
		debugn("can not bind netconn");
		tcpEchoServerError();
	}

	// Put the connection into LISTEN state
	err = netconn_listen(conn);
	if(err != ERR_OK){
		debugn("can not bind netconn");
		tcpEchoServerError();
	}

	while(1){
		// Grab new connection
		err = netconn_accept(conn, &newconn);
		debugn("accepted new connection %p\n", newconn);

		// Process the new connection
		if(err == ERR_OK){
			struct netbuf 	*buf;
			void 			*data;
			u16_t 			len;

			while((err = netconn_recv(newconn, &buf)) == ERR_OK){
				debugn("netconn_recv = %i", err);
				do{
					err = netbuf_data(buf, &data, &len);
					debugn("netbuf_data = %i, len = %u", err, len);
					err = netconn_write(newconn, data, len, NETCONN_COPY);
					debugn("netconn_write = %i", err);
				}while(netbuf_next(buf) >= 0);
				netbuf_delete(buf);
			}
			// Close connection and discard connection identifier
			netconn_close(newconn);
			netconn_delete(newconn);
		}
	}
}

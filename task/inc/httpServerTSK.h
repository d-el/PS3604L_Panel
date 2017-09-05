#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"

void http_server_serve(struct netconn *conn);
void httpServerTSK(void *pPrm);

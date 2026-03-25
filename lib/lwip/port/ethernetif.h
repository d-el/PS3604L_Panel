#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "lwip/err.h"
#include "lwip/netif.h"

void ethernetif_setMAC(struct netif *netif, const uint8_t mac[6]);
err_t ethernetif_init(struct netif *netif);

#ifdef __cplusplus
}
#endif

#endif

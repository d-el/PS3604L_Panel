set(LWIPPATH lwip-STABLE-2_2_0_RELEASE)

add_library(lwip STATIC
	port/ethernetif.c
	port/sys_arch.c
	
	${LWIPPATH}/src/apps/mqtt/mqtt.c
	
	${LWIPPATH}/src/api/api_lib.c
	${LWIPPATH}/src/api/api_msg.c
	${LWIPPATH}/src/api/err.c
	${LWIPPATH}/src/api/netbuf.c
	${LWIPPATH}/src/api/netdb.c
	${LWIPPATH}/src/api/netifapi.c
	${LWIPPATH}/src/api/sockets.c
	${LWIPPATH}/src/api/tcpip.c
	
	${LWIPPATH}/src/core/ipv4/acd.c
	${LWIPPATH}/src/core/ipv4/autoip.c
	${LWIPPATH}/src/core/ipv4/dhcp.c
	${LWIPPATH}/src/core/ipv4/etharp.c
	${LWIPPATH}/src/core/ipv4/icmp.c
	${LWIPPATH}/src/core/ipv4/igmp.c
	${LWIPPATH}/src/core/ipv4/ip4_addr.c
	${LWIPPATH}/src/core/ipv4/ip4_frag.c
	${LWIPPATH}/src/core/ipv4/ip4.c
	
	${LWIPPATH}/src/core/def.c
	${LWIPPATH}/src/core/dns.c
	${LWIPPATH}/src/core/inet_chksum.c
	${LWIPPATH}/src/core/init.c
	${LWIPPATH}/src/core/ip.c
	${LWIPPATH}/src/core/mem.c
	${LWIPPATH}/src/core/memp.c
	${LWIPPATH}/src/core/netif.c
	${LWIPPATH}/src/core/pbuf.c
	${LWIPPATH}/src/core/raw.c
	${LWIPPATH}/src/core/stats.c
	${LWIPPATH}/src/core/sys.c
	${LWIPPATH}/src/core/tcp_in.c
	${LWIPPATH}/src/core/tcp_out.c
	${LWIPPATH}/src/core/tcp.c
	${LWIPPATH}/src/core/timeouts.c
	${LWIPPATH}/src/core/udp.c
	
	${LWIPPATH}/src/netif/ppp/polarssl/arc4.c
	${LWIPPATH}/src/netif/ppp/polarssl/des.c
	${LWIPPATH}/src/netif/ppp/polarssl/md4.c
	${LWIPPATH}/src/netif/ppp/polarssl/md5.c
	${LWIPPATH}/src/netif/ppp/polarssl/sha1.c
	
	${LWIPPATH}/src/netif/ppp/auth.c
	${LWIPPATH}/src/netif/ppp/ccp.c
	${LWIPPATH}/src/netif/ppp/chap_ms.c
	${LWIPPATH}/src/netif/ppp/chap-md5.c
	${LWIPPATH}/src/netif/ppp/chap-new.c
	${LWIPPATH}/src/netif/ppp/demand.c
	${LWIPPATH}/src/netif/ppp/eap.c
	${LWIPPATH}/src/netif/ppp/ecp.c
	${LWIPPATH}/src/netif/ppp/eui64.c
	${LWIPPATH}/src/netif/ppp/fsm.c
	${LWIPPATH}/src/netif/ppp/ipcp.c
	${LWIPPATH}/src/netif/ppp/ipv6cp.c
	${LWIPPATH}/src/netif/ppp/lcp.c
	${LWIPPATH}/src/netif/ppp/magic.c
	${LWIPPATH}/src/netif/ppp/mppe.c
	${LWIPPATH}/src/netif/ppp/multilink.c
	${LWIPPATH}/src/netif/ppp/ppp.c
	${LWIPPATH}/src/netif/ppp/pppapi.c
	${LWIPPATH}/src/netif/ppp/pppcrypt.c
	${LWIPPATH}/src/netif/ppp/pppoe.c
	${LWIPPATH}/src/netif/ppp/pppol2tp.c
	${LWIPPATH}/src/netif/ppp/pppos.c
	${LWIPPATH}/src/netif/ppp/upap.c
	${LWIPPATH}/src/netif/ppp/utils.c
	${LWIPPATH}/src/netif/ppp/vj.c
	
	${LWIPPATH}/src/netif/ethernet.c
	${LWIPPATH}/src/netif/lowpan6.c
	${LWIPPATH}/src/netif/slipif.c
	)

target_include_directories(lwip
	PUBLIC	${LWIPPATH}/src/include/
			${LWIPPATH}/lwip/src/include
			${LWIPPATH}/lwip/port/include
			${LWIPPATH}/lwip/port
			port
			port/include
	)

target_link_libraries(lwip drivers config)

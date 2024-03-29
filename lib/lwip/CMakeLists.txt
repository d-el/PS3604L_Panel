add_library(lwip STATIC
	port/ethernetif.c
	port/sys_arch.c
	
	src/api/api_lib.c
	src/api/api_msg.c
	src/api/err.c
	src/api/netbuf.c
	src/api/netdb.c
	src/api/netifapi.c
	src/api/sockets.c
	src/api/tcpip.c
	
	src/core/ipv4/autoip.c
	src/core/ipv4/dhcp.c
	src/core/ipv4/etharp.c
	src/core/ipv4/icmp.c
	src/core/ipv4/igmp.c
	src/core/ipv4/ip4_addr.c
	src/core/ipv4/ip4_frag.c
	src/core/ipv4/ip4.c
	
	src/core/def.c
	src/core/dns.c
	src/core/inet_chksum.c
	src/core/init.c
	src/core/ip.c
	src/core/mem.c
	src/core/memp.c
	src/core/netif.c
	src/core/pbuf.c
	src/core/raw.c
	src/core/stats.c
	src/core/sys.c
	src/core/tcp_in.c
	src/core/tcp_out.c
	src/core/tcp.c
	src/core/timeouts.c
	src/core/udp.c
	
	src/netif/ppp/polarssl/arc4.c
	src/netif/ppp/polarssl/des.c
	src/netif/ppp/polarssl/md4.c
	src/netif/ppp/polarssl/md5.c
	src/netif/ppp/polarssl/sha1.c
	
	src/netif/ppp/auth.c
	src/netif/ppp/ccp.c
	src/netif/ppp/chap_ms.c
	src/netif/ppp/chap-md5.c
	src/netif/ppp/chap-new.c
	src/netif/ppp/demand.c
	src/netif/ppp/eap.c
	src/netif/ppp/ecp.c
	src/netif/ppp/eui64.c
	src/netif/ppp/fsm.c
	src/netif/ppp/ipcp.c
	src/netif/ppp/ipv6cp.c
	src/netif/ppp/lcp.c
	src/netif/ppp/magic.c
	src/netif/ppp/mppe.c
	src/netif/ppp/multilink.c
	src/netif/ppp/ppp.c
	src/netif/ppp/pppapi.c
	src/netif/ppp/pppcrypt.c
	src/netif/ppp/pppoe.c
	src/netif/ppp/pppol2tp.c
	src/netif/ppp/pppos.c
	src/netif/ppp/upap.c
	src/netif/ppp/utils.c
	src/netif/ppp/vj.c
	
	src/netif/ethernet.c
	src/netif/lowpan6.c
	src/netif/slipif.c
	)

target_include_directories(lwip
	PUBLIC	src/include/
			lwip/src/include
			wip/port/include
			lwip/port
			port
			port/include
	)

target_link_libraries(lwip drivers config)

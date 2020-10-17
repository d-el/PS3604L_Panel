add_library(net STATIC
	src/htmlPage.c
	src/httpServerTSK.c
	src/sntp.c
	)

target_include_directories(net PUBLIC
	include
	)

target_link_libraries(net lwip)

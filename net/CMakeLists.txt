add_library(net STATIC
	src/htmlPage.c
	src/httpServerTSK.c
	src/modbusServerTSK.cpp
	src/sntp.c
	)

target_include_directories(net PUBLIC
	include
	)

target_link_libraries(net lwip freemodbus)

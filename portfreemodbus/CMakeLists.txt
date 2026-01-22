add_library(portfreemodbus STATIC
	src/portevent.c
	src/portother.c
	src/porttcp.cpp
	src/porttimer.c
	)

target_include_directories(portfreemodbus PUBLIC
	include
	)

target_link_libraries(portfreemodbus freemodbus freertos utils config hal regulator)
	
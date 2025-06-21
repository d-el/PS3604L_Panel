add_library(regulator STATIC
	src/regulatorConnTSK.c
	src/regulatorTcpRequest.c
	)

target_include_directories(regulator PUBLIC
	include
	)

target_link_libraries(regulator freertos drivers utils lwip systems prmsystem libmodbus)

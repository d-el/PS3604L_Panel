add_library(regulator STATIC
	src/modbusServerTSK.c
	src/regulatorConnTSK.c
	)

target_include_directories(regulator PUBLIC
	include
	)

target_link_libraries(regulator freertos drivers utils lwip systems prmsystem libmodbus)

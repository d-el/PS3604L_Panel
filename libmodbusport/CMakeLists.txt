add_library(libmodbusport STATIC
	modbusprint.c
	rtuport.c
	)

target_link_libraries(libmodbusport freertos drivers utils)

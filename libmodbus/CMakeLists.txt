add_library(libmodbus STATIC
	src/modbus.c
	src/modbus-data.c
	src/modbus-rtu.c
	)

target_include_directories(libmodbus 
	PUBLIC src
	)

target_compile_definitions(libmodbus PUBLIC -DMCU)

target_compile_options(libmodbus PUBLIC "-Wno-unused-variable -O0")

target_link_libraries(libmodbus libmodbusport)

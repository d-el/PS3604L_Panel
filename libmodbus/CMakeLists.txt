add_library(libmodbus STATIC
	src/modbus.c
	src/modbus-data.c
	src/modbus-rtu.c
	)

target_include_directories(libmodbus 
	PUBLIC src
	)

target_compile_definitions(libmodbus PUBLIC -DLIB_MODBUS_MCU)

target_compile_options(libmodbus PUBLIC "-Wno-unused-variable")

target_link_libraries(libmodbus libmodbusport)

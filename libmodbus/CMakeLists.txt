add_library(libmodbus STATIC
	src/modbus.c
	src/modbus-data.c
	src/modbus-rtu.c
	)

target_include_directories(libmodbus 
	PUBLIC src
	)

set(LIBMODBUS_VERSION "3.1.6")
set(LIBMODBUS_VERSION_MAJOR 3)
set(LIBMODBUS_VERSION_MINOR 1)
set(LIBMODBUS_VERSION_MICRO 6)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/modbus-version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/src/modbus-version.h @ONLY)

target_compile_definitions(libmodbus PUBLIC -DLIB_MODBUS_MCU)

target_compile_options(libmodbus PUBLIC "-Wno-unused-variable")

target_link_libraries(libmodbus libmodbusport)

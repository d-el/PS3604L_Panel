add_library(utils STATIC
	src/crc.c
	src/plog.c
	src/timegm.c
	)

target_include_directories(utils PUBLIC
	include
	)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/version.h @ONLY)

target_link_libraries(utils freertos iqmathlib-cm4)

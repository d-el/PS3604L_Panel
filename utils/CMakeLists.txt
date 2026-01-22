add_library(utils STATIC
	src/crc.c
	src/plog.c
	src/timegm.c
	)

target_include_directories(utils PUBLIC
	include
	)

target_link_libraries(utils freertos iqmathlib-cm4)

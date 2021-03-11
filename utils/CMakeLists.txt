add_library(utils STATIC
	src/crc.c
	src/plog.c
	src/specificMath.c
	)

target_include_directories(utils PUBLIC
	include
	)

target_link_libraries(utils freertos drivers iqmathlib-cm4)

add_library(dev STATIC
	src/display.c
	src/graphics.c
	src/24AAxx.c
	)

target_include_directories(dev PUBLIC
	include
	)

target_link_libraries(dev freertos drivers utils)

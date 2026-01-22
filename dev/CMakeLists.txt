add_library(dev STATIC
	src/24AAxx.cpp
	src/st7735.c
	)

target_include_directories(dev
	PUBLIC
		include
	PRIVATE
		include/dev
	)

target_link_libraries(dev freertos hal driver utils)

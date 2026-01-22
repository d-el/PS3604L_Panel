add_library(driver STATIC
	src/i2c
	)

target_include_directories(driver PUBLIC
	PUBLIC
		include
	PRIVATE
		include/driver
	)

target_link_libraries(driver freertos hal utils)

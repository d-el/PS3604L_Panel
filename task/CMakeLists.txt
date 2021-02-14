add_library(task STATIC
	src/monitorTSK.c
	src/regulatorConnTSK.c
	src/systemTSK.cpp
	)

target_include_directories(task PUBLIC
	include
	)

target_link_libraries(task freertos drivers utils lwip systems prmsystem window net libmodbus)

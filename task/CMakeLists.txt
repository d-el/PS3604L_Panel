add_library(task STATIC
	src/monitorTSK.c
	src/systemTSK.cpp
	)

target_include_directories(task PUBLIC
	include
	)

target_link_libraries(task freertos hal utils lwip systems prmsystem window net regulator)

target_link_options(task PUBLIC -Wl,--require-defined=vApplicationMallocFailedHook)

add_library(task STATIC
	src/monitorTSK.c
	src/regulatorConnTSK.c
	src/systemTSK.c
	)

target_include_directories(task PUBLIC
	include
	PRIVATE ../app/inc
	)

target_link_libraries(task freertos drivers utils lwip systems prmsystem window net)

add_library(window STATIC
	src/arial.c
	src/baseTSK.c
	src/chargeTSK.c
	src/dsegBold.c
	src/dSegBold8x14.c
	src/font6x8.c
	src/font8x12.c
	src/ico.c
	src/imageLogo.c
	src/menuSystem.c
	src/settingTSK.c
	src/startupTSK.c
	src/ui.c
	)

target_include_directories(window PUBLIC
	include
	PRIVATE ../app/inc
	)

target_link_libraries(window freertos drivers dev utils)

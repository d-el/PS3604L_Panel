add_library(window STATIC
	src/arial.c
	src/baseTSK.cpp
	src/chargeTSK.cpp
	src/display.cpp
	src/dsegBold.c
	src/dSegBold8x14.c
	src/font6x8.c
	src/font8x12.c
	src/footer.cpp
	src/ico.c
	src/imageLogo.c
	src/menuSystem.cpp
	src/settingTSK.cpp
	src/startupTSK.cpp
	src/ui.cpp
	)

target_include_directories(window PUBLIC
	include
	PRIVATE ../app/inc
	)

target_link_libraries(window freertos hal dev utils lwip regulator task)

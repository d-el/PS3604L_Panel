add_library(hal STATIC
	src/beep.c
	src/clock.c
	src/cm4.c
	src/drivers.c
	src/enco.c
	src/flash.c
	src/gpio.c
	src/htimer.c
	src/i2c.c
	src/key.c
	src/ledpwm.c
	src/pvd.c
	src/rng.c
	src/rtc.c
	src/spi.c
	src/stm32f4x7_eth_bsp.c
	src/stm32f4x7_eth.c
	src/sysTimeMeas.c
	src/uart.c
	src/uniqueDeviceId.c
	)

target_include_directories(hal
	PUBLIC
		include
	PRIVATE
		include/hal
	)

target_link_libraries(hal cmsis)

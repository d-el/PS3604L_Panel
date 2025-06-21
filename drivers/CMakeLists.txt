add_library(drivers STATIC
	src/beep.c
	src/clock.c
	src/cm4.c
	src/drivers.c
	src/enco.c
	src/flash.c
	src/gpio.c
	src/i2c.c
	src/key.c
	src/ledpwm.c
	src/pvd.c
	src/rng.c
	src/rtc.c
	src/spi.c
	src/stm32f4x7_eth_bsp.c
	src/stm32f4x7_eth.c
	src/uart.c
	src/uniqueDeviceId.c
	)

target_include_directories(drivers PUBLIC
	include
	)

target_link_libraries(drivers cmsis freertos task)

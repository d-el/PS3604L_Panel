add_library(cmsis STATIC
	Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c
	)

target_include_directories(cmsis PUBLIC
	Include
	Device/ST/STM32F4xx/Include
	)

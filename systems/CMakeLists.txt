add_library(systems STATIC
	src/debugCore.c
	src/semihosting.c
	src/startup_stm32f407xx.c
	src/syscall.c
	src/sysTimeMeas.c
	src/write.c
	)

target_include_directories(systems PUBLIC
	include
	)

target_link_libraries(systems drivers)

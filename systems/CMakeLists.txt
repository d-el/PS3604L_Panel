add_library(systems STATIC
	src/debugCore.c
	src/memmgr.c
	src/semihosting.c
	src/startup_stm32f407xx.c
	src/syscall.c
	src/sysTimeMeas.c
	src/write.c
	)

target_include_directories(systems PUBLIC
	include
	)

target_link_libraries(systems drivers freertos)

target_link_options(systems PUBLIC
	-u HardFault_Handler
	-Wl,-u malloc
	-u_write
	-Wl,--wrap,_malloc
	-Wl,--wrap,_free
	-Wl,--wrap,_calloc
	-Wl,--wrap,_realloc
	-Wl,--wrap,_malloc_r
	-Wl,--wrap,_free_r
	-Wl,--wrap,_calloc_r
	-Wl,--wrap,_realloc_r
	)

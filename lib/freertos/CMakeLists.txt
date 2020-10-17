add_library(freertos STATIC
	src/croutine.c
	src/event_groups.c
	src/heap_4.c
	src/list.c
	src/port.c
	src/queue.c
	src/tasks.c
	src/timers.c
	)

target_include_directories(freertos
	PUBLIC include
	)

target_link_libraries(freertos cmsis config)

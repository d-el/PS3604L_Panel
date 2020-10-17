cmake_minimum_required(VERSION 3.6)

# Embedded files
set(PROJECT_EMBED_OBJ_FILES "")
foreach(FILENAME ${PROJECT_EMBED_FILES})
	get_filename_component(FILENAME_ONLY ${FILENAME} NAME)
	get_filename_component(FILEPATH_ONLY ${FILENAME} PATH)
	add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME}.o 
			COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/${FILEPATH_ONLY}
			WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
			COMMAND ${CMAKE_OBJCOPY} 
			ARGS -I binary -O elf32-littlearm -B arm --rename-section .data=.rodata ${FILENAME} ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME}.o
			DEPENDS ${FILENAME}
		)
	list(APPEND PROJECT_EMBED_OBJ_FILES ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME}.o)
	message(STATUS "Objcopy'ing ${FILENAME}")
	message(STATUS "${CMAKE_CURRENT_BINARY_DIR}/${FILENAME}.o")
	message(STATUS "${CMAKE_OBJCOPY} ")
endforeach(FILENAME)

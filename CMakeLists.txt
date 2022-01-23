cmake_minimum_required(VERSION 3.6)

project(PS3604LF
	VERSION 2.2.3
	LANGUAGES CXX C
	)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

add_compile_options(
	-DSTM32F407xx
	)

add_compile_options(
	-g3 -O2
	-fno-exceptions
	-fmessage-length=0
	-ffunction-sections
	-fdata-sections
	-fsigned-char
	-fsingle-precision-constant
	-Wfloat-equal
	-Wuninitialized
	-Wextra
	-Wall
	
	$<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
	)

add_link_options(
	-Wl,--gc-sections
	-Wl,--print-memory-usage
	-Wl,--undefined=uxTopUsedPriority
	-Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.map
	)

configure_file(utils/include/version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/utils/include/version.h @ONLY)

set(PROJECT_EMBED_FILES
	net/resource/404.html
	net/resource/favico.bmp
	net/resource/index.html
	net/resource/overall.js
	net/resource/styles.css
	)
include(embeddedfiles.cmake)

add_subdirectory(config)
add_subdirectory(dev)
add_subdirectory(drivers)
add_subdirectory(lib/CMSIS)
add_subdirectory(lib/freertos)
add_subdirectory(lib/IQmathLib-cm4)
add_subdirectory(lib/lwip)
add_subdirectory(libmodbus)
add_subdirectory(libmodbusport)
add_subdirectory(net)
add_subdirectory(prmsystem)
add_subdirectory(regulator)
add_subdirectory(systems)
add_subdirectory(task)
add_subdirectory(utils)
add_subdirectory(window)

set(SOURCE_FILES
	main.cpp
	)

add_executable(${PROJECT_NAME}.elf ${SOURCE_FILES} ${PROJECT_EMBED_OBJ_FILES})

target_link_libraries(${PROJECT_NAME}.elf
	systems
	)

set(NAME ${PROJECT_NAME}_${PROJECT_VERSION})
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
	BYPRODUCTS ${NAME}.hex
	COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${NAME}.hex
	
	BYPRODUCTS ${NAME}.bin
	COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${NAME}.bin
	
	COMMENT "Generate hex and bin"
	)

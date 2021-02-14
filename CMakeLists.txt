cmake_minimum_required(VERSION 3.6)

project(PS3604LF CXX C)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

set(CPUFLAGS "\
	-mcpu=cortex-m4 \
	-march=armv7e-m \
	-mthumb \
	-mfloat-abi=hard \
	-mfpu=fpv4-sp-d16 \
	")

set(COMMONFLAGS "\
	-g3 -O2 \
	-fmessage-length=0 \
	-ffunction-sections \
	-fdata-sections \
	-fsigned-char \
	-fsingle-precision-constant \
	-Wfloat-equal \
	-Wuninitialized \
	-Wextra \
	-Wall \
	")

add_compile_options(-DSTM32F407xx)

set(CMAKE_CXX_FLAGS "${CPUFLAGS} ${COMMONFLAGS} -fno-rtti -fno-exceptions")
set(CMAKE_C_FLAGS "${CPUFLAGS} ${COMMONFLAGS}")

set(FUSED "\
	-u _IQ14mpy \
	-u _IQ14div \
	")

set(CMAKE_EXE_LINKER_FLAGS "\
	${CPUFLAGS} \
	${FUSED} \
	-T ${CMAKE_CURRENT_SOURCE_DIR}/systems/STM32F407VETx_FLASH.ld \
	-Wl,--gc-sections \
	-Xlinker --gc-sections --specs=nano.specs \
	-Wl,--print-memory-usage \
	-Wl,--undefined=uxTopUsedPriority \
	-Wl,-Map=${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.map \
	${CMAKE_CURRENT_SOURCE_DIR}/lib/IQmathLib-cm4/IQmathLib-cm4.a \
	 ")

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

# Additional command which will run after the above from a different directory
add_custom_command(TARGET ${PROJECT_NAME}.elf PRE_BUILD
	COMMAND ${PROJECT_SOURCE_DIR}/scripts/vgen.sh utils/src/version.c
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

set(HEX_FILE ${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.bin)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
	COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
	COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
	COMMAND ${CMAKE_SIZE} --format=berkeley $<TARGET_FILE:${PROJECT_NAME}.elf>
	COMMENT "Building ${HEX_FILE} \nBuilding ${BIN_FILE}"
	)

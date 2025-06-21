SET(CMAKE_SYSTEM arm-cortex-m4)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-m4)

set(CMAKE_ASM_COMPILER	arm-none-eabi-gcc)
set(CMAKE_C_COMPILER	arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER	arm-none-eabi-g++)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)


set(CPUFLAGS "-mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16")

set(CMAKE_CXX_FLAGS ${CPUFLAGS} CACHE INTERNAL "")
set(CMAKE_C_FLAGS ${CPUFLAGS} CACHE INTERNAL "")

set(CMAKE_EXE_LINKER_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/systems/stub.ld -Wl,--gc-sections --specs=nano.specs"
	CACHE INTERNAL ""
	)

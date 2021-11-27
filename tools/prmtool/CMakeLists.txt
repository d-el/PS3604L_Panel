cmake_minimum_required (VERSION 2.6)

project (prmtool CXX)
set(CMAKE_CXX_STANDARD 17)

set(COMMONFLAGS "\
	-g3 -Og \
	")
	
set(CMAKE_CXX_FLAGS "${CPUFLAGS} ${COMMONFLAGS} -std=c++17")
set(CMAKE_C_FLAGS "${CPUFLAGS} ${COMMONFLAGS} -std=gnu11")

add_executable(prmtool
	prmtool.cpp
	)

target_link_libraries(prmtool
	jsoncpp
	)

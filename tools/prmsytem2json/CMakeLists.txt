cmake_minimum_required(VERSION 3.14)

project(prmsytem2json CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(jsoncpp REQUIRED)

set(COMMONFLAGS "\
	-g3 -Og \
	")
	
set(CMAKE_CXX_FLAGS "${CPUFLAGS} ${COMMONFLAGS} -std=c++17")
set(CMAKE_C_FLAGS "${CPUFLAGS} ${COMMONFLAGS} -std=gnu11")

add_executable(prmsytem2json
	gen.cpp
	)

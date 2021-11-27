include(ExternalProject)

add_library(prmsystem STATIC
	src/prmSystem.cpp
	src/prmSystemCallback.cpp
	)

target_include_directories(prmsystem PUBLIC
	include
	generated
	)

ExternalProject_Add(prmsytem2json
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prmsytem2json
	PREFIX ${CMAKE_CURRENT_BINARY_DIR}/tools
	BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/tools/
	INSTALL_COMMAND ${CMAKE_COMMAND} -E true
	)
	
set(PARAMETER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../config/parametres.txt)

add_custom_target(prmsystemobj
	COMMAND ${CMAKE_COMMAND} -E make_directory "generated" 

	BYPRODUCTS generated/parameter.dcl
	COMMAND ${CMAKE_CURRENT_BINARY_DIR}/tools/prmsytem2json ${PARAMETER_FILE} generated/parameter.dcl type
	
	BYPRODUCTS generated/parameter.def
	COMMAND ${CMAKE_CURRENT_BINARY_DIR}/tools/prmsytem2json ${PARAMETER_FILE} generated/parameter.def object
	
	BYPRODUCTS generated/parameter.json
	COMMAND ${CMAKE_CURRENT_BINARY_DIR}/tools/prmsytem2json ${PARAMETER_FILE} generated/parameter.json json
	
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	DEPENDS ${PARAMETER_FILE}
	DEPENDS prmsytem2json
	COMMENT "Generate dcl, object, json from ${PARAMETER_FILE}"
)

add_custom_target(parameters.json
	BYPRODUCTS ${PROJECT_SOURCE_DIR}/parameters.json
	COMMAND ${PROJECT_SOURCE_DIR}/tools/prmsytem2json/build/prmsytem2json include/parametres.txt ${PROJECT_SOURCE_DIR}/parameters.json json
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	DEPENDS prmsytem2json
	DEPENDS include/parametres.txt
	COMMENT "Generate parameters.json from parametres.h"
)

add_dependencies(prmsystem prmsystemobj)

target_link_libraries(prmsystem utils)

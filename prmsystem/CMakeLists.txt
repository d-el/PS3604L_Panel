add_library(prmsystem STATIC
	src/prmSystem.cpp
	src/prmSystemCallback.cpp
	)

target_include_directories(prmsystem PUBLIC
	include
	generated
	)

add_custom_command(
	OUTPUT prmsytem2json
	COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prmsytem2json/build.sh
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	)

set(PARAMETER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../config/parametres.txt)

add_custom_target(prmsystemobj
	BYPRODUCTS ${CMAKE_CURRENT_SOURCE_DIR}/generated/parameter.dcl
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prmsytem2json/build/prmsytem2json ${PARAMETER_FILE}
    	${CMAKE_CURRENT_SOURCE_DIR}/generated/parameter.dcl type
	
	BYPRODUCTS ${CMAKE_CURRENT_SOURCE_DIR}/generated/parameter.def
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prmsytem2json/build/prmsytem2json ${PARAMETER_FILE}
    	${CMAKE_CURRENT_SOURCE_DIR}/generated/parameter.def object
	
	BYPRODUCTS ${CMAKE_CURRENT_SOURCE_DIR}/generated/parameter.json
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prmsytem2json/build/prmsytem2json ${PARAMETER_FILE}
    	${CMAKE_CURRENT_SOURCE_DIR}/generated/parameter.json json
	
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	DEPENDS ${PARAMETER_FILE}
	DEPENDS prmsytem2json
	COMMENT "Generate parametrestype.h from parametres.h"
)

add_custom_target(parameters.json
	BYPRODUCTS ${PROJECT_SOURCE_DIR}/parameters.json
    COMMAND ${PROJECT_SOURCE_DIR}/tools/prmsytem2json/build/prmsytem2json ${CMAKE_CURRENT_SOURCE_DIR}/include/parametres.txt ${PROJECT_SOURCE_DIR}/parameters.json json
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	DEPENDS prmsytem2json
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include/parametres.txt
	COMMENT "Generate parameters.json from parametres.h"
)

add_dependencies(prmsystem prmsystemobj)
	
target_link_libraries(prmsystem utils)

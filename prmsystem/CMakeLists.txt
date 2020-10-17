add_library(prmsystem STATIC
	src/prmEditor.c
	src/prmSystem.c
	)

target_include_directories(prmsystem PUBLIC
	include
	)

target_link_libraries(prmsystem utils)

add_library(freemodbus STATIC
	modbus/mb.c

	modbus/functions/mbfunccoils.c
	modbus/functions/mbfuncdiag.c
	modbus/functions/mbfuncdisc.c
	modbus/functions/mbfuncfile.c
	modbus/functions/mbfuncholding.c
	modbus/functions/mbfuncinput.c
	modbus/functions/mbfuncother.c
	modbus/functions/mbutils.c
	
	modbus/rtu/mbcrc.c
	modbus/rtu/mbrtu.c
	modbus/tcp/mbtcp.c
	)

target_include_directories(freemodbus 
	PUBLIC 
		modbus/include
	PRIVATE 
		modbus/ascii
		modbus/rtu
		modbus/tcp
	)

target_link_libraries(freemodbus portfreemodbus)
	
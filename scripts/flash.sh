#!/bin/bash

hexfile=${HEX:-$(dirname "$0")/../build/PS3604LF.hex}
echo "Hex file: ${hexfile}"

debugger=${DEBUGGER:-openocd-jlink}
echo "Debugger: ${debugger}"

$(dirname "$0")/gdb-serv.sh $debugger -c "reset halt" -c "flash write_image erase $hexfile" -c " flash verify_image $hexfile" -c "reset" -c shutdown

#!/bin/bash

debugger=${DEBUGGER:-openocd-jlink}
$(dirname "$0")/gdb-serv.sh $debugger -c "reset halt" -c " stm32f2x mass_erase 0" -c "reset" -c shutdown

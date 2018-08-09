#!/bin/bash

print_usage () {
    echo "usage: gdb_serv_start.sh <metod>[openocd_stlink, openocd_jlink, jlink]"
    exit 1
}

if [ "$#" -ne 1 ]; then
    print_usage
fi

if [ "$1" == "openocd_stlink" ]; then
    echo "start" $1
    openocd -f openocd_stlink.cfg

elif [ "$1" == "openocd_jlink" ]; then
    echo "start" $1
    openocd -f openocd_jlink.cfg

elif [ "$1" == "jlink" ]; then
    echo "start" $1
    FLAG="
    -device STM32F407VG
    -If SWD
    -speed auto
    -port 1234
    -SWOPort 1235
    -TelnetPort 1236
    -nolocalhostonly
    "
    echo $FLAG
    JLinkGDBServer $FLAG

else
    print_usage
fi

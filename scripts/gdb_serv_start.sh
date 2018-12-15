#!/bin/bash

# save current dir
cwd=$(pwd)
# cd to script dir
DIR="$(cd "$(dirname "$0")" && pwd)"
cd $DIR

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
    while read line; do    
        FLAG+='-'$line' '
    done < jlink.cfg
    echo $FLAG
    JLinkGDBServer $FLAG

else
    print_usage
fi

# restore current dir
cd $cwd

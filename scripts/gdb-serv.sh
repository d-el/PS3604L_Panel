#!/bin/bash

function usage()
{
	cat << HEREDOC
	Usage: $progname <openocd-stlink, openocd-jlink, openocd-mculink, jlink>"
HEREDOC
}

if [ "$#" -lt 1 ]; then
	usage
fi

echo "start" $1 "${@:2}"
if [ "$1" == "openocd-stlink" ]; then
	openocd -f interface/stlink.cfg -c "transport select hla_swd" -f $(dirname "$0")/openocd.cfg "${@:2}"

elif [ "$1" == "openocd-jlink" ]; then
	openocd -f interface/jlink.cfg -c "transport select swd" -f $(dirname "$0")/openocd.cfg "${@:2}"

elif [ "$1" == "openocd-mculink" ]; then
	openocd -f interface/cmsis-dap.cfg -c "transport select swd" -f $(dirname "$0")/openocd.cfg "${@:2}"

else
	usage
fi

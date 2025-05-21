#!/bin/bash

function usage()
{
	cat << HEREDOC
	Usage: $progname <openocd-stlink, openocd-jlink, openocd-mculink"
HEREDOC
}

if [ "$#" -lt 1 ]; then
	debugger=${DEBUGGER:-openocd-jlink}
else
	debugger=$1
fi

openocdcfg=${OPENOCDCFG:-$(dirname "$0")/openocd.cfg}

echo "Debugger: ${debugger}"
echo "Config: ${openocdcfg}"
echo "start" $1 "${@:2}"

if [ ${debugger} == "openocd-stlink" ]; then
	openocd -f interface/stlink-dap.cfg -c "transport select dapdirect_swd" -f ${openocdcfg} "${@:2}"

elif [ ${debugger} == "openocd-jlink" ]; then
	openocd -f interface/jlink.cfg -c "transport select swd" -f ${openocdcfg} "${@:2}"

elif [ ${debugger} == "openocd-mculink" ]; then
	openocd -f interface/cmsis-dap.cfg -c "transport select swd" -f ${openocdcfg} "${@:2}"

else
	usage
fi

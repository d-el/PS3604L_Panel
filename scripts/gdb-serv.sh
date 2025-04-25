#!/bin/bash

function usage()
{
	cat << HEREDOC
	Usage: $progname <openocd-stlink, openocd-jlink, openocd-mculink"
HEREDOC
}

if [ "$#" -lt 1 ]; then
	usage
fi

openocdcfg=${OPENOCDCFG:-$(dirname "$0")/openocd.cfg}
echo "Use config: ${openocdcfg}"

echo "start" $1 "${@:2}"
if [ "$1" == "openocd-stlink" ]; then
	openocd -f interface/stlink-dap.cfg -c "transport select dapdirect_swd" -f ${openocdcfg} "${@:2}"

elif [ "$1" == "openocd-jlink" ]; then
	openocd -f interface/jlink.cfg -c "transport select swd" -f ${openocdcfg} "${@:2}"

elif [ "$1" == "openocd-mculink" ]; then
	openocd -f interface/cmsis-dap.cfg -c "transport select swd" -f ${openocdcfg} "${@:2}"

else
	usage
fi

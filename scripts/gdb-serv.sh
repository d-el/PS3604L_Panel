#!/bin/bash

function usage()
{
	cat << HEREDOC
	Usage: $progname <openocd-stlink, openocd-jlink, openocd-mculink, jlink>"
HEREDOC
}

# save current dir
cwd=$(pwd)
# cd to script dir
DIR="$(cd "$(dirname "$0")" && pwd)"
cd $DIR

if [ "$#" -lt 1 ]; then
	usage
fi

echo "start" $1 "${@:2}"
if [ "$1" == "openocd-stlink" ]; then
	openocd -f interface/stlink.cfg -c "transport select hla_swd" -f openocd.cfg "${@:2}"

elif [ "$1" == "openocd-jlink" ]; then
	openocd -f interface/jlink.cfg -c "transport select swd" -f openocd.cfg "${@:2}"

elif [ "$1" == "openocd-mculink" ]; then
	openocd -f interface/cmsis-dap.cfg -c "transport select swd" -f openocd.cfg "${@:2}"
	
elif [ "$1" == "jlink" ]; then
	while read line; do
		FLAG+='-'$line' '
	done < jlink.cfg
	echo $FLAG
	JLinkGDBServer $FLAG

else
	usage
fi

# restore current dir
cd $cwd

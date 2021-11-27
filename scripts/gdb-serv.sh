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

if [ "$#" -ne 1 ]; then
	print_usage
fi

if [ "$1" == "openocd-stlink" ]; then
	echo "start" $1
	openocd -f openocd-stlink.cfg

elif [ "$1" == "openocd-jlink" ]; then
	echo "start" $1
	openocd -f openocd-jlink.cfg

elif [ "$1" == "openocd-mculink" ]; then
	echo "start" $1
	openocd -f openocd-mculink.cfg
	
elif [ "$1" == "jlink" ]; then
	echo "start" $1
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

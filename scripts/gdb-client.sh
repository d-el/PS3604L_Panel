#!/bin/bash

function usage()
{
	cat << HEREDOC
	Usage: $progname [-h|--help] [-g|--gdbinitfile file]]
	optional arguments:
	-h, --help			show this help message and exit
	-g, --gdbinitfile	set GDB init file
HEREDOC
}

# save current dir
cwd=$(pwd)
# cd to script dir
DIR="$(cd "$(dirname "$0")" && pwd)"
cd $DIR

GDBINITFILE=gdbinit
GDB=arm-none-eabi-gdb
ELF=../build/PS3604LF.elf

progname=$(basename $0)
OPTS=$(getopt -o "hg:" --long "help,gdbinitfile:" -n "$progname" -- "$@")
if [ $? != 0 ] ; then echo "Error in command line arguments." >&2 ; usage; exit 1 ; fi
eval set -- "$OPTS"

while true; do
	case "$1" in
	-h | --help 		) usage; exit; ;;
	-g | --gdbinitfile  ) GDBINITFILE=$cwd/"$2"; shift 2 ;;
	--			 		) shift; break ;;
	*					) break ;;
	esac
done

$GDB -x $GDBINITFILE $ELF

# restore current dir
cd $cwd

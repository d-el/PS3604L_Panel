#!/bin/bash

link=`cat $1`
crc=$2
size=$3

echo "CRC=$crc, size=$size"

link=${link/CALC_CRC=0x00000000/CALC_CRC=0x$crc}
link=${link/IMAGE_SIZE=00000000/IMAGE_SIZE=$size}
echo "${link}"
eval "${link}"

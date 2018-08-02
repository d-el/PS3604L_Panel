#!/bin/bash

cd "$(dirname "$0")"
arm-none-eabi-gdb -x gdb.init ../build/PS3604LF.elf

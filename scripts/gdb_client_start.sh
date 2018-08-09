#!/bin/bash

cd "$(dirname "$0")"
arm-none-eabi-gdb -x gdbinit ../build/PS3604LF.elf

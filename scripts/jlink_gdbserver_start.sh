#!/bin/bash

FLAG="
    -device STM32
    -If SWD
    -speed auto
    -port 1234
    -SWOPort 1235
    -TelnetPort 1236
    -nolocalhostonly
    "

echo $FLAG
JLinkGDBServerCL $FLAG

# PS3604LF

### Overview
This repository include the source code of the front panel Power Supply [PS3604L](https://github.com/d-el/PS3604L).  
MCU: STM32F407VG.

### Requirements
toolchain arm-none-eabi 10.3 or higher  
gcc / g++ 7.5.0 or higher  
cmake 3.14 or higher  
libjsoncpp-dev  
crc32  
xPack xPack OpenOCD v0.12.0-7 or higher   

### Build project
>mkdir build  
>cd build  
>cmake .. -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake  
>make -j8  

#### Flash bootloader
>./scripts/flashb.sh openocd-jlink

#### Flash application
>./scripts/flash.sh openocd-jlink

#### Start debug server
>./scripts/gdb-serv.sh openocd-jlink

#### Start debug client
>./scripts/gdb-client.sh

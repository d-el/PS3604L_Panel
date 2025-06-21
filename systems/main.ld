INCLUDE memory.ld

MEMORY
{
    FLASH (rx)      : ORIGIN = _main_flash_start, LENGTH = _main_flash_size
    RAM (xrw)       : ORIGIN = _ram_start, LENGTH = _ram_size
    CCMRAM (rw)     : ORIGIN = 0x10000000, LENGTH = 64K
}

INCLUDE infosection.ld
INCLUDE common_cortexm.ld

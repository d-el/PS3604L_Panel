INCLUDE memory.ld

MEMORY
{
    FLASH (rx)      : ORIGIN = _boot_flash_start, LENGTH = _boot_flash_size
    RAM (xrw)       : ORIGIN = _ram_start, LENGTH = _ram_size
    CCMRAM (rw)     : ORIGIN = 0x10000000, LENGTH = 64K
}

INCLUDE common_cortexm.ld

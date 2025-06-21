_boot_flash_start = 0x08000000;
_boot_flash_size = 32K;

_main_flash_start = 0x08020000;
_main_flash_size = 384K;
_main_exe_offset = 1024;

_fwstorage_flash_start = 0x08080000;
_fwstorage_flash_size = 384K;

_ram_start = 0x20000000;
_ram_size = 128K;

_estack = 0x20020000;       /* end of RAM */
_Min_Heap_Size = 0x0000;    /* required amount of heap  */
_Min_Stack_Size = 0x800;    /* required amount of stack */

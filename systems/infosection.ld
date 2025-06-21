_header_magic = 0x38272585;

SECTIONS
{
  .imageheader :
  {
    . = ALIGN(4);
    _imageheader_start = .;
    LONG(CALC_CRC);
    LONG(IMAGE_SIZE);
    LONG(_header_magic);
    LONG(V_MAJOR);
    LONG(V_MINOR);
    LONG(V_PATCH);
    LONG(_main_exe_offset);
    . = ALIGN(_main_exe_offset);
  } >FLASH =0xFF
}

/*******************************************************************************
 */

/* Entry Point */
ENTRY(Reset_Handler)

SECTIONS
{
  .isr_vector (READONLY):
  {
    . = ALIGN(4);
    _isr_vector_start = .;        /* create a global symbol at vector start */
    KEEP(*(.isr_vector)) /* Startup code */
    . = ALIGN(4);
  } >FLASH =0xFF

  /* The program code and other data goes into FLASH */
  .text (READONLY):
  {
    . = ALIGN(4);
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)
    KEEP (*(.init))
    KEEP (*(.fini))
    . = ALIGN(8);
    _etext = .;        /* define a global symbols at end of code */
  } >FLASH =0xFF

  /* Constant data goes into FLASH */
  .rodata (READONLY):
  {
    . = ALIGN(4);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(4);
  } >FLASH =0xFF

  .ARM.extab (READONLY):
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } >FLASH =0xFF

  .ARM (READONLY):
  {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } >FLASH =0xFF

  .preinit_array (READONLY):
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >FLASH =0xFF
  
  .init_array (READONLY):
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >FLASH =0xFF
  
  .fini_array (READONLY):
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >FLASH =0xFF

  _sidata = LOADADDR(.data); /* used by the startup to initialize data */
  .data :   /* Initialized data sections goes into RAM, load LMA copy after code */
  {
    . = ALIGN(4);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */
    . = ALIGN(4);
    _edata = .;        /* define a global symbol at data end */
  } >RAM AT>FLASH =0xFF

  /* used by the startup to initialize ccmdata */
  _siccmdata = LOADADDR(.ccmdata);
  
  /* The program code and other data goes into CCM RAM */
  .ccmdata : 
  {
    . = ALIGN(4);
    _sccmdata = .;
    *(.ccmdata) 
    *(.ccmdata*)
    . = ALIGN(4);
    _eccmdata = .;
  } >CCMRAM AT> FLASH

  . = ALIGN(4); /* Uninitialized data section */
  .bss :
  {
    . = ALIGN(4);
    _sbss = .;         /* define a global symbol at bss start */
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
  } >RAM

  /* Uninitialized data section on CCM RAM */
  .ccmrambss : 
  {
    . = ALIGN(4);
    _sccmbss = .;
    *(.ccmbss) 
    *(.ccmbss*)
    . = ALIGN(4);
    _eccmbss = .;
  } >CCMRAM

  ._user_heap_stack : /* User_heap_stack section, used to check that there is enough RAM left */
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >RAM

  /* Remove information from the standard libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 :
  {
    *(.ARM.attributes)
  }
}

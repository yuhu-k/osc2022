SECTIONS
{
  . = 0x20000EC;
  .text :
  {
    KEEP(*(.text.boot))
    *(.text)
  }
  . = ALIGN(4096);
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(4);
  }
  .data :
  {
    *(.data)
    . = ALIGN(4096);
  }

  .bss (NOLOAD) :
  {
    __bss_start = .;

    *(.bss)

    __bss_end = .;
  }
  . = ALIGN(4096);
  .stack (NOLOAD):
  {
    __stack_end = .;

    . = . + 4096;

    __stack_start = .;
  }
  . = ALIGN(4096);
  __sp__ = .;
  . = . + 8;
  __lr__ = .;
  . = . + 8;
  . = ALIGN(4096);

}

SECTIONS
{
  .text :
  {
    KEEP(*(.text.boot))
    *(.text)
  }
  . = ALIGN(4096);

  .data :
  {
    *(.data)
  }
  . = ALIGN(4096);

  .bss (NOLOAD) :
  {
    __bss_start = .;

    *(.bss)

    __bss_end = .;
  }
  . = ALIGN(4096);
  __sp__ = .;
  . = . + 8;
  __lr__ = .;
  . = . + 8;
  . = ALIGN(4096);

}

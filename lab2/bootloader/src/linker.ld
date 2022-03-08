SECTIONS
{
  . = 0x80000;
  .relocate :
  {
    KEEP(*(.text.relocate))
    *(.text.relocator)
  }
  . = ALIGN(4096);

  _begin_ = .;
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
  _end_ = .;


}

__bss_size = (__bss_end - __bss_start) >> 3;
__relocation = 0x01080000;


SECTIONS
{
  . = 0x80000;
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

  _lr_addr_ = .;
  . = . + 8;
  . = ALIGN(4096);
  _end_ = .;


}

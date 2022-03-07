__heap_size = 0x100;;
__stack_size = 0x800;;


SECTIONS
{
  . = 0x80000;
  .relocate :{
    KEEP(*(.text.allocate))
    *(.allocator)
  }
  _begin = .;
  .text :
  {
    KEEP(*(.text.boot))
    *(.text)
  }
  .data :
  {
    *(.data)
  }
  .bss ALIGN(16) (NOLOAD) :
  {
    __bss_start = .;

    *(.bss)

    __bss_end = .;
  }
  . = ALIGN(8);
  .heap (NOLOAD):
    {
        . = . + 8;
        __end__ = .;
        end = __end__;
        *(.heap*)
        __HeapLimit = .;
    }
}

__bss_size = (__bss_end - __bss_start) >> 3;
__load_kernel_location = 0x01000000;
__heap_start = end - 8;

ENTRY(start)

SECTIONS
{
    _KERN_VMA = 0xfffffff800000000;

    . = _KERN_VMA;

    .text ALIGN(0x1000) :
    {
        *(.text)
    }

    .rodata ALIGN(0x1000) :
    {
        *(.rodata*)
    }

    .data ALIGN(0x1000) :
    {
        *(.data)
    }

    .bss ALIGN(0x1000) :
    {
        *(.bss)
    }

    .stack ALIGN(0x1000) :
    {
        _KERN_STACK_START = .;
        . += 0x1000;
        _KERN_STACK_END = .;
    }
}
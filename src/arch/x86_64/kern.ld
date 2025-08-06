ENTRY(start)

SECTIONS
{
    _KERN_LMA = 0x200000;
    _KERN_VMA = 0xffffffff80000000;

    . = _KERN_VMA;

    .text ALIGN(0x1000) : AT(ADDR(.text) - _KERN_VMA + _KERN_LMA)
    {
        *(.text)
    }

    .rodata ALIGN(0x1000) : AT(ADDR(.rodata) - _KERN_VMA + _KERN_LMA)
    {
        *(.rodata*)
    }

    .data ALIGN(0x1000) : AT(ADDR(.data) - _KERN_VMA + _KERN_LMA)
    {
        *(.data)
    }

    .bss ALIGN(0x1000) : AT(ADDR(.bss) - _KERN_VMA + _KERN_LMA)
    {
        *(.bss)
    }

    .stack ALIGN(0x1000) : AT(ADDR(.stack) - _KERN_VMA + _KERN_LMA)
    {
        _KERN_STACK_START = .;
        . += 0x1000;
        _KERN_STACK_END = .;
    }
}
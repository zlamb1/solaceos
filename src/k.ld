ENTRY(kmain)

SECTIONS
{
    _KERNEL_LMA = 0x100000;
    _KERNEL_VMA = 0xffffffff80000000;

    . = _KERNEL_VMA;

    .text ALIGN(0x1000) :
    {
        *(.text)
    }

    .data ALIGN(0x1000) :
    {
        *(.data)
    }

    .rodata ALIGN(0x1000) :
    {
        *(.rodata*)
    }

    .bss ALIGN(0x1000) :
    {
        *(.bss)
    }
}
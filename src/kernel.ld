ENTRY(KernelMain)

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

    .init_array ALIGN(0x8) :
    {
        PROVIDE(__preinit_array_start = .);
        KEEP(*(.preinit_array.*))
        KEEP(*(.preinit_array))
        PROVIDE(__preinit_array_end = .);

        . = ALIGN(0x8);

        PROVIDE(__init_array_start = .);
        KEEP(*(.init_array.*))
        KEEP(*(.init_array))
        PROVIDE(__init_array_end = .);
    }

    .bss ALIGN(0x1000) :
    {
        *(.bss)
    }
}
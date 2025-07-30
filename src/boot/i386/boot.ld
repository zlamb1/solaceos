ENTRY(start)

SECTIONS
{
    . = 0x100000;

    _KERN_TEXT_START = .;

    .boot ALIGN(8) :
    {
        *(.boot)
    }

    .text ALIGN(8) :
    {
        *(.text)
    }

    .rodata ALIGN(8) :
    {
        *(.rodata*)
    }

    .data ALIGN(8) :
    {
        *(.data)
    }

    _KERN_DATA_END = .;

    .bss ALIGN(8) :
    {
        *(.bss)
    }

    _KERN_END_BSS = .;
}
ENTRY(start)

SECTIONS
{
    . = 0x100000;

    .boot ALIGN(CONSTANT(MAXPAGESIZE)) :
    {
        _KERN_TEXT_START = .;
        *(.boot)
    }

    .text ALIGN(CONSTANT(MAXPAGESIZE)) :
    {
        *(.text)
        _KERN_TEXT_END = .;
    }

    .rodata ALIGN(CONSTANT(MAXPAGESIZE)) :
    {
        _KERN_RODATA_START = .;
        *(.rodata*)
        _KERN_RODATA_END = .;
    }

    .data ALIGN(CONSTANT(MAXPAGESIZE)) :
    {
        _KERN_DATA_START = .;
        *(.data)
        _KERN_DATA_END = .;
    }

    .bss ALIGN(CONSTANT(MAXPAGESIZE)) :
    {
        _KERN_BSS_START = .;
        *(.bss)
        _KERN_BSS_END = .;
    }
}
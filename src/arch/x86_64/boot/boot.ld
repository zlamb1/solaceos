ENTRY(start)

SECTIONS
{
    _KERN_LMA = 0x100000;
    _KERN_VMA = 0xffff800000000000;

    . = _KERN_LMA;

    .boot ALIGN(CONSTANT(MAXPAGESIZE)) :
    {
        _KERN_BOOT_START = .;
        *(.boot)
        *(.boot.data)
        _KERN_BOOT_END = .;
    }

    . += _KERN_VMA;

    .text ALIGN(CONSTANT(MAXPAGESIZE)) : AT(ADDR(.text) - _KERN_VMA)
    {
        _KERN_TEXT_START = .;
        *(.text)
        _KERN_TEXT_END = .;
    }

    .rodata ALIGN(CONSTANT(MAXPAGESIZE)) : AT(ADDR(.rodata) - _KERN_VMA)
    {
        _KERN_RODATA_START = .;
        *(.rodata*)
        _KERN_RODATA_END = .;
    }

    .data ALIGN(CONSTANT(MAXPAGESIZE)) : AT(ADDR(.data) - _KERN_VMA)
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

    # initial stack space used before allocation
    .stack ALIGN(CONSTANT(MAXPAGESIZE)) : AT(ADDR(.stack) - _KERN_VMA)
    {
        _KERN_INITSTACK_START = .;
        . += 1024;
        _KERN_INITSTACK_END = .;
    }

    _MB1_LOAD_ADDR = _KERN_BOOT_START;
    _MB1_LOAD_END_ADDR = _KERN_DATA_END - _KERN_VMA;
    _MB1_BSS_END_ADDR = _KERN_BSS_END - _KERN_VMA;

    _KERN_INITSTACK_LMA = _KERN_INITSTACK_START - _KERN_VMA;
}
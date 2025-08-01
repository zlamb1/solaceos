ENTRY(start)

SECTIONS
{
    _KERN_LMA = 0x100000;
    _KERN_VMA = 0xffff800000000000;

    . = _KERN_LMA;

    .boot ALIGN(0x1000) :
    {
        _KERN_BOOT_START = .;
        *(.boot)
    }

    .boot.data ALIGN(0x1000) :
    {
        *(.boot.data)
        _KERN_BOOT_END = .;
    }

    . = ALIGN(0x1000);

    . = _KERN_VMA + (_KERN_BOOT_END - _KERN_LMA);

    .text ALIGN(0x1000) : AT(ADDR(.text) - _KERN_VMA + _KERN_LMA)
    {
        _KERN_TEXT_START = .;
        *(.text)
        _KERN_TEXT_END = .;
    }

    .rodata ALIGN(0x1000) : AT(ADDR(.rodata) - _KERN_VMA + _KERN_LMA)
    {
        _KERN_RODATA_START = .;
        *(.rodata*)
        _KERN_RODATA_END = .;
    }

    .data ALIGN(0x1000) : AT(ADDR(.data) - _KERN_VMA + _KERN_LMA)
    {
        _KERN_DATA_START = .;
        *(.data)
        _KERN_DATA_END = .;
    }

    .bss ALIGN(0x1000) : AT(ADDR(.bss) - _KERN_VMA + _KERN_LMA)
    {
        _KERN_BSS_START = .;
        *(.bss)
        _KERN_BSS_END = .;
    }

    # initial stack space used before allocation
    .stack ALIGN(0x1000) : AT(ADDR(.stack) - _KERN_VMA + _KERN_LMA)
    {
        _KERN_INITSTACK_START = .;
        . += 0x1000;
        _KERN_INITSTACK_END = .;
    }

    _MB1_LOAD_ADDR = _KERN_BOOT_START;
    _MB1_LOAD_END_ADDR = _KERN_DATA_END - _KERN_VMA + _KERN_LMA;
    _MB1_BSS_END_ADDR = _KERN_BSS_END - _KERN_VMA + _KERN_LMA;

    _KERN_LMA_END = _KERN_INITSTACK_END - _KERN_VMA + _KERN_LMA;
    _KERN_INITSTACK_LMA = _KERN_INITSTACK_START - _KERN_VMA + _KERN_LMA;
}
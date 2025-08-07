ENTRY(start)

SECTIONS
{
    _LDR_LMA = 0x100000;

    . = _LDR_LMA;
    
    _LDR_START = .;

    .boot ALIGN(0x1000) :
    {
        _LDR_BOOT_START = .;
        *(.boot)
    }

    .text ALIGN(0x1000) :
    {
        _LDR_TEXT_START = .;
        *(.text)
        _LDR_TEXT_END = .;
    }

    .rodata ALIGN(0x1000) :
    {
        _LDR_RODATA_START = .;
        *(.rodata*)
        _LDR_RODATA_END = .;
    }

    .data ALIGN(0x1000) :
    {
        _LDR_DATA_START = .;
        *(.data)
        _LDR_DATA_END = .;
    }

    .bss ALIGN(0x1000) :
    {
        _LDR_BSS_START = .;
        *(.bss)
        _LDR_BSS_END = .;
    }

    .stack ALIGN(0x1000) :
    {
        _LDR_STACK_START = .;
        . += 0x4000;
        _LDR_STACK_END = .;
    }

    _LDR_END = .;

    _MB1_LOAD_ADDR = _LDR_BOOT_START;
    _MB1_LOAD_END_ADDR = _LDR_DATA_END;
    _MB1_BSS_END_ADDR = _LDR_STACK_END;
}
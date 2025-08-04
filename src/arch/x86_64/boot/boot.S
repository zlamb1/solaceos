.code32
.section .boot, "ax"

#define MB1_MAGIC 0x1BADB002
#define MB1_FLAGS 0x00010002

#define COLS 80
#define ROWS 25

#define BG_FG 0x0F

.global start

.extern _MB1_LOAD_ADDR
.extern _MB1_LOAD_END_ADDR
.extern _MB1_BSS_END_ADDR

.extern _LDR_STACK_END

.extern lmain

mb1_magic: 
    .long MB1_MAGIC
mb1_flags:
    .long MB1_FLAGS
mb1_checksum:
    .long -(MB1_MAGIC + MB1_FLAGS)
mb1_header_addr:
    .long mb1_magic
mb1_load_addr:
    .long _MB1_LOAD_ADDR
mb1_load_end_addr:
    .long _MB1_LOAD_END_ADDR
mb1_bss_end_addr:
    .long _MB1_BSS_END_ADDR
mb1_entry_attr:
    .long start
mb1_width:
    .long 0
mb1_height:
    .long 0
mb1_depth:
    .long 0

clr:
    ret

puts:
    mov $0xB8000, %ecx
1:
    lodsb
    cmp $0, %al
    je 2f 
    movb %al, (%ecx)
    movb $BG_FG, 1(%ecx)
    add $2, %ecx
    jmp 1b
2:
    ret

start:
    cmp $0x2BADB002, %eax
    jne mb1_fail

    mov $_LDR_STACK_END, %esp

    call clr

    cld
    mov $early_boot_msg, %esi

    lgdt gdtr
    ljmp $0x8, $set_cs

set_cs:
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %fs
    mov %ax, %es
    mov %ax, %gs
    mov %ax, %ss

    # pass mb1 info
    mov %ebx, %eax
    cld
    jmp lmain

loop:
    hlt
    jmp loop

mb1_fail:
    mov $mb1_fail_error, %esi
    call puts
    jmp loop

.data

early_boot_msg:
.asciz "Solace32: Boot Entry"

mb1_fail_error:
    .asciz "Invalid Multiboot1 Signature"

gdt_start:    
    # null descriptor
    .quad 0
gdt_code32:
    .short 0xFFFF
    .short 0
    .byte 0
    .byte 0b10011011
    .byte 0b11001111
    .byte 0
gdt_data32:
    .short 0xFFFF
    .short 0
    .byte 0
    .byte 0b10010011
    .byte 0b11001111
    .byte 0
gdt_code64:
    .short 0xFFFF
    .short 0
    .byte 0
    .byte 0b10011011
    .byte 0b10101111
    .byte 0
gdt_data64:
    .short 0xFFFF
    .short 0
    .byte 0
    .byte 0b10010011
    .byte 0b10101111
    .byte 0
gdt_end:

gdtr:
    gdtr_size: 
        .short gdt_end - gdt_start - 1
    gdtr_offset:
        .long gdt_start
        .long 0
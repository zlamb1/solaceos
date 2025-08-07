.code32
.section .boot, "ax"

#define MB1_MAGIC 0x1BADB002
#define MB1_FLAGS 0x00010002

#define COLS 80
#define ROWS 25

#define BG_FG 0x0F

.global start
.global enter_kernel

.extern _MB1_LOAD_ADDR
.extern _MB1_LOAD_END_ADDR
.extern _MB1_BSS_END_ADDR

.extern _LDR_STACK_END

.extern lmain
.extern kprintf

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

enter_kernel:
    push %ecx
    push %edx

    mov %cr4, %edi
    or $32, %edi
    mov %edi, %cr4

    mov %eax, %cr3

    mov %esp, %ebp
    sub $4, %esp
    and $-16, %esp

    movl $pae_msg, (%esp)
    call kprintf

    mov $0xC0000080, %ecx
    rdmsr
    or $256, %eax
    wrmsr

    mov %cr0, %eax
    or $0x80000001, %eax
    mov %eax, %cr0

    movl $paging_msg, (%esp)
    call kprintf

    mov %ebp, %esp
    
    pop %edx
    pop %ecx

    ljmp $0x18, $enter_kernel64

.code64
enter_kernel64:
    mov $0x20, %ax
    mov %ax, %ds
    mov %ax, %fs
    mov %ax, %es
    mov %ax, %gs
    mov %ax, %ss

    movb $0x10, 0xb8001
    
    shl $32, %rcx
    mov $0xFFFFFFFF, %rax
    andq %rax, %rdx
    or %rdx, %rcx

    jmp *%rcx

.data

early_boot_msg:
.asciz "Solace32: Boot Entry"

pae_msg:
.asciz "Solace32: PAE Enabled\n"

paging_msg:
.asciz "Solace32: Paging Enabled\n"

test_msg:
.asciz "Solace32: Test: %#llb\n"

mb1_fail_error:
    .asciz "Invalid Multiboot1 Signature"

.align 16

.global gdt_start

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
    .short 0
    .short 0
    .byte 0
    .byte 0b10010011
    .byte 0b11000000
    .byte 0
gdt_end:

.global gdtr_offset

gdtr:
    gdtr_size: 
        .short gdt_end - gdt_start - 1
    gdtr_offset:
        .long gdt_start
        .long 0
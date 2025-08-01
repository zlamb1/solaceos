.code32
.section .boot, "ax"

#define MB1_MAGIC 0x1BADB002
#define MB1_FLAGS 0x00010002

#define COLS 80
#define ROWS 25

#define BG_FG 0x0F

.global start

.extern _KERN_LMA
.extern _KERN_VMA

.extern _MB1_LOAD_ADDR
.extern _MB1_LOAD_END_ADDR
.extern _MB1_BSS_END_ADDR

.extern _KERN_INITSTACK_LMA

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

disable_cursor:
    mov $0xA, %al
    mov $0x3D4, %dx
    outb %al, %dx
    movw $0x3D5, %dx
    inb %dx, %al
    orb $0x20, %al
    outb %al, %dx
    ret

clr:
    mov $0xB8000, %ecx
1:
    cmp $0xB8FA0, %ecx
    jge 2f
    movb $0, (%ecx)
    movb $BG_FG, 1(%ecx)
    add $2, %ecx
    jmp 1b
2:
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

    mov $_KERN_INITSTACK_LMA, %esp

    call disable_cursor
    call clr

    cld
    mov $boot_str, %esi
    call puts

    lgdt gdtr
    ljmp $0x8, $set_cs

set_cs:
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %fs
    mov %ax, %es
    mov %ax, %gs
    mov %ax, %ss

# set up pml4t entry
    mov $pdptBoot, %eax
    and $-0x1000, %eax
    or $3, %eax
    mov %eax, pml4tBoot

# set up pdpt entry
    mov $pdtBoot, %eax
    and $-0x1000, %eax
    or $3, %eax
    mov %eax, pdptBoot

# set up pdt entry
    mov $ptBoot, %eax
    and $-0x1000, %eax
    or $3, %eax
    mov %eax, pdtBoot

# set up first page table
    mov $ptBoot, %ecx
    mov $0, %edx
1:
    cmp $ptBootEnd, %ecx
    jge 2f
    mov %edx, %eax
    or $3, %eax
    mov %eax, (%ecx)
    add $8, %ecx
    add $0x1000, %edx
    jmp 1b
2:

# enable paging
    mov $pml4tBoot, %eax
    mov %eax, %cr3

    mov %cr4, %eax
    or $0x20, %eax
    mov %eax, %cr4

    mov $0xC0000080, %ecx
    rdmsr
    or $0x100, %eax
    wrmsr

    mov %cr0, %eax
    or $0x80000001, %eax
    mov %eax, %cr0

    ljmp $0x18, $boot64

loop:
    hlt
    jmp loop

mb1_fail:
    mov $mb1_fail_error, %esi
    call puts
    jmp loop

.code64

boot64:
    mov $0x20, %ax
    mov %ax, %ds
    mov %ax, %fs
    mov %ax, %es
    mov %ax, %gs
    mov %ax, %ss

    mov $pdptBootVMA, %rax
    and $-0x1000, %rax
    or $3, %rax
    mov %rax, pml4tBootVMA

    mov $pdtBootVMA, %rax
    and $-0x1000, %rax
    or $3, %rax
    mov %rax, pdptBootVMA

    mov $ptBootVMA, %rax
    and $-0x1000, %rax
    or $3, %rax
    mov %rax, pdtBootVMA

    mov $ptBootVMA, %ecx
    mov $_KERN_LMA, %edx
1:
    cmp $ptBootEndVMA, %ecx
    jge 2f
    mov %edx, %eax
    or $3, %eax
    mov %eax, (%ecx)
    add $8, %ecx
    add $0x1000, %edx
    jmp 1b
2:

# flush TLB
    mov $pml4tBoot, %rax
    mov %rax, %cr3

.extern kmain

    # pass MB1 info pointer
    mov %rbx, %rdi

    movabs $kmain, %rax
    jmp *%rax

.section .boot.data, "aw"

pml4tBoot:
.rept 256
.quad 0
.endr
pml4tBootVMA:
.rept 256
.quad 0
.endr

pdptBoot:
.rept 512
.quad 0
.endr

pdptBootVMA:
.rept 512
.quad 0
.endr

pdtBoot:
.rept 512
.quad 0
.endr

pdtBootVMA:
.rept 512
.quad 0
.endr

ptBoot:
.rept 512
.quad 0
.endr
ptBootEnd:

ptBootVMA:
.rept 512
.quad 0
.endr
ptBootEndVMA:

boot_str:
.asciz "Setting Up C Environment..."

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
        .quad gdt_start
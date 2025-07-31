.code32
.section .boot, "ax"

#define MB1_MAGIC 0x1BADB002
#define MB1_FLAGS 0x00010002

#define COLS 80
#define ROWS 25

#define BG_FG 0x0F

.global start

.extern _KERN_TEXT_START
.extern _KERN_DATA_END
.extern _KERN_BSS_END

mb1_magic: 
    .long MB1_MAGIC
mb1_flags:
    .long MB1_FLAGS
mb1_checksum:
    .long -(MB1_MAGIC + MB1_FLAGS)
mb1_header_addr:
    .long mb1_magic
mb1_load_addr:
    .long _KERN_TEXT_START
mb1_load_end_addr:
    .long _KERN_DATA_END
mb1_bss_end_addr:
    .long _KERN_BSS_END
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

    lgdt gdtr
    ljmp $0x8, $set_cs
set_cs:
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %fs
    mov %ax, %es
    mov %ax, %gs
    mov %ax, %ss

    call disable_cursor
    call clr

    cld
    mov $boot_str, %esi
    call puts

loop:
    jmp loop

mb1_fail:
    mov $mb1_fail_error, %esi
    call puts
    jmp loop

.data

boot_str:
.asciz "Booting Kernel..."

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
gdt_end:

gdtr:
    gdtr_size: 
        .short gdt_end - gdt_start - 1
    gdtr_offset:
        .quad gdt_start

.code64
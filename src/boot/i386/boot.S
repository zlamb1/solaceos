.code32
.section .boot, "ax"

#define MB1_MAGIC 0x1BADB002
#define MB1_FLAGS 0x00010002

.global start

.extern _KERN_TEXT_START
.extern _KERN_DATA_END
.extern _KERN_END_BSS

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
    .long _KERN_END_BSS
mb1_entry_attr:
    .long start
mb1_width:
    .long 0
mb1_height:
    .long 0
mb1_depth:
    .long 0

start:
    movb $'H', (0xB8000)
    movb $0xFF, (0xB8001)

loop:
    jmp loop

.code64
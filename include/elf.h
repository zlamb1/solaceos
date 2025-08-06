#ifndef ELF_H
#define ELF_H 1

#include <stddef.h>
#include <stdint.h>

#include "errno.h"

#ifndef __maybe_unused
#define __maybe_unused
#endif

#define EI_MAG0       0x00
#define EI_MAG1       0x01
#define EI_MAG2       0x02
#define EI_MAG3       0x03
#define EI_CLASS      0x04
#define EI_DATA       0x05
#define EI_VERSION    0x06
#define EI_OSABI      0x07
#define EI_ABIVERSION 0x08
#define EI_PAD        0x09

typedef enum
{
  EI_CLASS_32 = 1,
  EI_CLASS_64 = 2
} elf_class_t;

typedef enum
{
  EI_DATA_LE = 1,
  EI_DATA_BE = 2
} elf_data_t;

typedef enum
{
  EI_OSABI_SYSV = 0x00,
  EI_OSABI_HPUX = 0x01,
  EI_OSABI_NETBSD = 0x02,
  EI_OSABI_LINUX = 0x03,
  EI_OSABI_GNU_HURD = 0x04,
  EI_OSABI_SOLARIS = 0x06,
  EI_OSABI_AIX = 0x07,
  EI_OSABI_IRIX = 0x08,
  EI_OSABI_FREEBSD = 0x09,
  EI_OSABI_TRU64 = 0x0A,
  EI_OSABI_NOVELL_MODESTO = 0x0B,
  EI_OSABI_OPENBSD = 0x0C,
  EI_OSABI_OPENVMS = 0x0D,
  EI_OSABI_NONSTOP_KERNEL = 0x0E,
  EI_OSABI_AROS = 0x0F,
  EI_OSABI_FENIXOS = 0x10,
  EI_OSABI_NUXI_CLOUDABI = 0x11,
  EI_OSABI_STRATUS_TECHNOLOGIES_OPENVOS = 0x12,
} elf_osabi_t;

typedef enum
{
  ET_NONE = 0,
  ET_REL = 1,
  ET_EXEC = 2,
  ET_DYN = 3,
  ET_CORE = 4,
} elf_type_t;

typedef enum
{
  EI_MACHINE_NONE = 0x00,
  EI_MACHINE_SPARC = 0x02,
  EI_MACHINE_X86 = 0x03,
  EI_MACHINE_MIPS = 0x08,
  EI_MACHINE_POWERPC = 0x14,
  EI_MACHINE_ARM = 0x28,
  EI_MACHINE_SUPERH = 0x2A,
  EI_MACHINE_IA64 = 0x32,
  EI_MACHINE_X86_64 = 0x3E,
  EI_MACHINE_AARCH64 = 0xB7
} elf_machine_t;

typedef enum
{
  PT_NULL = 0x00000000,
  PT_LOAD = 0x00000001,
  PT_DYN = 0x00000002,
  PT_INTERP = 0x00000003,
  PT_NOTE = 0x00000004,
  PT_SHLIB = 0x00000005,
  PT_PHDR = 0x00000006,
  PT_TLS = 0x00000007,
  PT_LOOS = 0x60000000,
  PT_HIOS = 0x6FFFFFFF,
  PT_LOPROC = 0x70000000,
  PT_HIPROC = 0x7FFFFFFF
} elf_program_type_t;

typedef enum
{
  PF_X = 0x1,
  PF_W = 0x2,
  PF_R = 0x4
} elf_pf_t;

typedef enum
{
  SHT_NULL = 0x0,
  SHT_PROGBITS = 0x1,
  SHT_SYMTAB = 0x2,
  SHT_STRTAB = 0x3,
  SHT_RELA = 0x4,
  SHT_HASH = 0x5,
  SHT_DYNAMIC = 0x6,
  SHT_NOTE = 0x7,
  SHT_NOBITS = 0x8,
  SHT_REL = 0x9,
  SHT_SHLIB = 0xA,
  SHT_DYNSYM = 0xB,
  SHT_INIT_ARRAY = 0xE,
  SHT_FINI_ARRAY = 0xF,
  SHT_PREINIT_ARRAY = 0x10,
  SHT_GROUP = 0x11,
  SHT_SYMBTAB_SHNDX = 0x12,
  SHT_NUM = 0x13,
} elf_sht_t;

typedef enum
{
  SHF_WRITE = 0x1,
  SHF_ALLOC = 0x2,
  SHF_EXECINSTR = 0x4,
  SHF_MERGE = 0x10,
  SHF_STRINGS = 0x20,
  SHF_LINK = 0x40,
  SHF_LINK_ORDER = 0x80,
  SHF_OS_NONCONFORMING = 0x100,
  SHF_GROUP = 0x200,
  SHF_TLS = 0x400,
  SHF_MASKOS = 0x0FF00000,
  SHF_MASKPROC = 0xF0000000,
  SHF_ORDERED = 0x4000000,
  SHF_EXCLUDE = 0x8000000
} elf_shf_t;

typedef struct
{
  unsigned char e_ident[16];
  uint16_t e_type;
  uint16_t e_machine;
  uint32_t e_version;
  uint32_t e_entry;
  uint32_t e_phoff;
  uint32_t e_shoff;
  uint32_t e_flags;
  uint16_t e_ehsize;
  uint16_t e_phentsize;
  uint16_t e_phnum;
  uint16_t e_shentsize;
  uint16_t e_shnum;
  uint16_t e_shstrndx;
} __attribute__ ((packed)) elf_hdr_32_t;

typedef struct
{
  unsigned char e_ident[16];
  uint16_t e_type;
  uint16_t e_machine;
  uint32_t e_version;
  uint64_t e_entry;
  uint64_t e_phoff;
  uint64_t e_shoff;
  uint32_t e_flags;
  uint16_t e_ehsize;
  uint16_t e_phentsize;
  uint16_t e_phnum;
  uint16_t e_shentsize;
  uint16_t e_shnum;
  uint16_t e_shstrndx;
} __attribute__ ((packed)) elf_hdr_64_t;

typedef struct
{
  uint32_t p_type;
  uint32_t p_offset;
  uint32_t p_vaddr;
  uint32_t p_paddr;
  uint32_t p_filesz;
  uint32_t p_memsz;
  uint32_t p_flags;
  uint32_t p_align;
} __attribute__ ((packed)) elf_ph_32_t;

typedef struct
{
  uint32_t p_type;
  uint32_t p_flags;
  uint64_t p_offset;
  uint64_t p_vaddr;
  uint64_t p_paddr;
  uint64_t p_filesz;
  uint64_t p_memsz;
  uint64_t p_align;
} __attribute__ ((packed)) elf_ph_64_t;

typedef struct
{
  uint32_t sh_name;
  uint32_t sh_type;
  uint32_t sh_flags;
  uint32_t sh_addr;
  uint32_t sh_offset;
  uint32_t sh_size;
  uint32_t sh_link;
  uint32_t sh_info;
  uint32_t sh_addralign;
  uint32_t sh_entsize;
} __attribute__ ((packed)) elf_sh_32_t;

typedef struct
{
  uint32_t sh_name;
  uint32_t sh_type;
  uint64_t sh_flags;
  uint64_t sh_addr;
  uint64_t sh_offset;
  uint64_t sh_size;
  uint32_t sh_link;
  uint32_t sh_info;
  uint64_t sh_addralign;
  uint64_t sh_entsize;
} __attribute__ ((packed)) elf_sh_64_t;

[[maybe_unused]] static int
elf_validate_hdr (void *hdr, elf_class_t class, elf_type_t type,
                  elf_machine_t machine, const char **msg)
{
  /* first few bytes are the same */
  elf_hdr_32_t *hdr32 = (elf_hdr_32_t *) hdr;
  if (hdr == NULL)
    {
      if (msg != NULL)
        *msg = "Null ELF Header";
      return -EINVAL;
    }
  if (hdr32->e_ident[EI_MAG0] != 0x7F || hdr32->e_ident[EI_MAG1] != 'E'
      || hdr32->e_ident[EI_MAG2] != 'L' || hdr32->e_ident[EI_MAG3] != 'F')
    {
      if (msg != NULL)
        *msg = "Invalid ELF Magic";
      return -EINVAL;
    }
  if (hdr32->e_ident[EI_CLASS] != class)
    {
      if (msg != NULL)
        *msg = "Invalid ELF Class";
      return -EINVAL;
    }
  if (hdr32->e_type != type)
    {
      if (msg != NULL)
        *msg = "Invalid ELF Type";
      return -EINVAL;
    }
  if (hdr32->e_machine != machine)
    {
      if (msg != NULL)
        *msg = "Invalid ELF Machine";
      return -EINVAL;
    }
  return 0;
}

int elf_arch_validate_hdr (void *hdr, const char **msg);

#endif
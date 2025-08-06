#include <stddef.h>

#include "elf.h"

int
elf_arch_validate_hdr (void *hdr, const char **msg)
{
  return elf_validate_hdr (hdr, EI_CLASS_64, ET_EXEC, EI_MACHINE_X86_64, msg);
}
#include "kprint.h"
#include "ldr/boot.h"
#include "palloc.h"

void __attribute__ ((noreturn)) kmain (boot_info_t *bi);

void
kmain (boot_info_t *bi)
{
  initprint (get_static_console (bi->boot_mode));
  kprintf ("Solace: Booting...\n");
  palloc_init (bi);
  for (;;)
    ;
}
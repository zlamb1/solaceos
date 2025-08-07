#include "kprint.h"
#include "ldr/boot.h"
#include "vga.h"

#ifndef VADDR
#define VADDR 0xffff800000000000
#endif

void __attribute__ ((noreturn)) kmain (boot_info_t *bi);

static vga_console_t vga;

void
kmain (boot_info_t *bi)
{
  (void) bi;
  vga = vga_console_init ((void *) (VADDR + 0xb8000), 80, 25, 160);
  initprint (&vga.base);
  kprintf ("Solace: Booting...\n");
  for (;;)
    ;
}
#include "kprint.h"
#include "mb1.h"
#include "vga.h"

void __attribute__ ((noreturn)) kmain (mb1_info_t *info);

static vga_console_t vga;

void
kmain (mb1_info_t *mb1_info)
{
  (void) mb1_info;
  vga = vga_console_init ((void *) (0xb8000), 80, 25, 160);
  initprint (&vga.base);
  kprintf ("Solace: Booting...\n");
  for (;;)
    ;
}
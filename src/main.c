#include <stddef.h>

#include "datetime.h"
#include "limine/limine.h"
#include "panic.h"
#include "print.h"

LIMINE_REQUESTS_START_MARKER

static struct limine_bootloader_info_request bl_request
    = LIMINE_BOOTLOADER_INFO_REQUEST;
static struct limine_executable_cmdline_request cmdline_request
    = LIMINE_EXECUTABLE_CMDLINE_REQUEST;
static struct limine_framebuffer_request fb_request
    = LIMINE_FRAMEBUFFER_REQUEST;
static struct limine_date_at_boot_request date_request
    = LIMINE_DATE_AT_BOOT_REQUEST;

LIMINE_REQUESTS_END_MARKER

__attribute__ ((noreturn)) void kmain (void);

static void
kbooted_by (void)
{
  struct limine_bootloader_info_response *res = bl_request.response;
  kprintln ("Booted By %s [%s]", res->name, res->version);
}

static void
kboot_time (void)
{
  struct datetime dt  = get_datetime (date_request.response->timestamp);
  u32 hour            = dt.hour % 12;
  u32 minute          = dt.minute;
  const char *segment = dt.hour >= 12 ? "PM" : "AM";

  if (minute > 9)
    kprintln ("Booted At %u:%u %s %u/%u/%u UTC", hour ? hour : 12, minute,
              segment, dt.month + 1, dt.day_of_month + 1, dt.year);
  else
    kprintln ("Booted At %u:0%u %s %u/%u/%u UTC", hour ? hour : 12, minute,
              segment, dt.month + 1, dt.day_of_month + 1, dt.year);
}

void
kmain (void)
{
  kprint_init (fb_request.response);

  kprintln ("Starting SolaceOS...");
  kbooted_by ();
  kboot_time ();
  kprintln ("Booted With Arguments: '%s'", cmdline_request.response->cmdline);

  for (;;)
    ;
}

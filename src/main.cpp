#include "abi.h"
#include "compiler.h"
#include "gfx/vconsole.hpp"
#include "io.hpp"
#include "limine/limine.h"

LIMINE_REQUESTS_START_MARKER

static struct limine_framebuffer_request fb_request =
    LIMINE_FRAMEBUFFER_REQUEST;

LIMINE_REQUESTS_END_MARKER

extern "C" NORETURN void KernelMain(void);

static Option<Gfx::VirtualConsole> vc;

void KernelMain(void) {
  InvokeConstructors();

  IO::Log << "Booting SolaceOS...\n";

  if (fb_request.response && fb_request.response->framebuffer_count) {
    limine_framebuffer *lfb = fb_request.response->framebuffers[0];

    vc = Gfx::VirtualConsole::Create(lfb);

    if (vc.hasValue()) {
      IO::Log << "Initializing Virtual Console...\n";
      IO::Log.AddConsoleDevice(&vc.unwrap());
    }
  }

  IO::PrintLn("Test: %u", 123);

  for (;;)
    ;
}
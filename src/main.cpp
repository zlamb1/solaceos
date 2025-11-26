#include "compiler.h"
#include "gfx/vcon.hpp"
#include "io.hpp"
#include "limine/limine.h"

LIMINE_REQUESTS_START_MARKER

static struct limine_framebuffer_request fb_request =
    LIMINE_FRAMEBUFFER_REQUEST;

LIMINE_REQUESTS_END_MARKER

extern "C" NORETURN void KernelMain(void);

static Option<Gfx::VirtualConsole> vc;

void KernelMain(void) {
  if (fb_request.response && fb_request.response->framebuffer_count) {
    limine_framebuffer *lfb = fb_request.response->framebuffers[0];

    vc = Gfx::VirtualConsole::Create(lfb);

    if (vc.hasValue()) {
      IO::Log.SetConsoleDevice(&vc.unwrap());
    }
  }

  IO::Log << "Booting Kernel..." << IO::EndLine;

  for (;;)
    ;
}
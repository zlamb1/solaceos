#pragma once

#include "gfx/fb.hpp"
#include "gfx/font.hpp"
#include "io/console.hpp"
#include "limine/limine.h"
#include "option.hpp"

namespace Gfx {

class VirtualConsole : public IO::ConsoleDevice {
  friend class Option;

public:
  static Option<VirtualConsole> Create(limine_framebuffer *fb);

  void ReverseCursor(void);
  void AdvanceCursor(void);

protected:
  VirtualConsole(Framebuffer fb, BitmapFont font);

  virtual void Flush() override;
  virtual void Write(const char *str, usize len) override;

  Framebuffer m_fb;
  BitmapFont m_font;

  u32 m_width, m_height;
  u16 m_x, m_y;
};

} // namespace Gfx

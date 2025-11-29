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

  virtual void WriteUnlocked(const char *str, usize len) override;

  inline VirtualConsole(const VirtualConsole &console)
      : m_fb(console.m_fb), m_font(console.m_font), m_width(console.m_width),
        m_height(console.m_height), m_x(console.m_x), m_y(console.m_y) {}

protected:
  VirtualConsole(Framebuffer fb, BitmapFont font);

  Framebuffer m_fb;
  BitmapFont m_font;

  u32 m_width, m_height;
  u16 m_x, m_y;
};

} // namespace Gfx

#pragma once

#include "con/con.hpp"
#include "gfx/fb.hpp"
#include "gfx/font.hpp"
#include "limine/limine.h"
#include "option.hpp"

namespace Gfx {

class VirtualConsole : public IO::ConsoleDevice {
public:
  static Option<VirtualConsole> Create(limine_framebuffer *fb);

  void ReverseCursor(void);
  void AdvanceCursor(void);

  virtual void WriteByte(char byte) override;
  virtual void WriteBuffer(const char *buf, usize len) override;
  virtual void Flush() override;

protected:
  VirtualConsole(Framebuffer fb, BitmapFont font);

  Framebuffer fb;
  BitmapFont font;

  u32 width, height;
  u16 cx, cy;
};

} // namespace Gfx

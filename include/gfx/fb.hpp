#pragma once

#include "common.h"
#include "limine/limine.h"
#include "option.hpp"

namespace Gfx {

namespace Color {

struct RGB {
  u8 r, g, b;
};

UNUSED static RGB BLACK = {0, 0, 0};
UNUSED static RGB WHITE = {255, 255, 255};

} // namespace Color

class Framebuffer {
public:
  static Option<Framebuffer> Create(limine_framebuffer *fb);

  inline u64 GetWidth() const { return width; }
  inline u64 GetHeight() const { return height; }

  void FillRect(u64 rectX, u64 rectY, u64 rectWidth, u64 rectHeight,
                Color::RGB color);

  void DrawBitmap(const u8 *bitmap, u64 bitmapWidth, u64 bitmapHeight,
                  u64 rowStride, u64 dstX, u64 dstY, bool msbOrder,
                  Color::RGB fg, Color::RGB bg);

protected:
  Framebuffer(limine_framebuffer *fb);

  volatile u8 *address;
  u64 width, height, pitch;
  u16 bpp;
  u8 red_mask_size, red_mask_shift;
  u8 green_mask_size, green_mask_shift;
  u8 blue_mask_size, blue_mask_shift;
};

} // namespace Gfx
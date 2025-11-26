#include "gfx/fb.hpp"

namespace Gfx {

Option<Framebuffer> Framebuffer::Create(limine_framebuffer *fb) {
  if (fb->red_mask_size > 8 || fb->green_mask_size > 8 ||
      fb->blue_mask_size > 8)
    return Option<Framebuffer>::None();

  if (fb->red_mask_shift > 31 || fb->green_mask_shift > 31 ||
      fb->blue_mask_shift > 31)
    return Option<Framebuffer>::None();

  return Some(Framebuffer(fb));
}

void Framebuffer::FillRect(u64 rectX, u64 rectY, u64 rectWidth, u64 rectHeight,
                           Color::RGB color) {
  if (rectX >= width || rectY >= height)
    return;

  if (width - rectX < rectWidth)
    rectWidth = width - rectX;

  if (height - rectY < rectHeight)
    rectHeight = height - rectY;

  u16 bytesPerPixel = bpp >> 3;
  u32 _color        = ((color.r >> (8 - red_mask_size)) << red_mask_shift) |
               ((color.g >> (8 - green_mask_size)) << green_mask_shift) |
               ((color.b >> (8 - blue_mask_size)) << blue_mask_shift);

  volatile u8 *row = (volatile u8 *) address;
  row += rectX * bytesPerPixel;

  for (u64 y = 0; y < rectHeight; ++y) {
    for (u64 x = 0; x < rectWidth; ++x) {
      ((volatile u32 *) row)[x] = _color;
    }
    row += pitch;
  }
}

void Framebuffer::DrawBitmap(const u8 *bitmap, u64 bitmapWidth,
                             u64 bitmapHeight, u64 rowStride, u64 dstX,
                             u64 dstY, bool msbOrder, Color::RGB fg,
                             Color::RGB bg) {
  if (dstX >= width || dstY >= height)
    return;

  if (width - dstX < bitmapWidth)
    bitmapWidth = width - dstX;

  if (height - dstY < bitmapHeight)
    bitmapHeight = height - dstY;

  const u16 bytesPerPixel = bpp >> 3;

  const u32 bgColor = ((bg.r >> (8 - red_mask_size)) << red_mask_shift) |
                      ((bg.g >> (8 - green_mask_size)) << green_mask_shift) |
                      ((bg.b >> (8 - blue_mask_size)) << blue_mask_shift);

  const u32 fgColor = ((fg.r >> (8 - red_mask_size)) << red_mask_shift) |
                      ((fg.g >> (8 - green_mask_size)) << green_mask_shift) |
                      ((fg.b >> (8 - blue_mask_size)) << blue_mask_shift);

  auto isSet = [&bitmap, &rowStride, &msbOrder](u64 x, u64 y) {
    u8 byte = bitmap[y * rowStride + (x >> 3)], bit = x & 7;
    return (byte >> (msbOrder ? 7 - bit : bit)) & 1;
  };

  for (u64 y = 0; y < bitmapHeight; ++y)
    for (u64 x = 0; x < bitmapWidth; ++x) {
      u64 index = (y + dstY) * pitch + (x + dstX) * bytesPerPixel;
      u32 color = isSet(x, y) ? fgColor : bgColor;
      for (u16 i = 0; i < bytesPerPixel; ++i) {
        address[index + i] = color;
        color >>= 8;
      }
    }
}

Framebuffer::Framebuffer(limine_framebuffer *fb) {
  address          = (volatile u8 *) fb->address;
  width            = fb->width;
  height           = fb->height;
  pitch            = fb->pitch;
  bpp              = fb->bpp;
  red_mask_size    = fb->red_mask_size;
  red_mask_shift   = fb->red_mask_shift;
  green_mask_size  = fb->green_mask_size;
  green_mask_shift = fb->green_mask_shift;
  blue_mask_size   = fb->blue_mask_size;
  blue_mask_shift  = fb->blue_mask_shift;
}

} // namespace Gfx
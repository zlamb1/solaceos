#include "gfx/vcon.hpp"
#include "gfx/psf.hpp"
#include "option.hpp"

extern char _binary_consolefont_psf_start;
extern char _binary_consolefont_psf_end;

namespace Gfx {

Option<VirtualConsole> VirtualConsole::Create(limine_framebuffer *fb) {
  if (fb == nullptr)
    return Option<VirtualConsole>::None();

  auto optFB = Framebuffer::Create(fb);

  if (!optFB.hasValue())
    return Option<VirtualConsole>::None();

  auto start = reinterpret_cast<uintptr_t>(&_binary_consolefont_psf_start);
  auto end   = reinterpret_cast<uintptr_t>(&_binary_consolefont_psf_end);

  if (start > end) {
    // TODO: panic/assert
    return Option<VirtualConsole>::None();
  }

  auto optFont = LoadPSFFont(reinterpret_cast<void *>(start), end - start);

  if (!optFont.hasValue())
    return Option<VirtualConsole>::None();

  auto font = optFont.unwrap();

  if (!font.ValidateFont())
    return Option<VirtualConsole>::None();

  auto _fb = optFB.unwrap();

  if (font.GetGlyphWidth() > _fb.GetWidth() ||
      font.GetGlyphHeight() > _fb.GetHeight())
    return Option<VirtualConsole>::None();

  return Some(VirtualConsole(_fb, font));
}

void VirtualConsole::ReverseCursor(void) {
  if (cx)
    --cx;
  else if (cy) {
    cx = width - 1;
    --cy;
  }
}

void VirtualConsole::AdvanceCursor(void) {
  if (++cx >= width) {
    cx = 0;
    if (++cy >= height)
      cy = height - 1;
  }
}

void VirtualConsole::WriteByte(char byte) {
  u8 b = byte;

  switch (b) {
  case '\b':
    ReverseCursor();
    return;
  case '\r':
    cx = 0;
    return;
  case '\n':
    cx = 0;
    if (cy < height - 1)
      ++cy;
    return;
  }

  if (b >= font.GetGlyphCount())
    return;

  u64 x = cx * font.GetGlyphWidth();
  u64 y = cy * font.GetGlyphHeight();

  fb.DrawBitmap(static_cast<const u8 *>(font.GetGlyphData()) +
                    (b * font.GetGlyphStride()),
                font.GetGlyphWidth(), font.GetGlyphHeight(),
                ((font.GetGlyphWidth() + 7) & ~7) >> 3, x, y, true,
                Color::WHITE, Color::BLACK);

  AdvanceCursor();
}

void VirtualConsole::WriteBuffer(const char *buf, usize len) {
  for (usize i = 0; i < len; ++i)
    WriteByte(buf[i]);
}

void VirtualConsole::Flush() {}

VirtualConsole::VirtualConsole(Framebuffer fb, BitmapFont font)
    : fb(fb), font(font), width(0), height(0), cx(0), cy(0) {
  width  = fb.GetWidth() / font.GetGlyphWidth();
  height = fb.GetHeight() / font.GetGlyphHeight();
}

} // namespace Gfx
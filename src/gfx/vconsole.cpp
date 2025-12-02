#include "gfx/vconsole.hpp"
#include "arch.hpp"
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

  return Option<VirtualConsole>::Some(VirtualConsole(_fb, font));
}

void VirtualConsole::ReverseCursor(void) {
  if (m_x)
    --m_x;
  else if (m_y) {
    m_x = m_width - 1;
    --m_y;
  }
}

void VirtualConsole::AdvanceCursor(void) {
  if (++m_x >= m_width) {
    m_x = 0;
    if (++m_y >= m_height)
      m_y = m_height - 1;
  }
}

VirtualConsole::VirtualConsole(Framebuffer fb, BitmapFont font)
    : m_fb(fb), m_font(font), m_width(0), m_height(0), m_x(0), m_y(0) {
  m_width  = fb.GetWidth() / font.GetGlyphWidth();
  m_height = fb.GetHeight() / font.GetGlyphHeight();
}

void VirtualConsole::Flush() {
  // Flush write-combining memory.
  Arch::StoreFence();
}

void VirtualConsole::Write(const char *str, usize len) {
  for (usize i = 0; i < len; ++i) {
    u8 b = str[i];

    switch (b) {
    case '\b':
      ReverseCursor();
      continue;
    case '\r':
      m_x = 0;
      continue;
    case '\n':
      m_x = 0;
      if (m_y < m_height - 1)
        ++m_y;
      continue;
    }

    if (b >= m_font.GetGlyphCount())
      return;

    u64 x = m_x * m_font.GetGlyphWidth();
    u64 y = m_y * m_font.GetGlyphHeight();

    m_fb.DrawBitmap(static_cast<const u8 *>(m_font.GetGlyphData()) +
                        (b * m_font.GetGlyphStride()),
                    m_font.GetGlyphWidth(), m_font.GetGlyphHeight(),
                    ((m_font.GetGlyphWidth() + 7) & ~7) >> 3, x, y, true,
                    Color::WHITE, Color::BLACK);

    AdvanceCursor();
  }
}

} // namespace Gfx
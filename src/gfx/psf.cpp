#include "gfx/psf.hpp"
#include "compiler.h"
#include "gfx/font.hpp"
#include "memory.hpp"
#include "option.hpp"

struct PSF1Header {
  u8 magic[2];
  u8 fontMode;
  u8 glyphHeight;
} PACKED;

struct PSF2Header {
  u8 magic[4];
  u32 version;
  u32 headerSize;
  u32 flags;
  u32 glyphCount;
  u32 glyphStride;
  u32 glyphHeight;
  u32 glyphWidth;
} PACKED;

static inline bool usize_mul_overflow(usize a, usize b) {
  if (!b)
    return false;
  return a > SIZE_MAX / b;
}

namespace Gfx {

Option<BitmapFont> LoadPSFFont(void *header, usize len) {
  if (header == nullptr)
    return Option<BitmapFont>::None();

  if (len > sizeof(PSF1Header)) {
    PSF1Header psf1Header;
    Memory::Copy(&psf1Header, header, sizeof(psf1Header));

    if (psf1Header.magic[0] == 0x36 && psf1Header.magic[1] == 0x04) {
      usize glyphCount = 256;

      if (psf1Header.fontMode & 0x1)
        glyphCount = 512;

      if (!psf1Header.glyphHeight)
        return Option<BitmapFont>::None();

      if (usize_mul_overflow(glyphCount, psf1Header.glyphHeight))
        return Option<BitmapFont>::None();

      if (glyphCount * psf1Header.glyphHeight > len - sizeof(PSF1Header))
        return Option<BitmapFont>::None();

      return Some(BitmapFont(
          static_cast<void *>(static_cast<char *>(header) + sizeof(PSF1Header)),
          glyphCount, 8, psf1Header.glyphHeight, psf1Header.glyphHeight));
    }
  }

  if (len > sizeof(PSF2Header)) {
    PSF2Header psf2Header;
    Memory::Copy(&psf2Header, header, sizeof(psf2Header));

    if (psf2Header.magic[0] == 0x72 && psf2Header.magic[1] == 0xB5 &&
        psf2Header.magic[2] == 0x4A && psf2Header.magic[3] == 0x86) {
      // bytes per row
      usize bpr = ((static_cast<usize>(psf2Header.glyphWidth) + 7) & ~7) >> 3;
      usize minStride;

      if (!psf2Header.glyphWidth || !psf2Header.glyphHeight)
        return Option<BitmapFont>::None();

      if (usize_mul_overflow(bpr, psf2Header.glyphHeight))
        return Option<BitmapFont>::None();

      minStride = bpr * psf2Header.glyphHeight;

      if (psf2Header.glyphStride < minStride)
        return Option<BitmapFont>::None();

      if (usize_mul_overflow(psf2Header.glyphCount, psf2Header.glyphStride))
        return Option<BitmapFont>::None();

      if (psf2Header.glyphCount * psf2Header.glyphStride >
          len - sizeof(PSF2Header))
        return Option<BitmapFont>::None();

      return Some(BitmapFont(
          static_cast<void *>(static_cast<char *>(header) + sizeof(psf2Header)),
          psf2Header.glyphCount, psf2Header.glyphWidth, psf2Header.glyphHeight,
          psf2Header.glyphStride));
    }
  }

  return Option<BitmapFont>::None();
}

} // namespace Gfx

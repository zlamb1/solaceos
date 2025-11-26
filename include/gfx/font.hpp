#pragma once

#include "common.h"

namespace Gfx {

class BitmapFont {
public:
  BitmapFont(void *glyphData, usize glyphCount, u32 glyphWidth, u32 glyphHeight,
             u8 glyphStride)
      : glyphData(glyphData), glyphCount(glyphCount), glyphWidth(glyphWidth),
        glyphHeight(glyphHeight), glyphStride(glyphStride) {}

  virtual ~BitmapFont() {}

  const void *GetGlyphData() const { return glyphData; }
  usize GetGlyphCount() const { return glyphCount; }
  u32 GetGlyphWidth() const { return glyphWidth; }
  u32 GetGlyphHeight() const { return glyphHeight; }
  u8 GetGlyphStride() const { return glyphStride; }

  bool ValidateFont() const {
    return glyphData != nullptr && glyphCount && glyphWidth && glyphHeight &&
           glyphStride;
  }

protected:
  void *glyphData;
  usize glyphCount;
  u32 glyphWidth, glyphHeight, glyphStride;
};

} // namespace Gfx

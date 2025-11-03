#ifndef FONT_H
#define FONT_H 1

#include "common.h"

struct font
{
  u32 glyph_count;
  u32 glyph_width;
  u32 glyph_height;
  u32 glyph_stride;
  u8 *glyphs;
};

#endif
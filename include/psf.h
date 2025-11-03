#ifndef PSF_H
#define PSF_H 1

#include <stddef.h>
#include <stdint.h>

#include "common.h"
#include "font.h"

#define PSF1_MAGIC 0x36, 0x04
#define PSF2_MAGIC 0x72, 0xB5, 0x4A, 0x86

#define PSF1_MODES512 0x1
#define PSF1_MODESTAB 0x2
#define PSF1_MODESEQ  0x4

#define PSF_SUCCESS   0
#define PSF_ERR_TRUNC -1
#define PSF_ERR_MAGIC -2

struct psf1_header
{
  u8 magic[2];
  u8 font_mode;
  u8 glyph_size;
} __attribute__ ((packed));

struct psf2_header
{
  u8 magic[4];
  u32 version;
  u32 header_size;
  u32 flags;
  u32 glyph_count;
  u32 glyph_size;
  u32 glyph_height;
  u32 glyph_width;
} __attribute__ ((packed));

int psf_parse (struct font *f, void *hdr, usize size);

#endif
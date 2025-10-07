#include "psf.h"

static const u8 psf1_magic[] = { PSF1_MAGIC };
static const u8 psf2_magic[] = { PSF2_MAGIC };

int
psf_parse (struct font *f, void *hdr, usize size)
{
  if (size < sizeof (struct psf1_header))
    return PSF_ERR_TRUNC;

  const u8 *bytes = hdr;

  if (bytes[0] == psf1_magic[0] && bytes[1] == psf1_magic[1])
    {
      struct psf1_header *phdr = hdr;

      f->glyph_count  = phdr->font_mode & PSF1_MODES512 ? 512 : 256;
      f->glyph_width  = 8;
      f->glyph_height = phdr->glyph_size;
      f->glyph_stride = f->glyph_height;
      f->glyphs       = (void *) ((char *) hdr + sizeof (struct psf1_header));

      if (size
          < sizeof (struct psf1_header) + f->glyph_count * f->glyph_stride)
        return PSF_ERR_TRUNC;
    }
  else if (bytes[0] == psf2_magic[0] && bytes[1] == psf2_magic[1]
           && bytes[2] == psf2_magic[2] && bytes[3] == psf2_magic[3])
    {
      if (size < sizeof (struct psf2_header))
        return PSF_ERR_TRUNC;

      struct psf2_header *phdr = hdr;
      f->glyph_count           = phdr->glyph_count;
      f->glyph_width           = phdr->glyph_width;
      f->glyph_height          = phdr->glyph_height;
      f->glyph_stride          = phdr->glyph_size;
      f->glyphs = (void *) ((char *) hdr + sizeof (struct psf2_header));

      if (size
          < sizeof (struct psf2_header) + f->glyph_count * f->glyph_stride)
        return PSF_ERR_TRUNC;
    }
  else
    return PSF_ERR_MAGIC;

  return PSF_SUCCESS;
}
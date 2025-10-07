#ifndef LIMINE_H
#define LIMINE_H 1

#include "common.h"

#define LIMINE_COMMON_MAGIC 0xc7b1dd30df4c8b88, 0x0a82e883a194f07b

#define LIMINE_REQUESTS_START_MARKER                                          \
  u64 limine_requests_start_marker[4]                                         \
      = { 0xf6b8f4b39de7d1ae, 0xfab91a6940fcb9cf, 0x785c6ed015d3e316,         \
          0x181e920a7852b9d9 };

#define LIMINE_FRAMEBUFFER_REQUEST                                            \
  { LIMINE_COMMON_MAGIC, 0x9d5827dcd881dd75, 0xa3148604f6fab11b }

#define LIMINE_DATE_AT_BOOT_REQUEST                                           \
  { LIMINE_COMMON_MAGIC, 0x502746e184c088aa, 0xfbc5ec83e6327893 }

#define LIMINE_REQUESTS_END_MARKER                                            \
  u64 limine_requests_end_marker[2]                                           \
      = { 0xadc0e0531bb10d03, 0x9572709f31764c62 };

#define LIMINE_FRAMEBUFFER_RGB 1

struct limine_framebuffer_request
{
  u64 id[4];
  u64 revision;
  struct limine_framebuffer_response *response;
};

struct limine_framebuffer_response
{
  u64 revision;
  u64 framebuffer_count;
  struct limine_framebuffer **framebuffers;
};

struct limine_framebuffer
{
  void *address;
  u64 width;
  u64 height;
  u64 pitch;
  u16 bpp; // Bits per pixel
  u8 memory_model;
  u8 red_mask_size;
  u8 red_mask_shift;
  u8 green_mask_size;
  u8 green_mask_shift;
  u8 blue_mask_size;
  u8 blue_mask_shift;
  u8 unused[7];
  u64 edid_size;
  void *edid;

  /* Response revision 1 */
  u64 mode_count;
  struct limine_video_mode **modes;
};

struct limine_date_at_boot_request
{
  u64 id[4];
  u64 revision;
  struct limine_date_at_boot_response *response;
};

struct limine_date_at_boot_response
{
  u64 revision;
  int64_t timestamp;
};

#endif
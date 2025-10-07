#ifndef LIMINE_H
#define LIMINE_H 1

#include <stdint.h>

#define LIMINE_COMMON_MAGIC 0xc7b1dd30df4c8b88, 0x0a82e883a194f07b

#define LIMINE_REQUESTS_START_MARKER                                          \
  uint64_t limine_requests_start_marker[4]                                    \
      = { 0xf6b8f4b39de7d1ae, 0xfab91a6940fcb9cf, 0x785c6ed015d3e316,         \
          0x181e920a7852b9d9 };

#define LIMINE_FRAMEBUFFER_REQUEST                                            \
  { LIMINE_COMMON_MAGIC, 0x9d5827dcd881dd75, 0xa3148604f6fab11b }

#define LIMINE_REQUESTS_END_MARKER                                            \
  uint64_t limine_requests_end_marker[2]                                      \
      = { 0xadc0e0531bb10d03, 0x9572709f31764c62 };

#define LIMINE_FRAMEBUFFER_RGB 1

struct limine_framebuffer_request
{
  uint64_t id[4];
  uint64_t revision;
  struct limine_framebuffer_response *response;
};

struct limine_framebuffer_response
{
  uint64_t revision;
  uint64_t framebuffer_count;
  struct limine_framebuffer **framebuffers;
};

struct limine_framebuffer
{
  void *address;
  uint64_t width;
  uint64_t height;
  uint64_t pitch;
  uint16_t bpp; // Bits per pixel
  uint8_t memory_model;
  uint8_t red_mask_size;
  uint8_t red_mask_shift;
  uint8_t green_mask_size;
  uint8_t green_mask_shift;
  uint8_t blue_mask_size;
  uint8_t blue_mask_shift;
  uint8_t unused[7];
  uint64_t edid_size;
  void *edid;

  /* Response revision 1 */
  uint64_t mode_count;
  struct limine_video_mode **modes;
};

#endif
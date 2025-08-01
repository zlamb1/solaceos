#ifndef SOLACE_MB1_H
#define SOLACE_MB1_H 1

#include <stddef.h>
#include <stdint.h>

typedef struct
{
  uint32_t start;
  uint32_t end;
  uint32_t string;
  uint32_t reserved;
} __attribute__ ((packed)) mb1_mod_t;

typedef struct
{
  union
  {
    struct __attribute__ ((packed))
    {
      uint32_t tabsize;
      uint32_t strsize;
      uint32_t addr;
      uint32_t reserved;
    } sym_table;
    struct __attribute__ ((packed))
    {
      uint32_t num;
      uint32_t size;
      uint32_t addr;
      uint32_t shndx;
    } sec_hdr_table;
  };

} __attribute__ ((packed)) mb1_syms_t;

typedef enum
{
  /* any unlisted types are considered reserved */
  MB1_MMAP_ENTRY_TYPE_FREE = 1,
  MB1_MMAP_ENTRY_TYPE_ACPI = 3,
  MB1_MMAP_ENTRY_TYPE_RESERVED = 4,
  MB1_MMAP_ENTRY_TYPE_BAD_RAM = 5,
} mb1_mmap_entry_type_t;

typedef struct
{
  /* -4: size; used to skip to next entry */
  uint64_t base_addr;
  uint64_t length;
  uint8_t type;
} __attribute__ ((packed)) mb1_mmap_entry_t;

typedef enum
{
  MB1_DRIVE_MODE_CHS = 0,
  MB1_DRIVE_MODE_LBA = 1
} mb1_drive_mode_t;

typedef struct
{
  uint32_t size;
  uint8_t number;
  uint8_t mode;
  uint16_t cylinders;
  uint8_t heads;
  uint8_t sectors;  /* sectors per track  */
  uint16_t ports[]; /* list of BIOS I/O ports; zero-terminated */
} __attribute__ ((packed)) mb1_drive_entry_t;

typedef struct
{
  uint16_t version;
  uint16_t cseg;
  uint32_t offset;
  uint16_t cseg_16;
  uint16_t dseg;
  uint16_t flags;
  uint16_t cseg_len;
  uint16_t cseg_16_len;
  uint16_t desg_len;
} __attribute__ ((packed)) mb1_apm_table_t;

typedef enum
{
  MB1_FRAMEBUFFER_TYPE_INDEXED = 0,
  MB1_FRAMEBUFFER_TYPE_RGB = 1,
  MB1_FRAMEBUFFER_TYPE_TEXT = 2,
} mb1_framebuffer_type_t;

typedef struct
{
  uint8_t red_value;
  uint8_t green_value;
  uint8_t blue_value;
} __attribute__ ((packed)) mb1_color_descriptor_t;

typedef struct
{
  uint32_t addr;
  uint32_t pitch;
  uint32_t width;
  uint32_t height;
  uint8_t bpp;
  uint8_t type;
  union
  {
    struct __attribute__ ((packed))
    {
      uint32_t palette_addr;
      uint8_t palette_num_colors;
    };
    struct __attribute__ ((packed))
    {
      uint8_t red_field_position;
      uint8_t red_field_mask_size;
      uint8_t green_field_position;
      uint8_t green_field_mask_size;
      uint8_t blue_field_position;
      uint8_t blue_field_mask_size;
    };
  };
} __attribute__ ((packed)) mb1_framebuffer_t;

typedef struct
{
  uint32_t flags;
  uint32_t mem_upper;
  uint32_t mem_lower;
  uint8_t part3;
  uint8_t part2;
  uint8_t part1;
  uint8_t drive;
  uint32_t cmdline;
  uint32_t mods_count;
  uint32_t mods_addr;
  mb1_syms_t syms;
  uint32_t mmap_size;
  uint32_t mmap_addr;
  uint32_t drives_size;
  uint32_t drives_addr;
  uint32_t config_table;
  uint32_t boot_loader_name;
  uint32_t apm_table;
  uint32_t vbe_control_info;
  uint32_t vbe_mode_info;
  uint32_t vbe_mode;
  uint32_t vbe_interface_seg;
  uint32_t vbe_interface_off;
  uint32_t vbe_interface_len;
  mb1_framebuffer_t framebuffer;
} __attribute__ ((packed)) mb1_info_t;

#endif

#ifndef EXT2FS_H
#define EXT2FS_H 1

#include <stdint.h>

#include "types.h"

typedef enum : u16
{
  EXT2_FS_CLEAN = 1,
  EXT2_FS_ERROR = 2,
} ext2fs_state_t;

typedef enum : u16
{
  EXT2_ERRORS_IGNORE = 1,
  EXT2_ERRORS_RO = 2,
  EXT2_ERRORS_PANIC = 3,
} ext2fs_errors_t;

typedef enum : u32
{
  EXT2_OSID_LINUX = 0,
  EXT2_OSID_HURD = 1,
  EXT2_OSID_MASIX = 2,
  EXT2_OSID_FREEBSD = 3,
} ext2fs_osid_t;

typedef enum : u32
{
  EXT2_OPT_FEATURE_PREALLOC_DIR = 0x1,
  EXT2_OPT_FEATURE_AFS_INO = 0x2,
  EXT2_OPT_FEATURE_JOURNAL = 0x4,
  EXT2_OPT_FEATURE_INO_EXT = 0x8,
  EXT2_OPT_FEATURE_RESIZABLE = 0x10,
  EXT2_OPT_FEATURE_DIR_HASH_IDX = 0x20,
} ext2fs_opt_features_t;

typedef enum : u32
{
  EXT2_REQ_FEATURE_COMPRESSION = 0x1,
  EXT2_REQ_FEATURE_DIR_HAS_TYPE = 0x2,
  EXT2_REQ_FEATURE_REPLAY_JOURNAL = 0x4,
  EXT2_REQ_FEATURE_HAS_JOURNAL = 0x8,
} ext2fs_compat_features_t;

typedef enum : u32
{
  EXT2_RO_FEATURE_SPARSE_SB_AND_BGDT = 0x1,
  EXT2_RO_FEATURE_64_BIT_FILES = 0x2,
  EXT2_RO_DIR_BIN_TREE = 0x4,
} ext2fs_ro_features_t;

typedef enum : u16
{
  EXT2_INO_PERM_OEXC = 0x1,
  EXT2_INO_PERM_OWRT = 0x2,
  EXT2_INO_PERM_ORD = 0x4,
  EXT2_INO_PERM_GEXC = 0x10,
  EXT2_INO_PERM_GWRT = 0x20,
  EXT2_INO_PERM_GRD = 0x40,
  EXT2_INO_PERM_UEXC = 0x100,
  EXT2_INO_PERM_UWRT = 0x200,
  EXT2_INO_PERM_URD = 0x400,

  EXT2_INO_STICKY_BIT = 0x1000,
  EXT2_INO_SETGID = 0x2000,
  EXT2_INO_SETUID = 0x4000,

  EXT2_INO_TYPE_FIFO = 0x1000,
  EXT2_INO_TYPE_CHR = 0x2000,
  EXT2_INO_TYPE_DIR = 0x4000,
  EXT2_INO_TYPE_BLK = 0x6000,
  EXT2_INO_TYPE_REG = 0x8000,
  EXT2_INO_TYPE_SYM = 0xA000,
  EXT2_INO_TYPE_SOCK = 0xC000,
} ext2fs_inode_mode_t;

typedef enum : u32
{
  EXT2_INO_FLAG_SEC_DEL = 0x1,
  EXT2_INO_FLAG_CPY_ON_DEL = 0x2,
  EXT2_INO_FLAG_FILE_COMP = 0x4,
  EXT2_INO_FLAG_SYNC_IMM = 0x8,
  EXT2_INO_FLAG_IMMUTABLE = 0x10,
  EXT2_INO_FLAG_APPEND_ONLY = 0x20,
  EXT2_INO_FLAG_NOT_INCL_DUMP = 0x40,
  EXT2_INO_FLAG_NO_SYNC_LAST_ACCESS = 0x80,
  EXT2_INO_FLAG_HASH_IDX_DIR = 0x10000,
  EXT2_INO_FLAG_AFS_DIR = 0x20000,
  EXT2_INO_FLAG_JOURNAL_FD = 0x40000,
} ext2fs_inode_flags_t;

typedef enum : u8
{
  EXT2_FT_UNKN = 0,
  EXT2_FT_REG = 1,
  EXT2_FT_DIR = 2,
  EXT2_FT_CHR = 3,
  EXT2_FT_BLK = 4,
  EXT2_FT_FIFO = 5,
  EXT2_FT_SOCK = 6,
  EXT2_FT_SYM = 7,
} ext2fs_ft_t;

typedef struct
{
  u32 sb_inodes;
  u32 sb_blocks;
  u32 sb_resblks;
  u32 sb_freeblks;
  u32 sb_freeinos;
  u32 sb_blk;
  u32 sb_log_blksize;
  u32 sb_log_frgsize;
  u32 sb_bpg;
  u32 sb_fpg;
  u32 sb_mtime;
  u32 sb_wtime;
  u16 sb_mnts;
  u16 sb_mnts_per_fsck;
  u16 sb_magic;
  ext2fs_state_t sb_state;
  ext2fs_errors_t sb_errors;
  u16 sb_minor;
  u32 sb_ctime;
  u32 sb_itime;
  ext2fs_osid_t sb_osid;
  u32 sb_major;
  u16 sb_resuid;
  u16 sb_resgid;
  /* Extended Fields : sb_major >= 1 */
  u32 sb_first_ino;
  u16 sb_ino_size;
  u16 sb_blkgrp;
  ext2fs_opt_features_t sb_opt_features;
  ext2fs_compat_features_t sb_compat_features;
  ext2fs_ro_features_t sb_ro_features;
  char sb_fs_id[16];
  char sb_volume_name[16];
  char sb_prev_mnt_path[64];
  u32 sb_comp_algo;
  u8 sb_preblks_file;
  u8 sb_preblks_dir;
  u8 sb_reserved1[2];
  char sb_journal_id[16];
  u32 sb_journal_ino;
  u32 sb_journal_dev;
  u32 sb_orphan_ino;
} ext2fs_sb_t;

typedef struct
{
  u32 bgdt_blk_bitmap;
  u32 bgdt_ino_bitmap;
  u32 bgdt_ino_table;
  u16 bgdt_freeblks;
  u16 bgdt_freeinos;
  u16 bgdt_dirs;
} ext2fs_bgdt_t;

typedef struct
{
  ext2fs_inode_mode_t ino_mode;
  u16 ino_uid;
  u32 ino_size_lo;
  u32 ino_atime;
  u32 ino_ctime;
  u32 ino_wtime;
  u32 ino_dtime;
  u16 ino_gid;
  u16 ino_nlinks;
  u32 ino_nsecs;
  ext2fs_inode_flags_t ino_flags;
  u32 ino_os_res1;
  u32 ino_blocks[15];
  u32 ino_gennum;
  u32 ino_xattr;
  u32 ino_size_hi;
  u32 ino_frag;
  u32 ino_os_res2[3];
} ext2fs_inode_t;

#endif
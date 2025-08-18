#ifndef EXT2FS_DISK_H
#define EXT2FS_DISK_H 1

#include <stdint.h>

#include "endian.h"
#include "types.h"

typedef struct
{
  __le32 sb_inodes;
  __le32 sb_blocks;
  __le32 sb_resblks;
  __le32 sb_freeblks;
  __le32 sb_freeinos;
  __le32 sb_blk;
  __le32 sb_log_blksize;
  __le32 sb_log_frgsize;
  __le32 sb_bpg;
  __le32 sb_fpg;
  __le32 sb_mtime;
  __le32 sb_wtime;
  __le16 sb_mnts;
  __le16 sb_mnts_per_fsck;
  __le16 sb_magic;
  __le16 sb_state;
  __le16 sb_errors;
  __le16 sb_minor;
  __le32 sb_ctime;
  __le32 sb_itime;
  __le32 sb_osid;
  __le32 sb_major;
  __le16 sb_resuid;
  __le16 sb_resgid;
  /* Extended Fields : sb_major >= 1 */
  __le32 sb_first_ino;
  __le16 sb_ino_size;
  __le16 sb_blkgrp;
  __le32 sb_opt_features;
  __le32 sb_compat_features;
  __le32 sb_ro_features;
  char sb_fs_id[16];
  char sb_volume_name[16];
  char sb_prev_mnt_path[64];
  __le32 sb_comp_algo;
  u8 sb_preblks_file;
  u8 sb_preblks_dir;
  u8 sb_reserved1[2];
  char sb_journal_id[16];
  __le32 sb_journal_ino;
  __le32 sb_journal_dev;
  __le32 sb_orphan_ino;
} ext2fs_sb_disk_t;

typedef struct
{
  __le32 dt_blk_bitmap;
  __le32 dt_ino_bitmap;
  __le32 dt_ino_table;
  __le16 dt_freeblks;
  __le16 dt_freeinos;
  __le16 dt_dirs;
} ext2fs_bgdt_disk_t;

typedef struct
{
  __le16 ino_mode;
  __le16 ino_uid;
  __le32 ino_size_lo;
  __le32 ino_atime;
  __le32 ino_ctime;
  __le32 ino_wtime;
  __le32 ino_dtime;
  __le16 ino_gid;
  __le16 ino_nlinks;
  __le32 ino_nsecs;
  __le32 ino_flags;
  __le32 ino_os_res1;
  __le32 ino_blocks[15];
  __le32 ino_gennum;
  __le32 ino_xattr;
  __le32 ino_size_hi;
  __le32 ino_frag;
  __le32 ino_os_res2[3];
} ext2fs_inode_disk_t;

#endif
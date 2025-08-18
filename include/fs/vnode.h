#ifndef VNODE_H
#define VNODE_H 1

#include "spinlock.h"
#include "types.h"

struct dentry;
struct vnode;

typedef struct
{
  int (*create) (struct vnode *, struct dentry *, int);
  void (*truncate) (struct vnode *);
} vnode_ops_t;

typedef struct
{
  ssize_t (*read) (struct vnode *, size_t, void *);
  ssize_t (*write) (struct vnode *, size_t, const void *);
} file_ops_t;

typedef struct vnode
{
  u64 v_ino;
  u64 v_size;
  u32 v_uid;
  u32 v_gid;
  u64 v_atime;
  u64 v_mtime;
  vnode_ops_t *v_ops;
  file_ops_t *v_fops;
  spinlock_t v_lock;
  void *v_data;
} vnode_t;

#endif
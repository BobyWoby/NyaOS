#ifndef __SLAB_H
#define __SLAB_H
#include <stddef.h>
#include <stdint.h>

#define PAGE_SIZE 4096
#define BUFS_PER_SLAB 8.0
#define SMALL_OBJ_SIZE PAGE_SIZE / BUFS_PER_SLAB

typedef struct slab kmem_slab;
typedef struct bufctl kmem_bufctl;

typedef enum {
    EMPTY,
    DELETED,
    FULL
} hash_state;

typedef struct header{
    uint32_t magic; // 0xDEADBEEF
    uint32_t size;
} kmem_buf_hdr;

typedef struct hashval{
    kmem_bufctl *val; // bufctl
    uint64_t key; // buffer addr 
    hash_state state;
}hash_val;

typedef struct hashtable{
    hash_val *buckets;
    size_t size, num_buckets;
} slab_ht ;

typedef struct bufctl{
    void *buf;
    kmem_slab *back; // backlink to parent slab
    struct bufctl *next; // next freelist entry
} kmem_bufctl;

typedef struct slab{
    struct slab *prev, *next;
    kmem_bufctl *freelist;
    int refs;
}kmem_slab;

typedef struct cache{
    kmem_slab *head, *tail, *fl_ptr;
    slab_ht *buf2bufctl;
    char *name;
    size_t size, align; // object size
} kmem_cache;
#endif

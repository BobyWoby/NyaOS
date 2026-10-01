#ifndef __SLAB_H
#define __SLAB_H
#include <stddef.h>
#include <stdint.h>
#include <kernel/pfa.h>

// number of pre-initialzied caches: cache sizes go from 2^1B -> 2^{INITIAL_SLAB_CNT}B
#define INITIAL_SLAB_CNT 12
#define BUFS_PER_SLAB 8.0
#define SMALL_OBJ_SIZE PAGE_SIZE / BUFS_PER_SLAB

typedef struct slab kmem_slab;
typedef struct cache kmem_cache;
typedef struct bufctl kmem_bufctl;

typedef enum {
    EMPTY,
    DELETED,
    FULL
} hash_state;

typedef struct hashval{
    kmem_bufctl *val; // bufctl
    uintptr_t key; // buffer addr 
    hash_state state;
}hash_val;

typedef struct hashtable{
    hash_val *buckets;
    size_t size, capacity;
} slab_ht ;

typedef struct bufctl{
    void *buf;
    kmem_slab *back; // backlink to parent slab
    struct bufctl *next; // next freelist entry
} kmem_bufctl;

typedef struct slab{
    struct slab *prev, *next;
    kmem_cache* back;
    kmem_bufctl *freelist;
    void *pstart;
    int refs, buf_cnt;
}kmem_slab;

typedef struct cache{
    slab_ht buf2bufctl;
    kmem_slab *head, *tail, *fl_ptr;
    char *name;
    size_t size, align; // object size
    kmem_slab hslab, tslab;
} kmem_cache;

// void kmem_cache_grow(kmem_cache* cache);
void* kmem_cache_alloc(kmem_cache* cache);
// void kmem_cache_init(kmem_cache* cache, size_t size);
void slab_alloc_init();
kmem_cache* kmem_cache_create(char* name, size_t size, int align);
void kmem_cache_grow(kmem_cache* cache);
void kmem_cache_reap(kmem_cache* cache);
void kmem_cache_free(kmem_cache* cache, void* buf);

#endif

#ifndef __SLAB_H
#define __SLAB_H
#include <stddef.h>
#include <stdint.h>

typedef struct bufctl kmem_bufctl;

typedef struct lslab{
    struct lslab *prev, *next;
    kmem_bufctl *freelist;
    int refs, size;
}kmem_slab;


typedef struct bufctl{
    kmem_slab *back; // backlink to parent slab
    struct bufctl *next; // next freelist entry
    void *buf;
} kmem_bufctl;

typedef struct hashtable{
    size_t size;
    // maps ppn to bufctl
    kmem_bufctl *buckets;
} slab_ht ;

typedef struct lcache{
    kmem_slab *head, *tail, *fl_ptr;
    slab_ht *buf2bufctl;
} lscache_t;

typedef struct buf_node{
    void *mem;
    struct buf_node *next;
} sbufctl;

typedef struct sslab{
   sbufctl *head; 
   int refcnt;
   struct sslab *next, *prev;
}kmem_sslab;

typedef struct scache{
    kmem_sslab *head, *tail, *fl_ptr;
}sscache_t;

#endif

#include <kernel/slab.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <kernel/pfa.h>

#define PAGE_SIZE 4096

uint64_t hash(uint64_t in, int sz){
    return in % sz;
}

size_t hash_buf(slab_ht *ht, uint64_t buf_addr){
    buf_addr = buf_addr >> 10; // get the page number of the buffer
    return (size_t)hash(ht->size, (uint64_t)buf_addr);
}




void slab_alloc_init(){
}


// allocate a slab
void *kmem_cache_alloc(lscache_t *cache){
    if(!cache->fl_ptr){
        // new slab with kalloc
    }
    kmem_slab* slab = cache->fl_ptr;
    kmem_bufctl *tmp = slab->freelist;
    slab->freelist = slab->freelist->next;
    slab->refs++;
    return tmp->buf;
}

// free all unused slabs
void *kmem_cache_reap(kmem_slab *cache){
    
}

void *kmalloc(size_t size){
    int idx;
    for(idx = 0; idx < 12 && size > cache_sizes[idx]; ++idx){}
    if (idx >= 12){
        // gonna have to do smt else here bc its too big
        // create a new cache?
    }else{
        return kmem_cache_alloc(caches[idx]);
    }
}

void kfree(void *ptr){
}

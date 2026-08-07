#include <kernel/pager.h>
#include <kernel/pfa.h>
#include <kernel/slab.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>


// object cache of size kmem_cache + 2 * kmem_slab
static kmem_cache cache_cache;
kmem_cache caches[12]; // statically pre-built caches in powers of 2

void* kmalloc(size_t size);
uint64_t hash(uint64_t in, int sz) { return in % sz; }

void* kmem_cache_alloc(kmem_cache* cache);

size_t hash_buf(slab_ht* ht, uint64_t buf_addr) {
    buf_addr = buf_addr >> 12;  // get the page number of the buffer
    return (size_t)hash((uint64_t)buf_addr, ht->size);
}

slab_ht* ht_create() { return NULL; }

kmem_bufctl* hash_get(slab_ht* ht, uint64_t key) {
    size_t hash = hash_buf(ht, key);
    while (true) {
        if (ht->buckets[hash].state == FULL && ht->buckets[hash].key == key) {
            return ht->buckets[hash].val;
        } else if (ht->buckets[hash].state == EMPTY) {
            break;
        } else {
            ++hash;
        }
    }
    return NULL;
}

void slab_alloc_init() {}

kmem_cache* kmem_cache_create(char *name, size_t size, int align) { 
    // create the cache
    kmem_cache* cache = kmem_cache_alloc(&cache_cache);
    cache->name = name;
    cache->size = size;
    cache->align = align;

    cache->head = (kmem_slab *)(cache + 1); 
    cache->tail = (kmem_slab *)(cache + 1) + 1;
    cache->head->prev = NULL;
    cache->head->next = cache->tail;
    cache->head->freelist = NULL;
    cache->head->refs = 0;

    cache->tail->prev = cache->head;
    cache->tail->next = NULL;
    cache->tail->freelist = NULL;
    cache->tail->refs = 0;

    cache->fl_ptr = cache->tail;

    return NULL;
}

// insert the slab before it (pass cache->tail to append)
void _insert_slab(kmem_slab *it, kmem_slab *slab){
    it->prev->next = slab;
    slab->prev = it->prev;
    it->prev = slab;
    slab->next = it;
}

// append slab to the end of the cache
void _append_slab(kmem_cache* cache, kmem_slab* slab) {
    _insert_slab(cache->tail, slab);
    if (cache->fl_ptr == cache->tail) {
        cache->fl_ptr = slab;
    }
}



// unlinks a slab from it's freelist
void _rm_slab(kmem_slab *slab){
    slab->prev->next = slab->next;
    slab->next->prev = slab->prev;
    slab->next = NULL;
    slab->prev = NULL;
}

// grow cache by one slab
// TODO: add alignment to the slabs
void kmem_cache_grow(kmem_cache* cache) {
    kmem_slab* new_slab;
    if (cache->size <= SMALL_OBJ_SIZE) {
        void* pstart = phys_to_virt((uint64_t)kalloc_frame());

        uintptr_t slab_addr = (uintptr_t)(pstart + PAGE_SIZE - sizeof(kmem_slab));
        slab_addr &= ~((uintptr_t)_Alignof(kmem_slab) - 1);
        
        new_slab = (kmem_slab *)slab_addr;
        new_slab->next = new_slab->prev = NULL;
        new_slab->refs = 0;
        new_slab->freelist = pstart;

        size_t eff_size = cache->size + sizeof(kmem_bufctl);

        void* p;
        for (p = pstart; p + eff_size < (void*)new_slab; p += eff_size) {
            void** tmp = p + cache->size;
            *tmp = p + eff_size;
        }

        void** tmp = (p - sizeof(void*));
        *tmp = NULL;
    } else {
        // large object cache
        size_t bytes = cache->size * BUFS_PER_SLAB;
        size_t frames = (bytes + PAGE_SIZE - 1) / PAGE_SIZE;
        void* pstart =
            phys_to_virt((uint64_t)kalloc_frames(frames));

        // this can prolly be swapped with kmem_cache_alloc instead w/ a global kmem_slab cache
        new_slab = (kmem_slab*)kmalloc(sizeof(kmem_slab) + BUFS_PER_SLAB * sizeof(kmem_bufctl));
        new_slab->next = new_slab->prev = NULL;
        new_slab->refs = 0;

        kmem_bufctl* buf_start = (kmem_bufctl*)(new_slab + 1);
        new_slab->freelist = buf_start;

        // TODO: rewrite this loop to be cleaner
        for (int i = 0; i < BUFS_PER_SLAB; ++i) {
            kmem_bufctl* buf = buf_start + i;
            buf->buf = (void *)((uintptr_t)pstart + (i * cache->size));
            buf->next = NULL;
            if (i < BUFS_PER_SLAB - 1) {
                // this should be right?
                buf->next = (kmem_bufctl*)((uintptr_t)buf + sizeof(kmem_bufctl));
            }
            buf->back = new_slab;
        }
    }
    // add the new_slab slab to the cache's free list
    _append_slab(cache, new_slab);
}

// free all unused slabs
void* kmem_cache_reap(kmem_cache* cache) { return NULL; }

void kmem_cache_free(kmem_cache *cache, void *buf){
    void *mem;
    kmem_slab *slab;
    if(cache->size < SMALL_OBJ_SIZE){
        uintptr_t mask = ~(PAGE_SIZE - 1); // page size should be a power of 2
        uintptr_t page = ((uintptr_t)buf & mask);
        slab = (kmem_slab *)(page + PAGE_SIZE - sizeof(kmem_slab));
        void **next = (void **)((uintptr_t)buf + cache->size);
        if((uintptr_t)*next != 0xDEADBEEF){
            // smt's wrong
        }
        *next = slab->freelist;
        slab->freelist = buf;
        slab->refs--;

        if(!slab->refs){
            //remove the slab and free the page
            _rm_slab(slab);
            // free the page
            free_page((void *)page);
        }
        if(slab->refs == BUFS_PER_SLAB - 1){
            // move the slab to the front of the cache's freelist
            _rm_slab(slab);
            _insert_slab(cache->fl_ptr, slab);
            cache->fl_ptr = slab;
        }
    }else{
        // need a hash table here
    }
}

void* kmem_cache_alloc(kmem_cache* cache) {
    // if there's no free slab
    if(cache->fl_ptr == NULL || cache->fl_ptr == cache->tail){
        kmem_cache_grow(cache);
    }
    if(cache->fl_ptr->refs == BUFS_PER_SLAB){
        kmem_cache_grow(cache);
    }

    kmem_slab *slab = cache->fl_ptr;
    
    // if this is a small object, the bufctl object is 
    if(cache->size <= SMALL_OBJ_SIZE){
        void *res = (void *)slab->freelist;
        slab->freelist = (kmem_bufctl *)((uintptr_t)res + cache->size);
        *(uint64_t *)((uintptr_t)res + cache->size) = 0xDEADBEEF;
        ++(slab->refs);
        return res;
    }else{
        kmem_bufctl *bufctl = slab->freelist; 
        slab->freelist = bufctl->next;
        bufctl->next = (void *)0xDEADBEEF;
        ++(slab->refs);
        return bufctl->buf;
    }

    return NULL;
}

void* kmalloc(size_t size) {
    int idx;
    for (idx = 0; idx < 12 && size > (1 << idx); ++idx) {
    }
    if (idx >= 12) {
        // gonna have to do smt else here bc its too big
        // create a new cache?
    } else {
        return kmem_cache_alloc(&caches[idx]);
    }
}

void kfree(void* ptr) {
    // gonna have to build a lookup table for ts :(
}

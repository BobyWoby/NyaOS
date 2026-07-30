#include <kernel/pager.h>
#include <kernel/pfa.h>
#include <kernel/slab.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

uint64_t hash(uint64_t in, int sz) { return in % sz; }

size_t hash_buf(slab_ht *ht, uint64_t buf_addr) {
  buf_addr = buf_addr >> 12; // get the page number of the buffer
  return (size_t)hash((uint64_t)buf_addr, ht->size);
}

kmem_bufctl *hash_get(slab_ht *ht, uint64_t key) {
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

// append slab to the end of the cache
void _add_slab(kmem_cache *cache, kmem_slab *slab) {
  kmem_slab *tmp = cache->tail;
  tmp->prev->next = slab;
  slab->prev = tmp->prev;
  tmp->prev = slab;
  slab->next = tmp;
  if (cache->fl_ptr == cache->tail) {
    cache->fl_ptr = slab;
  }
}

// grow cache by one slab
void kmem_cache_grow(kmem_cache *cache) {
  kmem_slab *new_slab;
  if (cache->size < SMALL_OBJ_SIZE) {
    void *pstart = phys_to_virt((uint64_t)kalloc_frame());
    new_slab = pstart + PAGE_SIZE - sizeof(kmem_slab);
    new_slab->next = new_slab->prev = NULL;
    new_slab->refs = 0;
    new_slab->freelist = pstart;
    size_t eff_size = cache->size + sizeof(kmem_bufctl);

    for (void *p = pstart; p < (void *)new_slab; p += eff_size) {
      void **tmp = p + cache->size;
      *tmp = p + eff_size;
    }
    // add the new_slab slab to the cache's free list
    _add_slab(cache, new_slab);
  } else {
  }
}

// free all unused slabs
void *kmem_cache_reap(kmem_cache *cache) { return NULL; }

void *kmem_cache_alloc(kmem_cache *cache) {
  //
  return NULL;
}

void *kmalloc(size_t size) {
  int idx;
  for (idx = 0; idx < 12 && size > cache_sizes[idx]; ++idx) {
  }
  if (idx >= 12) {
    // gonna have to do smt else here bc its too big
    // create a new cache?
  } else {
    return kmem_cache_alloc(caches[idx]);
  }
}

void kfree(void *ptr) {}

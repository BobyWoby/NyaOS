#include <kernel/pager.h>
#include <kernel/slab.h>
#include <kernel/pfa.h>
#include <stddef.h>
#include <stdint.h>

// start off with one physical page
slab_ht create_ht(){
    slab_ht ret;
    hash_val *buckets = (hash_val *)phys_to_virt((uintptr_t)kalloc_frame());
    return ret;
}

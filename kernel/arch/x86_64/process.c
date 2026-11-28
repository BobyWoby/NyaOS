#include "kernel/process.h"

#include "kernel/pager.h"
#include "kernel/slab.h"
#include "kernel/system.h"
#include "stdlib.h"

unsigned int curr_pid = 0;

// This changes teh curr_pt to the new processes page table
int proc_create(ucontext_t* ctx) {
    kmem_cache_alloc(process_cache);
    uintptr_t root_phys = addrs_create(0, 0);
    if (!root_phys) abort();

    curr_pt = (uint64_t*)root_phys;
    if (map_page( ctx->code_phys, UC_VA, PAGE_US) == -1) abort();

    if (map_page( ctx->stack_phys, UC_STK_BASE, PAGE_RW | PAGE_US) == -1) abort();
    return curr_pid++;
}



#ifndef __PROCESS_H
#define __PROCESS_H

#include <stddef.h>
#include <stdint.h>
#include "kernel/slab.h"
#define UC_VA 0x400000
#define UC_STK_BASE 0x700000

extern unsigned int curr_pid;


typedef struct process{
    unsigned int pid;   
    uint64_t *page_table;
} process_t;

kmem_cache *process_cache;
process_t *processes;

typedef struct ucontext{
    uintptr_t code_phys;
    uintptr_t stack_phys; // base pointer of the stack
} ucontext_t;

int proc_create(ucontext_t *);
#endif

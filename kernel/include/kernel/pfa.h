#ifndef __PFA_H
#define __PFA_H
#include <kernel/multiboot.h>
#include <stddef.h>
#include <stdint.h>

typedef uint64_t *pageframe_t;

void pfa_init(multiboot_info_t *mbd);
pageframe_t kalloc_frame_int();
pageframe_t kalloc_frame();
void kfree_frame();

#endif

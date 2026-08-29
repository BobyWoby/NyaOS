#ifndef __PFA_H
#define __PFA_H
#include <stddef.h>
#include <stdint.h>

typedef void *pageframe_t;

void pfa_init();
pageframe_t kalloc_frame_int();
pageframe_t kalloc_frame();
pageframe_t kalloc_frames(size_t frames);
// takes in the physical address
void kfree_frame(pageframe_t a);
void kfree_frames(pageframe_t a, size_t frames);
uint64_t mem_high();

#endif

#ifndef __PFA_H
#define __PFA_H
#include <stddef.h>
#include <stdint.h>

typedef void* pageframe_t;

void pfa_init();
pageframe_t kalloc_frame_int();
pageframe_t kalloc_frame();
void kfree_frame(pageframe_t a);
uint64_t mem_high();

#endif

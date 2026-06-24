#ifndef __PFA_H
#define __PFA_H
#include <stddef.h>
#include <stdint.h>

typedef unsigned int *pageframe_t;

void pfa_init(uint32_t mem_high);
pageframe_t kalloc_frame_int();
pageframe_t kalloc_frame();
void kfree_frame();

#endif

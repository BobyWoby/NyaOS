#ifndef __PFA_H
#define __PFA_H
#include <stddef.h>
#include <stdint.h>

typedef uint64_t *pageframe_t;

void pfa_init();
pageframe_t kalloc_frame_int();
pageframe_t kalloc_frame();
void kfree_frame();

#endif

#ifndef __PFA_H
#define __PFA_H
#include <stddef.h>
#include <stdint.h>
#include <kernel/multiboot.h>

typedef uint32_t *pageframe_t;

void pfa_init(multiboot_info_t* mbd);
pageframe_t kalloc_frame_int();
pageframe_t kalloc_frame();
void kfree_frame();

#endif

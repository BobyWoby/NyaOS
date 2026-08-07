#ifndef PAGER_H
#define PAGER_H

#include <stddef.h>
#include <stdint.h>
// #define PAGE_SIZE 0x100

void paging_init();
void map_page(void *paddr, void *vaddr, unsigned int flags);
void free_page(void *vaddr);
void *phys_to_virt(uint64_t paddr);
uint64_t virt_to_phys(void *vaddr);

#endif

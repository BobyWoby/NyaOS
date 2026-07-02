#ifndef PAGER_H
#define PAGER_H

#include <stddef.h>
#include <stdint.h>
// #define PAGE_SIZE 0x100

void paging_init();
void map_page(void *paddr, void *vaddr, unsigned int flags);
void free_page();

#endif

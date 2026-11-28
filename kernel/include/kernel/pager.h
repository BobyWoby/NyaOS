#ifndef PAGER_H
#define PAGER_H

#include <stddef.h>
#include <stdint.h>
#define PAGE_P 1
#define PAGE_RW (1ULL << 1)
#define PAGE_US (1ULL << 2)
#define PAGE_PWT (1ULL << 3)
#define PAGE_PCD (1ULL << 4)
#define PAGE_PS (1ULL << 7)
#define PAGE_XD (1ULL << 63)

#define PAGE_ADDR_MASK 0x000FFFFFFFFFF000ULL

void paging_init();
extern uint64_t *curr_pt;

[[nodiscard]] int map_page(uintptr_t paddr, uintptr_t vaddr, uint64_t flags);
void free_page(void *vaddr);
uintptr_t phys_to_virt(uintptr_t paddr);
uint64_t virt_to_phys(uintptr_t vaddr);
uintptr_t addrs_create(unsigned int parent_pid, unsigned int child_pid);
#endif

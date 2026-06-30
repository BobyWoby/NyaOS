#include <kernel/pager.h>
#include <kernel/pfa.h>
#include <kernel/system.h>
#include <stdint.h>
#define PAGE_SIZE 0x1000

// 512 entries per page
uint64_t *pml4;

// these are only used to map the kernel
uint64_t *pd;
uint64_t *pt;
extern char kernel_start_physical;
extern char kernel_start_virtual;
extern char kernel_end_virtual;

void map_page(void *paddr, void *vaddr, unsigned int flags) {
  uint64_t pml4idx = (uint64_t)vaddr >> 39;
  uint64_t pdptidx = ((uint64_t)vaddr >> 30) & 0x1ff;
  uint64_t pdidx = ((uint64_t)vaddr >> 21) & 0x1ff;
  uint64_t ptidx = ((uint64_t)vaddr >> 12) & 0x1ff;
  // last 12 bits are page offset

  if (!(pml4[pml4idx] & 0x1)) {
    // not present, should allocate a page
    pml4[pml4idx] = kalloc_frame() | 0x1;
  }

  uint64_t *pdpt = (uint64_t *)(pml4[pml4idx] & ~0xfff);
  if (!(pdpt[pdptidx] & 0x1)) {
    pdpt[pdptidx] = kalloc_frame() | 0x1;
  }

  uint64_t *pd = (uint64_t *)(pdpt[pdptidx] & ~0xfff);
  if (!(pd[pdidx] & 0x1)) {
    pd[pdidx] = kalloc_frame() | 0x1;
  }

  uint64_t *pt = (uint64_t *)(pd[pdidx] & ~0xfff);
  // if (!(pt[ptidx] & 0x1)) {
  //   pt[ptidx] = kalloc_frame() | 0x1;
  // }
  pt[ptidx] = ((uint64_t)paddr) | (flags & 0xFFF) | 0x01;
}

void paging_init() {
  pml4 = kalloc_frame();

  uint64_t offset = kernel_start_virtual - kernel_start_physical;
  for (char *p = &kernel_start_virtual; p < &kernel_end_virtual;
       p += PAGE_SIZE) {
    map_page(p - offset, p, 0);
  }

  __asm__ volatile("mov %0, %%cr3" : : "r"(pml4) : "memory");
}

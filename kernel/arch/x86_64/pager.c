#include <kernel/pager.h>
#include <kernel/pfa.h>
#include <kernel/requests.h>
#include <kernel/system.h>
#include <stdint.h>
#include <stdio.h>
#define PAGE_SIZE 0x1000

// 512 entries per page
uint64_t* pml4;

// these are only used to map the kernel
uint64_t* pd;
uint64_t* pt;
extern char kernel_start_physical;
extern char kernel_start_virtual;
extern char kernel_end_virtual;

uint64_t offset;

void zero_table(uint64_t paddr) {
    uint64_t* vaddr = (uint64_t*)(paddr + offset);
    for (int i = 0; i < 512; ++i) {
        vaddr[i] = 0;
    }
}
// map one 2 MiB page: walk stops at the PD, PS bit set
void map_page_2m(uint64_t paddr, uint64_t vaddr, uint64_t flags) {
    uint64_t pml4idx = (vaddr >> 39) & 0x1ff;
    uint64_t pdptidx = (vaddr >> 30) & 0x1ff;
    uint64_t pdidx = (vaddr >> 21) & 0x1ff;

    if (!(pml4[pml4idx] & 0x1)) {
        uint64_t f = (uint64_t)kalloc_frame();
        zero_table(f);
        pml4[pml4idx] = f | 0x3;  // present + writable
    }
    uint64_t* pdpt = (uint64_t*)((pml4[pml4idx] & ~0xfffULL) + offset);

    if (!(pdpt[pdptidx] & 0x1)) {
        uint64_t f = (uint64_t)kalloc_frame();
        zero_table(f);
        pdpt[pdptidx] = f | 0x3;
    }
    uint64_t* pd = (uint64_t*)((pdpt[pdptidx] & ~0xfffULL) + offset);

    pd[pdidx] = (paddr & ~0x1fffffULL) | flags | 0x80 | 0x3;  // PS | present | writable
}

void map_page(void* paddr, void* vaddr, unsigned int flags) {
    uint64_t pml4idx = (uint64_t)vaddr >> 39 & 0x1ff;
    uint64_t pdptidx = ((uint64_t)vaddr >> 30) & 0x1ff;
    uint64_t pdidx = ((uint64_t)vaddr >> 21) & 0x1ff;
    uint64_t ptidx = ((uint64_t)vaddr >> 12) & 0x1ff;
    // last 12 bits are page offset

    if (!(pml4[pml4idx] & 0x1)) {
        // not present, should allocate a page
        uint64_t f = (uint64_t)(kalloc_frame());
        zero_table(f);
        pml4[pml4idx] = f | 0x3;
    }

    uint64_t* pdpt = (uint64_t*)((pml4[pml4idx] & ~0xfff) + offset);
    if (!(pdpt[pdptidx] & 0x1)) {
        uint64_t f = (uint64_t)(kalloc_frame());
        zero_table(f);
        pdpt[pdptidx] = f | 0x3;
    }

    uint64_t* pd = (uint64_t*)((pdpt[pdptidx] & ~0xfff) + offset);
    if (!(pd[pdidx] & 0x1)) {
        uint64_t f = (uint64_t)(kalloc_frame());
        zero_table(f);
        pd[pdidx] = f | 0x3;
    }

    uint64_t* pt = (uint64_t*)((pd[pdidx] & ~0xfff) + offset);
    // if (!(pt[ptidx] & 0x1)) {
    //   pt[ptidx] = kalloc_frame() | 0x1;
    // }
    pt[ptidx] = ((uint64_t)paddr) | (flags & 0xFFF) | 0x3;
}

void paging_init() {
    offset = hhdm_request.response->offset;
    pml4 = (uint64_t*)((uint64_t)kalloc_frame() + offset);

    uint64_t kd = &kernel_start_virtual - &kernel_start_physical;

    for (int i = 0; i < 512; ++i) {
        pml4[i] = 0;
    }

    for (char* p = &kernel_start_virtual; p < &kernel_end_virtual; p += PAGE_SIZE) {
        uint64_t paddr = (uint64_t)p - eaddr_request.response->virtual_base +
                         eaddr_request.response->physical_base;
        map_page((void *)paddr, p, 0);
    }

    struct limine_memmap_response* mmap = mmap_request.response;
    uint64_t top = 0;
    for (uint64_t i = 0; i < mmap->entry_count; ++i) {
        struct limine_memmap_entry* e = mmap->entries[i];
        uint64_t end = e->base + e->length;
        if (end > top) top = end;
    }
    // top = (top + 0xfff) & ~0xfffULL; // round up to 4KiB
    //
    // for (char* p = 0; p < top; p += PAGE_SIZE) {
    //     map_page(p, p + offset, 0);
    // }
    //

    top = (top + 0x1fffff) & ~0x1fffffULL;  // round up to 2 MiB
    for (uint64_t p = 0; p < top; p += 0x200000) map_page_2m(p, p + offset, 0);

    __asm__ volatile("mov %0, %%cr3" : : "r"((uint64_t)pml4 - offset) : "memory");

    printf("Paging initialized!\n");
}

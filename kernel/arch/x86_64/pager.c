#include <kernel/ioapic.h>
#include <kernel/pager.h>
#include <kernel/pfa.h>
#include <kernel/requests.h>
#include <kernel/system.h>
#include <stdint.h>
#include <stdio.h>

#include "stdlib.h"

// 512 entries per page
uint64_t* pml4;

extern char kernel_start_physical;
extern char kernel_start_virtual;
extern char kernel_end_virtual;

static uint32_t* page_bm;  // bitmap of virtual pages

uint64_t offset;

void zero_table(uint64_t paddr) {
    uint64_t* vaddr = (uint64_t*)(paddr + offset);
    for (int i = 0; i < 512; ++i) {
        vaddr[i] = 0;
    }
}

static uint64_t* next_table(uint64_t* table, size_t idx, bool user) {
    uint64_t pte = table[idx];
    if (!(pte & PAGE_P)) {
        // the page isn't present so we should request a new physical frame to make it resident
        uintptr_t frame = (uintptr_t)kalloc_frame();
        if ((void*)frame == NULL) {
            abort();
        }

        zero_table(frame);  // zero out the frame
        table[idx] = frame | PAGE_P | PAGE_RW | (PAGE_US * user);
    } else {
        if (pte & PAGE_PS) {
            return NULL;
        }
        if (user) {
            table[idx] |= PAGE_US;
        }
    }
    return (uint64_t*)phys_to_virt(table[idx] & PAGE_ADDR_MASK);
}

// map one 2 MiB page: walk stops at the PD, PS bit set
int map_page_2m(uint64_t paddr, uint64_t vaddr, uint64_t flags) {
    uint64_t pml4idx = (vaddr >> 39) & 0x1ff;
    uint64_t pdptidx = (vaddr >> 30) & 0x1ff;
    uint64_t pdidx = (vaddr >> 21) & 0x1ff;

    bool user = (flags & PAGE_US) != 0;

    uintptr_t* pdpt = next_table(pml4, pml4idx, user);
    if (!pdpt) {
        return -1;
    }

    uint64_t* pd = next_table(pdpt, pdptidx, user);
    if (!pd) {
        return -1;
    }
    if (pd[pdidx] & PAGE_P) return -1;

    pd[pdidx] = (paddr & ~0x1fffffULL) | flags | PAGE_PS | PAGE_P;  // PS | present | writable
    return 0;
}

int map_page(uintptr_t paddr, uintptr_t vaddr, uint64_t flags) {
    uintptr_t pml4idx = vaddr >> 39 & 0x1ff;
    uintptr_t pdptidx = vaddr >> 30 & 0x1ff;
    uintptr_t pdidx = vaddr >> 21 & 0x1ff;
    uintptr_t ptidx = vaddr >> 12 & 0x1ff;

    // last 12 bits are page offset
    bool user = (flags & PAGE_US) != 0;

    uint64_t* pdpt = next_table(pml4, pml4idx, user);
    if (!pdpt) return -1;

    uint64_t* pd = next_table(pdpt, pdptidx, user);
    if (!pd) return -1;

    uint64_t* pt = next_table(pd, pdidx, user);
    if (!pt) return -1;

    if (pt[ptidx] & PAGE_P) {
        return -1;
    }
    pt[ptidx] = (paddr & PAGE_ADDR_MASK) | flags | PAGE_P;
    return 0;
}

void free_page(void* vaddr) {
    uint64_t pml4idx = (uint64_t)vaddr >> 39 & 0x1ff;
    uint64_t pdptidx = ((uint64_t)vaddr >> 30) & 0x1ff;
    uint64_t pdidx = ((uint64_t)vaddr >> 21) & 0x1ff;
    uint64_t ptidx = ((uint64_t)vaddr >> 12) & 0x1ff;

    if (!(pml4[pml4idx] & 0x1)) {
        return;  // nothing to do
    }

    uint64_t* pdpt = (uint64_t*)((pml4[pml4idx] & ~0xfff) + offset);
    if (!(pdpt[pdptidx] & 0x1)) {
        return;
    }
    uint64_t* pd = (uint64_t*)((pdpt[pdptidx] & ~0xfff) + offset);
    if (!(pd[pdidx] & 0x1)) {
        return;
    }

    uint64_t* pt = (uint64_t*)((pd[pdidx] & ~0xfff));
    kfree_frame(pt);

    bool empty = true;
    for (int i = 0; i < 512; ++i) {
        if (pd[i] & 0x1) {
            empty = false;
        }
    }
}

// allocate num_pages of virtually contiguous pages of memory
void* kalloc_pages(size_t num_pages) { return NULL; }

uintptr_t phys_to_virt(uintptr_t paddr) { return (paddr + offset); }

uintptr_t virt_to_phys(uintptr_t vaddr) { return (vaddr - offset); }

void paging_init() {
    __asm__ __volatile__("cli");
    offset = hhdm_request.response->offset;
    pml4 = (uint64_t*)((uint64_t)kalloc_frame() + offset);

    for (int i = 0; i < 512; ++i) {
        pml4[i] = 0;
    }
    for (char* p = &kernel_start_virtual; p < &kernel_end_virtual; p += PAGE_SIZE) {
        uint64_t paddr = (uint64_t)p - eaddr_request.response->virtual_base +
                         eaddr_request.response->physical_base;
        if (map_page(paddr, (uintptr_t)p, PAGE_RW) == -1) {
            abort();
        }
    }

    struct limine_memmap_response* mmap = mmap_request.response;
    uint64_t top = 0;
    for (uint64_t i = 0; i < mmap->entry_count; ++i) {
        struct limine_memmap_entry* e = mmap->entries[i];
        uint64_t end = e->base + e->length;
        if (end > top) top = end;
    }

    top = (top + 0x1fffff) & ~0x1fffffULL;  // round up to 2 MiB
    uintptr_t ioapic_region = ioapic_phys & ~(uintptr_t)0x1fffff;
    uint64_t flags = PAGE_RW;
    for (uint64_t p = 0; p < top; p += 0x200000) {
        flags = PAGE_RW;
        if (p == ioapic_region) {
            flags |= PAGE_PWT | PAGE_PCD;
        }
        if (map_page_2m(p, p + offset, flags) == -1) {
            abort();
        }
    }
    // maps ioapic separately if not in the hhdm
    if (ioapic_region >= top) {
        if (map_page(ioapic_phys, ioapic_phys + offset, 0b11000 | PAGE_RW) == -1) {
            abort();
        }
    }

    __asm__ volatile("mov %0, %%cr3" : : "r"((uint64_t)pml4 - offset) : "memory");

    // printf("Paging initialized!\n");
}

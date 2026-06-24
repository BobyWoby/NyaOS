#include <kernel/page.h>
#include <stddef.h>
#include <stdint.h>

#define PAGE_SIZE 0x1000
static uint32_t npages;
static uint32_t* bitmap;

extern char endkernel; // only care abt the address of this

static inline void bm_set(uint32_t i) { bitmap[i >> 5] |= (1u << (i & 0b11111)); }
static inline void bm_clear(uint32_t i) { bitmap[i >> 5] &= ~(1u << (i & 0b11111)); }
static inline int bm_test(uint32_t i) { return bitmap[i >> 5] & (1u << (i & 0b11111)); }

typedef uintptr_t pageframe_t;

static void pfa_init(uint32_t mem_high) {
    npages = mem_high / PAGE_SIZE;
    uintptr_t addr = ((uintptr_t)&endkernel + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    bitmap = (uint32_t *)addr;
}

static pageframe_t kalloc_frame_int() {}

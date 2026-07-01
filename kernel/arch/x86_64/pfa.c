#include <kernel/limine.h>
#include <kernel/requests.h>
#include <kernel/pfa.h>
#include <kernel/system.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define PAGE_SIZE 0x1000
static uint32_t npages;
static uint32_t *bitmap;

extern char endkernel; // only care abt the address of this



static inline void bm_set(uint32_t i) {
  bitmap[i >> 5] |= (1u << (i & 0b11111));
}
static inline void bm_clear(uint32_t i) {
  bitmap[i >> 5] &= ~(1u << (i & 0b11111));
}
static inline int bm_test(uint32_t i) {
  return bitmap[i >> 5] & (1u << (i & 0b11111));
}

pageframe_t pre_frames[20];

uint64_t mem_high() {
  struct limine_memmap_response *mmap = mmap_request.response;
  unsigned int high = 0;
  for (uint64_t i = 0; i < mmap->entry_count; ++i) {
    struct limine_memmap_entry *entry = mmap->entries[i];

    if (entry->type == LIMINE_MEMMAP_USABLE &&
        entry->base + entry->length > high) {
      high = entry->base + entry->length;
    }
  }
  return high;
}

void free_available_memory() {
  struct limine_memmap_response *mmap = mmap_request.response;
  for (int i = 0; i < mmap->entry_count; i++) {
    struct limine_memmap_entry *entry = mmap->entries[i];
    if (entry->type == LIMINE_MEMMAP_USABLE) {
      for (uint64_t m = entry->base;
           m + PAGE_SIZE < entry->base + entry->length; m += PAGE_SIZE) {
        bm_clear(m / PAGE_SIZE);
      }
    }
  }
}

uintptr_t bitmap_addr(uint64_t bitmap_size) {
  uint64_t offset = hhdm_request.response->offset;
  struct limine_memmap_response *mmap = mmap_request.response;
  for (int i = 0; i < mmap->entry_count; i++) {
    struct limine_memmap_entry *entry = mmap->entries[i];
    if (entry->type == LIMINE_MEMMAP_USABLE && entry->length > bitmap_size) {
      return entry->base + offset;
    }
  }
  // TODO: PANIC
}

void pfa_init() {
  npages = mem_high() / PAGE_SIZE;
  uint64_t bitmap_size = ((npages + 31) / 32) * sizeof(uint32_t);

  // uintptr_t addr =
  bitmap = (uint32_t *)bitmap_addr(bitmap_size);

  // set all the pages to be used by default
  for (int i = 0; i < ((npages + 31) >> 5); ++i) {
    bitmap[i] = 0xffffffff;
  }

  free_available_memory();

  // mark everything up to end-of-bitmap as used, might not be necessary
  // uint32_t reserved = ((addr + bitmap_size) + PAGE_SIZE - 1) / PAGE_SIZE;
  // for (uint32_t i = 0; i < reserved; ++i) {
  //   bm_set(i);
  // }
}

// return physical address of the frame
// find the first availabe frame
pageframe_t kalloc_frame_int() {
  for (unsigned int i = 0; i < (npages + 31) >> 5; ++i) {
    if (bitmap[i] == 0xffffffff) {
      continue;
    } else {
      for (unsigned int j = 0; j < 32; ++j) {
        unsigned int idx = ((i << 5) | j);
        if (!bm_test(idx)) {
          bm_set(idx);
          return (pageframe_t)(idx * PAGE_SIZE);
        }
      }
    }
  }
  // no frames left
  return NULL;
}

pageframe_t kalloc_frame() {
  static uint8_t allocate =
      1; // whether or not we are going to allocate a new set of preframes
  static uint8_t pframe = 0;
  pageframe_t ret;

  if (pframe == 20) {
    allocate = 1;
  }

  if (allocate == 1) {
    for (int i = 0; i < 20; i++) {
      pre_frames[i] = kalloc_frame_int();
      if (pre_frames[i] == NULL) {
        // TODO: PANIC
          printf("out of frames");
      }
    }
    pframe = 0;
    allocate = 0;
  }
  ret = pre_frames[pframe];
  pframe++;
  return (ret);
}

void kfree_frame(pageframe_t a) { bm_clear((uint64_t)a / PAGE_SIZE); }

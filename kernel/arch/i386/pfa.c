#include <kernel/multiboot.h>
#include <kernel/page.h>
#include <stddef.h>
#include <stdint.h>

#define PAGE_SIZE 0x1000
#define ERROR 0x01
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

typedef unsigned int *pageframe_t;
pageframe_t pre_frames[20];

unsigned int mem_high(multiboot_info_t *mbd) {
  unsigned int high = 0;
  for (int i = 0; i < mbd->mmap_length; i += sizeof(multiboot_memory_map_t)) {
    multiboot_memory_map_t *mmmt =
        (multiboot_memory_map_t *)(mbd->mmap_addr + i);
    // printf("Start Addr: %x | Length: %x | Size: %x | Type: %d\n", mmmt->addr,
    //        mmmt->len, mmmt->size, mmmt->type);
    if (mmmt->type == MULTIBOOT_MEMORY_AVAILABLE) {
      /*
       * Do something with this memory block!
       * BE WARNED that some of memory shown as availiable is actually
       * actively being used by the kernel! You'll need to take that
       * into account before writing to memory!
       */
      if (mmmt->addr + mmmt->len > high) {
        high = mmmt->addr + mmmt->le;
      }
    }
  }
}

void pfa_init(multiboot_info_t *mbd) {
  npages = mem_high / PAGE_SIZE;

  // grab the next page aligned memory address after endkernel
  uintptr_t addr = ((uintptr_t)&endkernel + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
  bitmap = (uint32_t *)addr;

  uint32_t bitmap_size = ((npages + 31) / 32) * sizeof(uint32_t);
  // set all the pages to be free
  for (int i = 0; i < ((npages + 31) >> 5); ++i) {
    bitmap[i] = 0;
  }

  // mark everything up to end-of-bitmap as used
  uint32_t reserved = ((addr + bitmap_size) + PAGE_SIZE - 1) / PAGE_SIZE;
  for (uint32_t i = 0; i < reserved; ++i) {
    bm_set(i);
  }
}

// return physical address of the frame
// find the first availabe frame
pageframe_t kalloc_frame_int() {
  for (unsigned int i = 0; i < npages >> 5; ++i) {
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
  return nullptr;
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
      if (pre_frames[i] == nullptr) {
        // TODO: PANIC
      }
    }
    pframe = 0;
    allocate = 0;
  }
  ret = pre_frames[pframe];
  pframe++;
  return (ret);
}

void kfree_frame(pageframe_t a) { bm_clear((uint32_t)a / PAGE_SIZE); }

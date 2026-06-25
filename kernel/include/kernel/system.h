#ifndef __SYSTEM_H
#define __SYSTEM_H

#include <stdint.h>

typedef struct {
  uint32_t reserved;
  uint64_t rsp0;
  uint64_t rsp1; 
  uint64_t rsp2; 
  uint64_t reserved1; 

  uint64_t ist1; 
  uint64_t ist2; 
  uint64_t ist3; 
  uint64_t ist4; 
  uint64_t ist5; 
  uint64_t ist6; 
  uint64_t ist7; 

  uint64_t reserved2; 

  uint8_t reserved3;
  uint8_t iopb;
} tss_entry_t;

extern uint32_t *page_directory;
extern uint32_t *first_page_table;

void gdt_init();
void paging_init();

static inline void hcf(void) {
    for (;;) {
        asm("hlt");
    }
}

#endif

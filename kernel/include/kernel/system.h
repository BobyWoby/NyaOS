#ifndef __SYSTEM_H
#define __SYSTEM_H

#include <stdint.h>

typedef struct {
  uint32_t link;
  uint32_t esp0;
  uint32_t ssp0; // only first 2 bytes (0x0 and 0x1)
  uint32_t esp1;
  uint32_t ssp1; // only first 2 bytes (0x0 and 0x1)
  uint32_t esp2;
  uint32_t ssp2; // only first 2 bytes (0x0 and 0x1)
  uint32_t cr3;
  uint32_t eip;
  uint32_t eflags;
  uint32_t eax;
  uint32_t ecx;
  uint32_t edx;
  uint32_t ebx;
  uint32_t esp;
  uint32_t ebp;
  uint32_t esi;
  uint32_t edi;
  uint32_t es;   // only first 2 bytes (0x0 and 0x1)
  uint32_t cs;   // only first 2 bytes (0x0 and 0x1)
  uint32_t ss;   // only first 2 bytes (0x0 and 0x1)
  uint32_t ds;   // only first 2 bytes (0x0 and 0x1)
  uint32_t fs;   // only first 2 bytes (0x0 and 0x1)
  uint32_t gs;   // only first 2 bytes (0x0 and 0x1)
  uint32_t ldtr; // only first 2 bytes (0x0 and 0x1)
  uint32_t iopb; // only last 2 bytes (0x2 and 0x3)
  uint32_t ssp;
} tss_entry_t;

extern uint32_t page_directory[1024] __attribute__((aligned(4096)));
extern uint32_t first_page_table[1024] __attribute__((aligned(4096)));

void gdt_init();
void paging_init();

#endif

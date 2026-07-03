#ifndef __SYSTEM_H
#define __SYSTEM_H

#include <stdint.h>
#include <stdbool.h>

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

typedef struct lm_sysseg_desc {
  uint16_t limit_lo;
  uint16_t base_lo;
  uint8_t base_mid;
  uint8_t access_byte;
  uint8_t flags;
  uint8_t base_hi;
  uint32_t base_last;
  uint32_t reserved;
} lm_sysseg_desc_t;

extern uint64_t *pml4;
extern uint64_t *pdpt;
extern uint64_t *pd;
extern uint64_t *pt;

void gdt_init();

static inline void hcf(void) {
  for (;;) {
    asm("hlt");
  }
}

static inline void cpuid(int code, uint32_t *a, uint32_t *d) {
  asm volatile("cpuid" : "=a"(*a), "=d"(*d) : "a"(code) : "ecx", "ebx");
}

static inline int cpuid_string(int code, uint32_t where[4]) {
  asm volatile("cpuid"
               : "=a"(*where), "=b"(*(where + 1)), "=c"(*(where + 2)),
                 "=d"(*(where + 3))
               : "a"(code));
  return (int)where[0];
}

static const uint32_t CPUID_FLAG_MSR = 1 << 5;

static inline bool cpuHasMSR() {
  static uint32_t a, d;
  cpuid(1, &a, &d);
  return d & CPUID_FLAG_MSR;
}

static inline void cpuGetMSR(uint32_t msr, uint32_t *lo, uint32_t *hi) {
  asm volatile("rdmsr" : "=a"(*lo), "=d"(*hi) : "c"(msr));
}

static inline void cpuSetMSR(uint32_t msr, uint32_t lo, uint32_t hi) {
  asm volatile("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
}

#endif

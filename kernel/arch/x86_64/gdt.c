#include <kernel/system.h>
#include <stdint.h>
#include <string.h>

// why tf is it organized this way i have no idea
typedef struct gdt_entry {
  uint16_t limit_lo; // bits 0-15 of the limit
  uint16_t base_lo;  // bits 0-15 of the base
  uint8_t base_mid;  // bits 16-23 of the base
  // Access Byte
  // 7: Present bit
  // 6-5: Descriptor Privilege Level (DPL)
  // 4: Descriptor type bit
  // 3: Executable Bit (0 = data segment, 1 = code segment)
  // 2: Direction bit/Conforming bit
  // 1: R/W permissions
  // 0: Accesssed bit
  uint8_t access_byte;

  // flags
  // bits 0-3 = limit bits
  // 4-7 = flags
  // 7: Granularity flag (scales limit value by this much), 0 = 1 byte(byte
  // granularity), 1 = 4 KiB(page granularity)
  //
  // 6: Size flag
  //
  // 5: Long mode code flag (defines 64-bit code seg)
  //
  // 4: Reserved
  uint8_t flags;
  uint8_t base_hi;
} __attribute__((packed)) gdt_entry_t;

typedef struct {
  uint16_t limit;
  uint64_t base;
} __attribute__((packed)) gdt_ptr_t;

gdt_entry_t gdt[9];
gdt_ptr_t gp;
tss_entry_t tss_entry;

extern void flush_tss(void);
extern void flush_gdt(gdt_ptr_t *);

void encodeGDTEntry(int i, uint64_t base, uint32_t limit, uint8_t access,
                    uint8_t flags) {
  if (limit > 0xfffff) {
    // TODO: PANIC HERE
  }
  gdt[i].base_lo = base & 0xffff;
  gdt[i].base_mid = (base >> 16) & 0xff;
  gdt[i].base_hi = (base >> 24) & 0xff;

  gdt[i].limit_lo = limit & 0xffff;
  gdt[i].flags = (limit >> 16) & 0x0f;

  gdt[i].access_byte = access;
  gdt[i].flags |= flags << 4;
}

void write_tss(int idx) {
  uint64_t base = (uint64_t)&tss_entry;
  uint32_t limit = sizeof(tss_entry) - 1;

  encodeGDTEntry(idx, base, limit, 0x89, 0x0);
  gdt[idx + 1].base_lo = (base >> 32) & 0xffff;
  gdt[idx + 1].limit_lo = (base >> 48) & 0xffff;

  // gdt[i].base_last = (base >> 32);
  memset(&tss_entry, 0, sizeof(tss_entry));
  // tss_entry.ssp0 = 0x10;
}

void gdt_init() {
  gp.limit = sizeof(gdt) - 1;
  gp.base = (uint64_t)&gdt;

  // null descriptor
  encodeGDTEntry(0, 0, 0, 0, 0);

  // 32-bit kernel mode code segment
  encodeGDTEntry(1, 0, 0xfffff, 0x9a, 0xc);

  // 64-bit kernel mode code segment
  encodeGDTEntry(2, 0, 0xfffff, 0x9a, 0xa);

  // kernel mode data segment
  encodeGDTEntry(3, 0, 0xfffff, 0x92, 0xc);

  // user mode data segment
  encodeGDTEntry(4, 0, 0xfffff, 0xfa, 0xa);

  // 32-bit user mode code segment
  encodeGDTEntry(5, 0, 0xfffff, 0xf2, 0xc);

  // 64-bit user mode code segment
  encodeGDTEntry(6, 0, 0xfffff, 0xfa, 0xa);

  // TODO: Task state segment
  write_tss(7);

  flush_gdt(&gp);

  flush_tss();
}

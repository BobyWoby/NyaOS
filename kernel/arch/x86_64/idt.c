#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <kernel/idt.h>
#include <kernel/pic.h>
#include <kernel/isr.h>

#define IDT_MAX_DESCRIPTORS 256

typedef struct {
  uint16_t offset_lo; // offset is for ISR address
  uint16_t ss; // segment selector for kernel's cs
  uint8_t ist; // only bottom 3 bits used

  // Flag bits
  // 0-3: gate type
  //    0xe for 64-bit interrupt gate
  //    0xf for 64-bit trap gate
  // 4: 0
  // 5-6: DPL (privilege level)
  // 7: Present bit
  //
  // Common flag settings
  // 64-bit Interrupt Gate: 0x8E (p=1, dpl=0b00, type=0b1110 =>
  // flags=0b1000_1110=0x8E)
  //
  // 64-bit Trap Gate: 0x8F (p=1, dpl=0b00, type=0b1111 =>
  // flags=1000_1111b=0x8F)
  uint8_t attributes;
  uint16_t offset_mid;
  uint32_t offset_hi;
  uint32_t reserved;
} __attribute__((packed)) idt_entry_t;

typedef struct{
    uint16_t limit;
    uint64_t base;
}__attribute__((packed)) idt_ptr_t;

__attribute__((aligned(0x10)))
static idt_entry_t idt[256]; // aligned for performance

static idt_ptr_t idtr;

extern void* isr_stub_table[];

static bool vectors[IDT_MAX_DESCRIPTORS];


void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags){
    idt_entry_t *descriptor = &idt[vector];

    descriptor->offset_lo = (uint64_t)isr & 0xffff;
    descriptor->ss = (2 << 3) | 0;
    descriptor->ist = 0;
    descriptor->attributes = flags;
    descriptor->offset_mid = ((uint64_t)isr >> 16) & 0xffff;
    descriptor->offset_hi = ((uint64_t)isr >> 32) & 0xffffffff;
    descriptor->reserved = 0;
}

void idt_init(){
    idtr.base = (uintptr_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;

    for(uint8_t vector = 0; vector < 32; ++vector){
        idt_set_descriptor(vector, isr_stub_table[vector], 0x8e);
        vectors[vector] = true;
    }
    __asm__ volatile("lidt %0" : : "m"(idtr)); // load new IDT
    pic_init();
    __asm__ volatile ("sti"); // set interrupt flag
}

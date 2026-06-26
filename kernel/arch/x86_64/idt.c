#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint16_t offset_lo;
  uint16_t ss; // segment selector
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
  uint8_t flags;
  uint16_t offset_mid;
  uint32_t offset_hi;
  uint32_t reserved;
} __attribute__((packed)) idt_entry_t;

typedef uint64_t idt_ptr_t;

idt_ptr_t fill_idt_ptr(uint64_t offset, uint16_t size) {
  return (offset << 16) | (size);
}

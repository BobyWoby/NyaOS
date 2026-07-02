#include <kernel/apic.h>
#include <kernel/pager.h>
#include <kernel/pic.h>
#include <kernel/system.h>
#include <stdbool.h>
#include <stdint.h>

#define IA32_APIC_BASE_MSR 0x1B
#define IA32_APIC_BASE_MSR_BSP 0x100 // Processor is a BSP
#define IA32_APIC_BASE_MSR_ENABLE 0x800

// write 0 to here to send EOI
#define EOI_REG 0xB0

static uint64_t apic_offset;

bool check_apic() {
  uint32_t eax, edx;
  cpuid(1, &eax, &edx);
  return edx & 0x100; // bit 9 of edx checks if theres a local APIC
}

void cpu_set_apic_base(uintptr_t apic) {
  uint32_t edx = 0;
  uint32_t eax = (apic & 0xfffff0000) | IA32_APIC_BASE_MSR_ENABLE;

  edx = (apic >> 32) & 0x0f; // pretty sure this is always on in long mode

  cpuSetMSR(IA32_APIC_BASE_MSR, eax, edx);
}

uintptr_t cpu_get_apic_base() {
  uint32_t eax, edx;
  cpuGetMSR(IA32_APIC_BASE_MSR, &eax, &edx);

  return (eax & 0xfffff000) | ((uint64_t)(edx & 0x0f) << 32);
}

void write_reg(uint32_t data, uint32_t reg_offset) {
  outb(data, apic_offset + reg_offset);
}
uint32_t read_reg(uint32_t reg_offset) { return inl(apic_offset + reg_offset); }

void apic_init() {
  remap_irqs();
  pic_disable();
  // should map the base addr page as uncacheable
  uint64_t apic_offset = cpu_get_apic_base();

  uint64_t paddr = kalloc_frame();
  map_page(paddr, apic_offset, 0b11000); // UC is PAT3 in Limine
  cpu_set_apic_base(base);
  write_reg(read_reg(0xf0) | 0x100, 0xf0); // enable spurious interrupts
}

void send_eoi() { write_reg(0, EOI_REG); }

#include <kernel/apic.h>
#include <kernel/io.h>
#include <kernel/pager.h>
#include <kernel/pfa.h>
#include <kernel/pic.h>
#include <kernel/system.h>
#include <stdbool.h>
#include <stdint.h>

#define IA32_APIC_BASE_MSR 0x1B
#define IA32_APIC_BASE_MSR_BSP 0x100  // Processor is a BSP
#define IA32_APIC_BASE_MSR_ENABLE 0x800

// write 0 to here to send EOI
#define EOI_REG 0xB0

static volatile uintptr_t apic_offset;

bool check_apic() {
    uint32_t eax, edx;
    cpuid(1, &eax, &edx);
    return edx & 0x100;  // bit 9 of edx checks if theres a local APIC
}

void cpu_set_apic_base(uintptr_t apic) {
    uint32_t edx = 0;
    uint32_t eax = (apic & 0xfffff000) | IA32_APIC_BASE_MSR_ENABLE;

    edx = (apic >> 32) & 0x0f;  // pretty sure this is always on in long mode

    cpuSetMSR(IA32_APIC_BASE_MSR, eax, edx);
}

uintptr_t cpu_get_apic_base() {
    uint32_t eax, edx;
    cpuGetMSR(IA32_APIC_BASE_MSR, &eax, &edx);

    return (eax & 0xfffff000) | ((uint64_t)(edx & 0x0f) << 32);
}

void cpu_write_io_apic(void* ioapicaddr, uint32_t reg, uint32_t val) {
    uint32_t volatile* ioapic = (uint32_t volatile*)ioapicaddr;
    ioapic[0] = (reg & 0xff); // set addr reg
    ioapic[4] = value; // write to IOAPIC_BASE+0x10
}

uint32_t cpu_read_io_apic(void *ioapicaddr, uint32_t reg)
{
   uint32_t volatile *ioapic = (uint32_t volatile *)ioapicaddr;
   ioapic[0] = (reg & 0xff);
   return ioapic[4];
}

void write_reg(uint32_t data, uint32_t reg_offset) {
    *(volatile uint32_t*)(apic_offset + reg_offset) = data;
}
uint32_t read_reg(uint32_t reg_offset) { return *(volatile uint32_t*)(apic_offset + reg_offset); }

void apic_init() {
    remap_irqs();
    pic_disable();
    // should map the base addr page as uncacheable
    apic_offset = cpu_get_apic_base();

    // uint64_t* paddr = kalloc_frame();
    map_page((void*)apic_offset, (void*)apic_offset, 0b11000);  // UC is PAT3 in Limine

    cpu_set_apic_base(apic_offset);
    write_reg(read_reg(0xf0) | 0x100, 0xf0);  // enable spurious interrupts

    __asm__ volatile("sti");  // set interrupt flag
}

void send_eoi() { write_reg(0, EOI_REG); }

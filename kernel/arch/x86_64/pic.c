// #include <sys/io.h> //TODO: comment this out later
#include <kernel/io.h>
#include <kernel/pic.h>
#include <stdint.h>

#define PIC_PRIMARY_CMDSTAT 0x20
#define PIC_PRIMARY_INTRDATA 0x21
#define PIC_SECONDARY_CMDSTAT 0xa0
#define PIC_SECONDARY_INTRDATA 0xa1

void remap_irqs() {
    // remap IRQs to 32-47 since the first 31 are reserved by hardware
    uint8_t icw2 = 0x20;  // for primary PIC
    outb(icw2, PIC_PRIMARY_INTRDATA);

    icw2 = 0x28;  // for secondary PIC
    outb(icw2, PIC_SECONDARY_INTRDATA);
}

// Firmware/Limine leaves the CPU's Local APIC enabled. While it's enabled,
// the 8259 PIC's INTR line only reaches the core if LAPIC LINT0 is in ExtINT
// mode; otherwise every legacy IRQ is silently dropped. Since we drive
// interrupts through the PIC, disable the LAPIC (IA32_APIC_BASE bit 11) so
// external interrupts are delivered the legacy way.
static void lapic_disable() {
    uint32_t lo, hi;
    __asm__ volatile("rdmsr" : "=a"(lo), "=d"(hi) : "c"(0x1B));
    lo &= ~(1u << 11);
    __asm__ volatile("wrmsr" : : "a"(lo), "d"(hi), "c"(0x1B));
}

void pic_init() {
    lapic_disable();

    uint8_t icw1 = 0b00010001;
    outb(icw1, PIC_PRIMARY_CMDSTAT);
    outb(icw1, PIC_SECONDARY_CMDSTAT);

    remap_irqs();

    uint8_t icw3 = 0b00000100;  // use IRQ2 on the primary PIC
    outb(icw3, PIC_PRIMARY_INTRDATA);

    icw3 = 2;  // use IR line 2 for secondary pic
    outb(icw3, PIC_SECONDARY_INTRDATA);

    uint8_t icw4 = 0b1;
    outb(icw4, PIC_PRIMARY_INTRDATA);
    outb(icw4, PIC_SECONDARY_INTRDATA);

    // null out data registers (unmasks all the IRQs)
    outb(0, PIC_PRIMARY_INTRDATA);
    outb(0, PIC_SECONDARY_INTRDATA);

    IRQ_set_mask(0);
    IRQ_set_mask(8);
}

// 0b0010000 is the bit for an EOI request in ocw2
// void send_eoi(uint8_t irq) {
//     if (irq >= 8) outb(0x20, PIC_SECONDARY_CMDSTAT);
//     outb(0x20, PIC_PRIMARY_CMDSTAT);
// }

void pic_disable() {
    outb(0xff, PIC_PRIMARY_INTRDATA);
    outb(0xff, PIC_SECONDARY_INTRDATA);
}

void IRQ_set_mask(uint8_t IRQline) {
    uint16_t port;
    uint8_t value;

    if (IRQline < 8) {
        port = PIC_PRIMARY_INTRDATA;
    } else {
        port = PIC_SECONDARY_INTRDATA;
        IRQline -= 8;
    }
    value = inb(port) | (1 << IRQline);
    outb(value, port);
}

void IRQ_clear_mask(uint8_t IRQline) {
    uint16_t port;
    uint8_t value;

    if (IRQline < 8) {
        port = PIC_PRIMARY_INTRDATA;
    } else {
        port = PIC_SECONDARY_INTRDATA;
        IRQline -= 8;
    }
    value = inb(port) & ~(1 << IRQline);
    outb(value, port);
}

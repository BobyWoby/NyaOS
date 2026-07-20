#include <kernel/ioapic.h>
#include <kernel/apic.h>
#include <kernel/pager.h>
#include <kernel/requests.h>
#include <kernel/rsdt.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define ISA_IRQ_COUNT 16

void write_ioapic(const uintptr_t apic_base, const uint8_t offset, const uint32_t val) {
    *(volatile uint32_t*)(apic_base) = offset;
    *(volatile uint32_t*)(apic_base + 0x10) = val;
}

uint32_t read_ioapic(const uintptr_t apic_base, const uint8_t offset) {
    *(volatile uint32_t*)(apic_base) = offset;
    return *(volatile uint32_t*)(apic_base + 0x10);
}

void* find_table(char* signature) {
    struct limine_rsdp_response* rsdp_res = rsdp_request.response;
    uint64_t offset = hhdm_request.response->offset;

    XSDP_t* rsdp = (XSDP_t*)rsdp_res->address;

    if (rsdp->revision >= 2 && rsdp->xsdt_addr != 0) {
        XSDT_t* xsdt = (XSDT_t*)(rsdp->xsdt_addr + offset);
        int entries = (xsdt->header.length - sizeof(xsdt->header)) / 8;

        for (int i = 0; i < entries; ++i) {
            XSDT_header_t* h = (XSDT_header_t*)(xsdt->otherSDTPtr[i] + offset);
            if (!strncmp(h->signature, signature, 4)) {
                return (void*)h;
            }
        }
    } else {
        XSDT_header_t* rsdt = (XSDT_header_t*)((uint64_t)rsdp->rsdt_addr + offset);
        uint32_t* sdt_ptr = (uint32_t*)((uint8_t*)rsdt + sizeof(XSDT_header_t));
        int entries = (rsdt->length - sizeof(XSDT_header_t)) / 4;

        for (int i = 0; i < entries; ++i) {
            XSDT_header_t* h = (XSDT_header_t*)((uint64_t)sdt_ptr[i] + offset);
            if (!strncmp(h->signature, signature, 4)) {
                return (void*)h;
            }
        }
    }
    printf("No Table Found!\n");
    return NULL;
}

static void ioapic_route(uintptr_t base, uint32_t gsi_base, uint32_t gsi,
                         uint16_t iso_flags, uint8_t vector, uint8_t lapic_id) {
    uint32_t index = gsi - gsi_base;
    uint32_t reg = 0x10 + index * 2;

    uint32_t low = vector;

    // could set delivery mode here if we want to 

    if ((iso_flags & 0x3) == 0x3) low |= (1 << 13);
    if ((iso_flags & 0xc) == 0xc) low |= (1 << 15);
    uint32_t high = (uint32_t)lapic_id << 24;

    write_ioapic(base, reg + 1, high);
    write_ioapic(base, reg, low);
}

// Must run after apic_init() (needs the LAPIC mapped for read_reg).
void ioapic_init() {
    MADT_t* madt = (MADT_t*)find_table("APIC");
    if (!madt) {
        return;
    }

    uint64_t ioapic_phys = 0, gsi_base = 0;

    // Default ISA IRQ -> GSI identity map; overridden by any ISO entries.
    uint32_t irq_gsi[ISA_IRQ_COUNT];
    uint16_t irq_flags[ISA_IRQ_COUNT];
    for (int i = 0; i < ISA_IRQ_COUNT; ++i) {
        irq_gsi[i] = i;
        irq_flags[i] = 0;
    }

    uint8_t* p = (uint8_t*)madt + sizeof(MADT_t);
    uint8_t* end = (uint8_t*)madt + madt->hdr.length;

    while (p < end) {
        madt_record_hdr_t* entry = (madt_record_hdr_t*)p;
        if (entry->length == 0) break;  // guard against a malformed table
        switch (entry->type) {
            case 1: {  // IO APIC
                madt_ioapic_t* io = (madt_ioapic_t*)entry;
                ioapic_phys = io->ioapic_addr;
                gsi_base = io->gsi_base;
            } break;
            case 2: {  // Interrupt Source Override
                madt_iso_t* iso = (madt_iso_t*)entry;
                if (iso->irq_src < ISA_IRQ_COUNT) {
                    irq_gsi[iso->irq_src] = iso->gsi;
                    irq_flags[iso->irq_src] = iso->flags;
                }
            } break;
        }
        p += entry->length;
    }

    if (!ioapic_phys) {
        printf("No IO APIC found in MADT!\n");
        return;
    }

    // Map the IO APIC MMIO window uncacheable (UC = PAT3), like the LAPIC.
    uint64_t hhdm = hhdm_request.response->offset;
    uintptr_t ioapic_base = ioapic_phys + hhdm;
    map_page((void*)ioapic_phys, (void*)ioapic_base, 0b11000);

    // Destination = this CPU's LAPIC (ID in bits 24-31 of the LAPIC ID reg).
    uint8_t lapic_id = read_reg(0x20) >> 24;

    // Route PS/2 keyboard: ISA IRQ 1 -> vector 0x21.
    ioapic_route(ioapic_base, gsi_base, irq_gsi[1], irq_flags[1], 0x21, lapic_id);
}

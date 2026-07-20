#ifndef __IOAPIC_H
#define __IOAPIC_H
#include <kernel/requests.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// typedef struct XSDP {
//   char signature[8];
//   uint8_t check_sum;
//   char oemid[6];
//   uint8_t revision;
//   uint32_t rsdt_addr;
//
//   uint32_t len;
//   uint64_t xsdt_addr;
//   uint8_t extended_checksum;
//   uint8_t reserved[3];
//
// } __attribute__((packed)) XSDP_t;


void write_ioapic(const uintptr_t apic_base, const uint8_t offset, const uint32_t val);
uint32_t read_ioapic(const uintptr_t apic_base, const uint8_t offset);
void *find_table();
void ioapic_init();

#endif

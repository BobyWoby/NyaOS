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

void find_madt() {
  XSDP_t *acpi_tbl;
  struct limine_rsdp_respons *rsdp_res = rsdp->response;
  XSDP_t *rsdp = rsdp_res->address;

  uint64_t xsdt_address = rsdp->xsdt_add;
  uint64_t offset = hhdm->response->offset;
  XSDT_t *xsdt = (xsdt_address + offset);
  int entries = (xsdt->header.length - sizeof(xsdt->header)) / 8;

  for (int i = 0; i < entries; ++i) {
    XSDT_header_t *h = xsdt->otherSDTPtr[i];
    if (!strncmp(h->signature, "APIC", 4)) {
      // parse madt
    }
  }
  printf("No MADT Found!\n");
}

#endif

#ifndef __RSDT_H
#define __RSDT_H

#include <stddef.h>
#include <stdint.h>

typedef struct XSDP {
  char signature[8];
  uint8_t check_sum;
  char oemid[6];
  uint8_t revision;
  uint32_t rsdt_addr;

  uint32_t len;
  uint64_t xsdt_addr;
  uint8_t extended_checksum;
  uint8_t reserved[3];

} __attribute__((packed)) XSDP_t;

typedef struct XSDT_header {
  char signature[4];
  uint32_t length;
  uint8_t revision;
  uint8_t checksum;
  char OEMID[6];
  char OEMTableID[8];
  uint32_t OEMRevision;
  uint32_t creatorID;
  uint32_t creatorRevision;
} __attribute__((packed)) XSDT_header_t;

typedef struct XSDT {
  XSDT_header_t header;
  uint64_t otherSDTPtr[];
}__attribute__((packed)) XSDT_t;


typedef struct MADT {
    XSDT_header_t hdr;
    uint32_t lapic_addr;
    uint32_t flags;
}__attribute__((packed)) MADT_t ;

typedef struct MADT_record{
    uint8_t type;
    uint8_t length;
}__attribute__((packed)) madt_record_hdr_t;

typedef struct madt_ioapic_t{
    madt_record_hdr_t hdr;
    uint8_t ioapic_id;
    uint8_t reserved;
    uint32_t ioapic_addr;
    uint32_t gsi_base;
}__attribute__((packed)) madt_ioapic_t;

typedef struct madt_iso_t{
    madt_record_hdr_t hdr;
    uint8_t bus_src;
    uint8_t irq_src;
    uint32_t gsi;
    uint16_t flags;
}__attribute__((packed)) madt_iso_t;


#endif

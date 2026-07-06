#ifndef __IOAPIC_H
#define __IOAPIC_H

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
  // uint64_t otherSDTPtr[(header.length - sizeof(header)) / 8];
} XSDT_t;

#endif

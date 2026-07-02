#ifndef __APIC_H
#define __APIC_H
#include <stdint.h>

void apic_init();
void write_reg(uint32_t data, uint32_t reg_offset);
uint32_t read_reg(uint32_t reg_offset);

#endif

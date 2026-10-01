#ifndef __APIC_H
#define __APIC_H
#include <stdint.h>

void apic_init();
void send_eoi();
void write_reg(uint32_t data, uint32_t reg_offset);
uint32_t read_reg(uint32_t reg_offset);
void apic_start_timer(uint32_t divider, uint32_t sleep_micros);

#endif

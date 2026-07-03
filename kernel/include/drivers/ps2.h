#ifndef __PS2_H
#define __PS2_H
#include <stdint.h>

void ps2_init();
uint8_t recv();
uint8_t poll();
void send(uint8_t data, int port);

void dev1_enable_irq();
void dev2_enable_irq();
#endif

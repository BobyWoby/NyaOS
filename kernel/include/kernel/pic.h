#ifndef __PIC_H
#define __PIC_H
#include <stdint.h>

void remap_irqs();
void IRQ_set_mask(uint8_t IRQline);
void IRQ_clear_mask(uint8_t IRQline);
void pic_init();
void send_eoi(uint8_t irq);
void pic_disable();

#endif

#ifndef __PIC_H
#define __PIC_H

void pic_init();
void send_eoi(uint8_t irq);
void pic_disable();

#endif

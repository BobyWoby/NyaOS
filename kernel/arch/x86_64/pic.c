// #include <sys/io.h> //TODO: comment this out later
#include <kernel/pic.h>
#include <kernel/io.h>
#include <stdint.h>

#define PIC_PRIMARY_CMDSTAT 0x20
#define PIC_PRIMARY_INTRDATA 0x21
#define PIC_SECONDARY_CMDSTAT 0xa0
#define PIC_SECONDARY_INTRDATA 0xa1

void pic_init(){
    uint8_t icw1 = 0b00010001;
    outb(icw1, PIC_PRIMARY_CMDSTAT);
    outb(icw1, PIC_SECONDARY_CMDSTAT);

    // remap IRQs to 32-47 since the first 31 are reserved by hardware
    uint8_t icw2 = 0x20; // for primary PIC
    outb(icw2, PIC_PRIMARY_INTRDATA);

    icw2 = 0x28; // for secondary PIC
    outb(icw2, PIC_SECONDARY_INTRDATA);

    uint8_t icw3 = 0b00000100; // use IRQ2 on the primary PIC
    outb(icw3, PIC_PRIMARY_CMDSTAT);

    icw3 = 2; // use IR line 2 for secondary pic
    outb(icw3, PIC_SECONDARY_INTRDATA);

    uint8_t icw4 = 0b1;  
    outb(icw4, PIC_PRIMARY_INTRDATA);
    outb(icw4, PIC_SECONDARY_INTRDATA);

    // null out data registers
    outb(0, PIC_PRIMARY_INTRDATA);
    outb(0, PIC_SECONDARY_INTRDATA);
}

// 0b0010000 is the bit for an EOI request in ocw2
void send_eoi(uint8_t irq){
    if(irq >= 8) outb(0x20, PIC_SECONDARY_CMDSTAT);
    outb(0x20, PIC_PRIMARY_CMDSTAT);
}

void pic_disable(){
    outb(0xff, PIC_PRIMARY_INTRDATA);
    outb(0xff, PIC_SECONDARY_INTRDATA);
}

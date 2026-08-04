#include <drivers/serial.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <kernel/io.h>

void write_byte(uint16_t port, uint8_t offset, unsigned char byte){
    outb(byte, port + offset);
}
unsigned char read_byte(uint16_t port, uint8_t offset){
    return inb(port + offset);
}

// bit 7    bit 6               bits 5-3        bit 2       bits 1-0
// DLAB     Break Enable Bit    Parity bits     stop bits   data bits
void set_lcr(uint16_t port, unsigned char byte){
    write_byte(port, 3, byte);
}

unsigned char read_lcr(uint16_t port){
    return read_byte(port, 3);
}

void default_lcr(uint16_t port){
    set_lcr(port, 0b00000011); // 8N1 w/ DLAB off rn
}

bool serial_received(uint16_t port){
    return read_byte(port, 5) & 1;
}

char read_serial(uint16_t port){
    while(!serial_received(port)){}
    return read_byte(port, 0);
}

bool is_transmit_empty(uint16_t port){
    return read_byte(port, 5) & 0x20;
}

void write_serial(uint16_t port, unsigned char byte){
    while(!is_transmit_empty(port)){}
    write_byte(port, 0, byte);
}

void serial_init(uint16_t port, unsigned int baud){
    uint16_t divisor = CLOCK_RATE / baud;
    // disable inerrupts
    write_byte(port, 1, 0);

    // enable DLAB
    set_lcr(port, 0x80); 

    // set the baud rate divisor
    write_byte(port, 0, divisor & 0xff);
    write_byte(port, 1, divisor >> 8);
    // disable DLAB and set default LCR
    default_lcr(port);

    write_byte(port, 2, 0xc7);
    write_byte(port, 4, 0x0b);
    write_byte(port, 4, 0x1e); // test serial chip
    write_byte(port, 0, 0xae); // test serial chip
    if(read_byte(port, 0) != 0xae){
        printf("Serial is faulty.\n");
        return;
    }

    write_byte(port, 4, 0x0f);
}

void handle_serial(int vector){
    // needs heap to be able to have a write queue
    if(vector == 0x22){
        // COM2/4
    }else if(vector == 0x23){
        // COM1/3
    }
}

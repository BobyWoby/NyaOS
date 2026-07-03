#include <drivers/ps2.h>
#include <kernel/io.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
// #include <sys/io.h>  //TODO: Comment this out

// used for reading data received from ps/2 device
#define DATA 0x60

#define STATUS 0x64

// sends commands to ps/2 controller (not devices)
#define COMMAND 0x64

void send(uint8_t data, int port);
uint8_t poll();

void write(uint8_t data, uint8_t port) {
    while (inb(STATUS) & 0x2);
    outb(data, port);
}

uint8_t read() {
    while (!(inb(STATUS) & 0x1));
    return inb(DATA);
}

bool check_aml() { return true; }

void disable_devices() {
    write(0xad, COMMAND);
    write(0xa7, COMMAND);
}

uint8_t detect_device(uint8_t port){
    send(0xf5, port);
    while(poll() != 0xfa){
    }
    send(0xf2, port);
    while(poll() != 0xfa){
    }
    return poll();
}

void enable_scanning(uint8_t port){
    send(0xf4, port);
}

void dev2_enable_irq(){
    write(0x20, COMMAND);
    uint8_t cfg = read();
    cfg |= (0x2);
    write(0x60, COMMAND);
    write(cfg, DATA);
}

void dev1_enable_irq(){
    write(0x20, COMMAND);
    uint8_t cfg = read();
    cfg |= (0x1);
    // if (dual_channel) cfg |= 0x2;
    write(0x60, COMMAND);
    write(cfg, DATA);
}

void ps2_init() {
    if (!check_aml()) {
        printf("No PS/2 Detected\n");
        return;
    }
    disable_devices();
    inb(DATA);  // just flushing so we don't actually care if there's smt here

    // Set Controller Configuration Byte
    write(0x20, COMMAND);
    uint8_t cfg = read();
    cfg &= ~(0b1010001);
    write(0x60, COMMAND);
    write(cfg, DATA);

    // Controller self test
    write(0xaa, COMMAND);
    if (read() != 0x55) {
        printf("PS/2 self test failed\n");
        return;
    }
    // NOTE: Might be unnecessary
    write(0x60, COMMAND);
    write(cfg, DATA);

    // Check if there are two channels
    bool dual_channel = false;
    write(0xa8, COMMAND);
    write(0x20, COMMAND);
    if (!((cfg = read()) & 0x20)) {
        dual_channel = true;
        write(0xa7, COMMAND);
        cfg &= ~(0b100010);
        write(0x60, COMMAND);
        write(cfg, DATA);
    }

    // interface tests
    write(0xab, COMMAND);
    uint8_t res;
    if ((res = read()) != 0x00) {
        printf("PS/2 first port test failed with code %X\n", res);
        return;
    }
    if (dual_channel) {
        printf("Dual Channel PS/2 Controller Detected\n");
        write(0xa9, COMMAND);
        if (read() != 0x00) {
            printf("PS/2 second port test failed\n");
            return;
        }
    }

    // enable devices
    write(0xae, COMMAND);
    if (dual_channel) write(0xa8, COMMAND);

    send(0xff, 0);
    uint8_t did = poll();  // ACK (0xfa)
    if (did != 0xfa && did != 0xaa) {
        printf("PS/2 first port device reset failed\n");
        return;
    }
    poll();  // self-test result (0xaa)
    // drain any remaining bytes without blocking
    while (inb(STATUS) & 0x1) inb(DATA);
    printf("PS/2 first port device reset\n");

    if (dual_channel) {
        while (inb(STATUS) & 0x1) inb(DATA);   
        send(0xff, 1);
        uint8_t did = poll();
        if (did != 0xfa && did != 0xaa) {
            printf("PS/2 second port device reset failed\n");
            return;
        }
        poll();  // self-test result (0xaa)
        // drain any remaining bytes (mouse also sends its device id)
        while (inb(STATUS) & 0x1) inb(DATA);
        printf("PS/2 second port device reset\n");
    }

    uint8_t did1 = detect_device(0);
    printf("PS/2 first port device ID: %x\n", did1);
    while (inb(STATUS) & 0x1) printf(" %x\n", inb(DATA));   

    uint8_t did2 = detect_device(1);
    printf("PS/2 second port device ID: %x\n", did2);
    while (inb(STATUS) & 0x1) printf(" %x\n", inb(DATA));   
}

uint8_t poll() {
    while (!(inb(STATUS) & 0x1)) {
    }
    return inb(DATA);
}

// this is blocking rn, might wanna change that at some point
// port is either 0 for the first port, or 1 for the second
void send(uint8_t data, int port) {
    if (!port) {
        while (inb(STATUS) & 0x2) {
        }
        outb(data, DATA);
    } else {
        outb(0xd4, COMMAND);
        while (inb(STATUS) & 0x2);
        outb(data, DATA);
    }
    return;
}

uint8_t recv() { return inb(DATA); }

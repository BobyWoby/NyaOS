#ifndef __SERIAL_H
#define __SERIAL_H

#define COM1 0x3f8
#define COM2 0x2f8
#define COM3 0x3e8
#define COM4 0x2e8
#define COM5 0x5f8
#define COM6 0x4f8
#define COM7 0x5e8
#define COM8 0x4e8

#define CLOCK_RATE 115200

void handle_serial(int vector);

#endif

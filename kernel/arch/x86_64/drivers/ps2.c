#include <kernel/io.h>
#include <stdint.h>
#include <stdio.h>

// used for reading data received from ps/2 device
#define DATA 0x60

#define STATUS 0x64

// sends commands to ps/2 controller (not devices)
#define COMMAND 0x64

void send(uint8_t data, int port);
uint8_t poll();

void set_config(uint8_t mask, bool and) {
  outb(0x20, COMMAND);
  while (inb(STATUS) & 0x3 != 0x1) {
    // make sure the command was sent and the response was received
  }
  uint8_t config = inb(DATA);
  // clear bits 6, 4, and 0
  if (and) {
    config &= mask;
  } else {
    config |= mask;
  }

  outb(config, DATA);
  outb(0x60, COMMAND);

  while (inb(STATUS) & 0x2) {
  }
}

void ps2_init() {
  // disable devices during initialization
  outb(0xad, COMMAND);
  outb(0xa7, COMMAND);

  inb(DATA);

  set_config(~(0b1010001), true);

  // TODO: perform a controller self test here
  outb(0xaa, COMMAND);
  while (inb(STATUS) & 0x3 != 0x1) {
    // make sure the command was sent and the response was received
  }
  if (inb(DATA) != 0x55) {
    printf("PS/2 Controller Self Test Failed\n");
  }

  outb(0xa8, COMMAND);

  outb(0x20, COMMAND);
  while (inb(STATUS) & 0x3 != 0x1) {
    // make sure the command was sent and the response was received
  }
  config = inb(DATA);
  if (config & 0x10) {
    // dual channel device
    outb(0xa7, COMMAND);
    set_config(~(0b100010), true);
  }
  // TODO: Perform interface tests

  // re-enable devices
  outb(0xae, COMMAND);
  outb(0xa8, COMMAND);
  // need to re-enable interrupts here
  set_config(0b11, false);

  send(0xff, 0);
  send(0xff, 1);
  uint8_t response = poll();
  if (response == 0xfa) {
    response = poll();
    // check for 0xaa
    uint8_t ps2id = poll();
  } else {
    printf("PS/2 response is wrong\n");
  }
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
    while (inb(STATUS) & 0x2)
      ;
    outb(data, DATA);
  }
  return;
}

uint8_t read() { return inb(DATA); }

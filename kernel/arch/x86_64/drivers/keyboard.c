#include <drivers/keyboard.h>
#include <drivers/ps2.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define ACK 0xfa
#define RESEND 0xfe
#define PORT 0

typedef enum{
    READY = 0,
} KEYBOARD_STATE;

static KEYBOARD_STATE state;

typedef struct node{
    uint8_t cmd;
   struct node *next; 
} kb_cmd_node_t;

typedef struct queue{
    kb_cmd_node_t *_head;
    kb_cmd_node_t *_tail;
    unsigned int _size;
}kb_cmd_queue_t;

static kb_cmd_queue_t queue;

void kb_push(uint8_t cmd){
}

void handle_keyboard() {
    uint8_t scancode = recv();  // read + re-arm IRQ1
    switch (state) {
        case READY:
            scancode = recv();
            break;
    }
    printf("scancode: %#x\n", scancode);
}

void kb_enable_scanning() {
    send(0xf0, PORT);
    uint8_t res = 0;
    uint8_t scancode;

    res = poll();
    if (res == 0xfa) {
        send(0, PORT);
        if(poll() == 0xfa){
            scancode = poll();
            printf("Enabling scanning on keyboard with scancode set %x\n", scancode);
        }
    }

    res = 0;
    do {
        send(0xf4, PORT);
    } while ((res = poll()) == 0xfe);

    state = READY;

    // enable interrupts for the ps/2 device 1
    dev1_enable_irq();
}

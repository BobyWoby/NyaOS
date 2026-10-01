#include <drivers/keyboard.h>
#include <drivers/ps2.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <kernel/slab.h>

#define ACK 0xfa
#define RESEND 0xfe
#define PORT 0

typedef enum{
    READY = 0,
    MAKE_0,
    EXTENDED,
    EXTENDED_BREAK
} 
KEYBOARD_STATE; 

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
static kmem_cache *kb_cmd_cache;

void handle_keyboard() {
    uint8_t scancode;
    // scancode = recv();  // read + re-arm IRQ1
    scancode = recv();
    switch(state){
    }

    printf("scancode: %#x\n", scancode);
}

void kb_enable_scanning() {
    // initialize keyboard structures
    kb_cmd_cache = kmem_cache_create("kb_cmd", sizeof(kb_cmd_node_t), 0);

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

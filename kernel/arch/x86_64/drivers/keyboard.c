#include <drivers/ps2.h>
#include <drivers/keyboard.h>
#include <stdint.h>
#include <stdbool.h>

#define ACK 0xfa
#define RESEND 0xfe
#define PORT 0

void kb_enable_scanning(){
    uint8_t res =0;
    do{
        send(0xf4, PORT);
    }while((res = poll()) == 0xfe);
}

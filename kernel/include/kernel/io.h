#ifndef __IO_H
#define __IO_H

#include <stddef.h>
#include <stdint.h>

static inline unsigned char inb(unsigned short port){
    uint8_t ret;
    __asm__ volatile("inb %w1, %b0" : "=a"(ret) : "Nd"(port) : "memory");
    return ret;
}

unsigned char inb_p(unsigned short port);

static inline unsigned short inw(unsigned short port){
    uint16_t ret;
    __asm__ volatile("inw %w1, %w0" : "=a"(ret) : "Nd"(port) : "memory");
    return ret;
}

unsigned short inw_p(unsigned short port);

static inline unsigned int inl(unsigned short port) {
    uint32_t ret;
    __asm__ volatile("inl %w1, %k0" : "=a"(ret) : "Nd"(port) : "memory");
    return ret;
}
unsigned int inl_p(unsigned short port);

static inline void outb(unsigned char value, unsigned short port) {
    __asm__ volatile("outb %b0, %w1" : : "a"(value), "Nd"(port) : "memory");
}

void outb_p(unsigned char value, unsigned short port);

static inline void outw(unsigned short value, unsigned short port){
    __asm__ volatile("outw %w0, %w1" : : "a"(value), "Nd"(port) : "memory");
};
void outw_p(unsigned short value, unsigned short port);

static inline void outl(unsigned int value, unsigned short port){
    __asm__ volatile("outl %k0, %w1" : : "a"(value), "Nd"(port) : "memory");
};

void outl_p(unsigned int value, unsigned short port);
#endif

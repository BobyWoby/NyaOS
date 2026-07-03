#ifndef  __ISR_H
#define __ISR_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;  // common-stub pushes
    uint64_t vector, error_code;                 // stub pushes
    uint64_t rip, cs, rflags, rsp, ss;           // CPU pushes
} registers_t;

void exception_handler(registers_t *regs);


#endif

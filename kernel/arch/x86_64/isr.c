#include <kernel/isr.h>
#include <drivers/keyboard.h>
#include <kernel/pic.h>
#include <kernel/io.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

// typedef struct {
//     uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
//     uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;  // common-stub pushes
//     uint64_t vector, error_code;                 // stub pushes
//     uint64_t rip, cs, rflags, rsp, ss;           // CPU pushes
// } registers_t;

void page_fault(uint64_t vaddr) { printf("faulting addr (CR2)=%#llx\n", vaddr); }




void exception_handler(registers_t* regs) {
    // Hardware IRQs (remapped to 0x20-0x2f): dispatch quietly and return.
    if (regs->vector >= 0x20 && regs->vector <= 0x2f) {
        if (regs->vector == 0x21) handle_keyboard();
        send_eoi(regs->vector - 0x20);
        return;
    }

    // CPU exceptions: dump state and halt.
    printf("\n*** EXCEPTION %llu\n", regs->vector);
    printf("err=%#llx  RIP=%#llx  CS=%#llx  RFLAGS=%#llx\n", regs->error_code, regs->rip, regs->cs,
           regs->rflags);
    printf("RAX=%#llx RBX=%#llx ... RSP=%#llx\n", regs->rax, regs->rbx, regs->rsp);

    if (regs->vector == 14) {  // page fault
        uint64_t cr2;
        __asm__ volatile("mov %%cr2, %0" : "=r"(cr2));
        page_fault(cr2);
    }
    __asm__ volatile("cli; hlt");  // hangs
}

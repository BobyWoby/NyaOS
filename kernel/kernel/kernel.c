// #include <kernel/multiboot.h>
#include <drivers/keyboard.h>
#include <drivers/ps2.h>
#include <kernel/apic.h>
#include <kernel/idt.h>
#include <kernel/ioapic.h>
#include <kernel/limine.h>
#include <kernel/pager.h>
#include <kernel/pfa.h>
#include <kernel/system.h>
#include <kernel/tty.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "kernel/slab.h"
#include "stdlib.h"

__attribute__((used, section(".limine_requests"))) static volatile uint64_t limine_base_revision[] =
    LIMINE_BASE_REVISION(6);

__attribute__((
    used,
    section(".limine_requests_start"))) static volatile uint64_t limine_requests_start_marker[] =
    LIMINE_REQUESTS_START_MARKER;

__attribute__((
    used, section(".limine_requests_end"))) static volatile uint64_t limine_requests_end_marker[] =
    LIMINE_REQUESTS_END_MARKER;

extern void enable_sse();
extern void jump_usermode();

void key_down(KEY kc) {
    char c;
    if (kc >= A && kc <= Z) {
        c = kc - A + 'a';
        if (key_states[LSHIFT] || caps_lock) {
            c -= 'a' - 'A';
        }
        printf("%c", c);
    } else if (kc == RET) {
        printf("\n");
    } else if (kc == SPACE) {
        printf(" ");
    }
}

void test_user_function() {
    // asm("cli");
    while (true) {
    };
}

static const uint8_t user_code[] = {
    0xfa,       // CLI should fail
    0xeb, 0xfe  // short jump to -2 instructions from this + 1
};

const uintptr_t UC_VA = 0x400000;
const uintptr_t UC_STK_BASE = 0x700000;

void setup_usercode(uintptr_t user_code_va, uintptr_t code_stack_base) {
    uintptr_t code_frame = (uintptr_t)kalloc_frame();
    void* code_kernel_ptr = (void*)phys_to_virt((uintptr_t)code_frame);
    memset(code_kernel_ptr, 0, PAGE_SIZE);
    memcpy(code_kernel_ptr, user_code, sizeof(user_code));

    uintptr_t code_va = user_code_va;
    if (map_page(code_frame, code_va, PAGE_RW | PAGE_US) == -1) {
        abort();
    }

    uintptr_t stack_frame = (uintptr_t)kalloc_frame();
    memset((void*)phys_to_virt(stack_frame), 0, PAGE_SIZE);
    if (map_page(stack_frame, code_stack_base, PAGE_RW | PAGE_US) == -1) abort();
}

extern void jump_usermode(uintptr_t entry, uintptr_t stack_top)
    __attribute__((noreturn));

void kernel_main() {
    if (LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision) == false) {
        hcf();
    }

    enable_sse();
    gdt_init();
    terminal_initialize();

    pfa_init();

    if (parse_madt() == -1) {
        printf("Failed to parse MADT\n");
        hcf();
    };
    paging_init();
    idt_init();

    slab_alloc_init();

    apic_init();
    apic_start_timer(0x3, 1000000);
    ioapic_init();

    printf("UwU Hallo %sUwu\nWelcome to NyaOS\n", ":3");
    ps2_init();
    kb_enable_scanning();
    key_down_handler = key_down;

    setup_usercode(UC_VA, UC_STK_BASE);
    jump_usermode(UC_VA, UC_STK_BASE + PAGE_SIZE);

    // spin forever ig
    hcf();
}

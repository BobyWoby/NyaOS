// #include <kernel/multiboot.h>
#include <kernel/limine.h>
#include <kernel/pfa.h>
#include <kernel/system.h>
#include <kernel/idt.h>
#include <kernel/tty.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

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

void kernel_main() {
    if (LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision) == false) {
        hcf();
    }

    enable_sse();
    gdt_init();
    idt_init();

    terminal_initialize();

    // gdt_init();
    // // set up pfa here
    // pfa_init();
    // paging_init();
    //
    // hcf();
    printf("UwU Hallo %sUwu\nHow are you?", ":3");
    pfa_init();
    hcf();
}

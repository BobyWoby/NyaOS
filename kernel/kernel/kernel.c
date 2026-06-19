#include <kernel/tty.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <kernel/system.h>

void kernel_main(void) {
    gdt_init();
    terminal_initialize();
    printf("UwU Hallo %s Uwu\n", ":3");
}

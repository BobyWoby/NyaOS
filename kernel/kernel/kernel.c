#include <stdio.h>
#include <kernel/tty.h>

void kernel_main(void) {
    terminal_initialize();
    printf("UwU Hallo %s Uwu\n", ":3");
}

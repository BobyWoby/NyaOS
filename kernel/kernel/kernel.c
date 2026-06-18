#include <kernel/tty.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

void kernel_main(void) {
  terminal_initialize();
  printf("UwU Hallo %s Uwu\n", ":3");
}

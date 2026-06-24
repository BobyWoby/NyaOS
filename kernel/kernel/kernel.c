#include <kernel/multiboot.h>
#include <kernel/pfa.h>
#include <kernel/system.h>
#include <kernel/tty.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

void kernel_main(multiboot_info_t *mbd, unsigned int magic) {
  gdt_init();
  // set up pfa here
  pfa_init(mbd);
  paging_init();

  terminal_initialize();
  printf("UwU Hallo %s Uwu\n", ":3");
}

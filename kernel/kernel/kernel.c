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
#include "kernel/slab.h"

__attribute__((used, section(".limine_requests"))) static volatile uint64_t
    limine_base_revision[] = LIMINE_BASE_REVISION(6);

__attribute__((used,
               section(".limine_requests_start"))) static volatile uint64_t
    limine_requests_start_marker[] = LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end"))) static volatile uint64_t
    limine_requests_end_marker[] = LIMINE_REQUESTS_END_MARKER;

extern void enable_sse();

void key_down(KEY kc){
    char c;
    if(kc >= A && kc <= Z){
        c = kc - A + 'a';
        if(key_states[LSHIFT] || caps_lock){
            c -= 'a' - 'A';
        }
        printf("%c", c);
    }else if(kc == RET){
        printf("\n");
    }else if(kc == SPACE){
        printf(" ");
    }
    
}

void kernel_main() {
  if (LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision) == false) {
    hcf();
  }

  enable_sse();
  gdt_init();
  terminal_initialize();

  pfa_init();
  paging_init();
  idt_init();

  slab_alloc_init();

  apic_init();
  ioapic_init();

  printf("UwU Hallo %sUwu\nWelcome to NyaOS\n", ":3");
  ps2_init();
  kb_enable_scanning();
  key_down_handler = key_down;

  // spin forever ig
  hcf();
}

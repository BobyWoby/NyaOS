#include <kernel/tty.h>
#include <kernel/multiboot.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <kernel/system.h>

void kernel_main(multiboot_info_t* mbd, unsigned int magic) {
    gdt_init();
    // set up pfa here
    
    paging_init();

    terminal_initialize();
    for(int i = 0; i < mbd->mmap_length; i += sizeof(multiboot_memory_map_t)){
        multiboot_memory_map_t* mmmt = (multiboot_memory_map_t*) (mbd->mmap_addr + i);
        printf("Start Addr: %x | Length: %x | Size: %x | Type: %d\n",
            mmmt->addr, mmmt->len, mmmt->size, mmmt->type);
        if(mmmt->type == MULTIBOOT_MEMORY_AVAILABLE) {
            /* 
             * Do something with this memory block!
             * BE WARNED that some of memory shown as availiable is actually 
             * actively being used by the kernel! You'll need to take that
             * into account before writing to memory!
             *
             * Add to running sum of memory here though
             */
        }
    }
    printf("UwU Hallo %s Uwu\n", ":3");
}

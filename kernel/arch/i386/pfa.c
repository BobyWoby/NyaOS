#include <kernel/page.h>
#include <stddef.h>
#include <stdint.h>

#define FREE 0x00
#define USED 0x01
#define ERROR 0x01
#define NUM_PAGES 20

typedef uintptr_t pageframe_t;

uint32_t frame_map[NUM_PAGES];


static pageframe_t kalloc_frame_int(){
    uint32_t i = 0;
    while(frame_map[i] != FREE){
        if(++i == NUM_PAGES){
            return (ERROR);
        }
    }
    frame_map[i] = USED;
    // return (startframe + (i * 0x1000));
}


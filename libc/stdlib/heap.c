#include <kernel/pager.h>
#include <kernel/pfa.h>
#include <kernel/system.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define PAGE_SIZE 4096
#define MAGIC 0xDEADC0DE

// TODO: unocmment the macros
// might change this later cuz this might run into limine structures
extern char kernel_end_virtual;

// #if defined (__is_libk)

typedef struct node {
    struct node* next;
    uint32_t size;
} __attribute__((packed)) heap_node_t;

uint64_t* heap;
heap_node_t* head;
char *heap_end;


void heap_init() {
    map_page(kalloc_frame(), &kernel_end_virtual, 0);
    heap = (uint64_t *)&kernel_end_virtual;
    head = (heap_node_t *)heap;
    head->next = NULL;
    head->size = PAGE_SIZE - sizeof(heap_node_t);
    heap_end = (char *)(head + PAGE_SIZE);
}
// #endif

void free(void * p){
    heap_node_t *ptr = p - sizeof(heap_node_t);
    if(ptr->next == NULL){
        ptr->size = (uint64_t)heap_end - (uint64_t)p;
    }else{
        ptr->size = (uint64_t)ptr->next - (uint64_t)p;
    }
    
    // should prolly coalesce the list here
    heap_node_t *tmp = head;
    while(tmp != NULL){
        if(tmp->size != 0xDEADC0DE){
            
        }
        tmp = tmp->next;
    }
}


void* malloc(size_t size) {
    // #if defined (__is_libk)
    heap_node_t *ptr = head;
    while(ptr != NULL){
        if(ptr->size != 0xDEADC0DE && size < ptr->size){
            // split the list here
            ptr->size -= size;
            heap_node_t *tmp = (heap_node_t *)((uint64_t)ptr + sizeof(heap_node_t) + size);
            tmp->size = ptr->size - sizeof(heap_node_t) - size;
            tmp->next = ptr->next;
            ptr->next = tmp;
            ptr->size = 0xDEADC0DE;

            return (void *)((uint64_t)ptr + sizeof(heap_node_t));
        }
        ptr = ptr->next;
    }
    // could also allocate a new page if we run out of space
    printf("malloc error: OOM\n");
    return NULL;
    // #else
    // // TODO: implement user space malloc
    // #endif
}

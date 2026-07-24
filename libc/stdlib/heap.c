#include <kernel/pager.h>
#include <kernel/pfa.h>
#include <kernel/system.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define PAGE_SIZE 4096
#define MAGIC 0xDEADBEEF

// TODO: unocmment the macros
// might change this later cuz this might run into limine structures
extern char kernel_end_virtual;

// #if defined (__is_libk)

typedef struct node {
    struct node* next;
    uint32_t size;
} __attribute__((packed)) heap_node_t;

typedef struct used_node {
    uint32_t magic;
    uint32_t size;
} __attribute__((packed)) used_block_t;

uint64_t* heap;
heap_node_t* head; // head of the free list
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
        if(tmp->size != MAGIC){
            
        }
        tmp = tmp->next;
    }
}


void* malloc(size_t size) {
    // #if defined (__is_libk)
    heap_node_t *ptr = head;
    while(ptr != NULL){
        if(ptr->size != MAGIC && size < ptr->size){
            heap_node_t *tmp = (heap_node_t *)((uint64_t)ptr + sizeof(heap_node_t) + size);
            tmp->size = ptr->size - size - sizeof(heap_node_t);
            tmp->next = ptr->next;
            if(ptr == head){
                head = tmp;
            }
            used_block_t *header = (used_block_t *)ptr;
            header->size = size;
            header->magic = MAGIC;
            return (void *)((uint64_t)header + sizeof(used_block_t));
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

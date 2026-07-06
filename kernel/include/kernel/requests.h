#ifndef __REQUESTS_H
#define __REQUESTS_H
#include <kernel/limine.h>
#include <stdint.h>

extern volatile struct limine_hhdm_request hhdm_request;
extern volatile struct limine_framebuffer_request framebuffer_request;
extern volatile struct limine_module_request module_request;
extern volatile struct limine_memmap_request mmap_request;
extern volatile struct limine_executable_address_request eaddr_request;
extern volatile struct limine_rsdp_request rsdp_request;

#endif

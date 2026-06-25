#include <kernel/pfa.h>
#include <kernel/system.h>
#include <stdint.h>

extern void loadPageDirectory(uint32_t *);
extern void enablePaging();
uint32_t *page_directory;
uint32_t *first_page_table;

void fill_first_page_table() {
  // holds physical address to start mapping pages to
  // here, we map pages to the very beginning of memory
  unsigned int i;

  // fill all entries in the table to map 4 megabytes
  for (i = 0; i < 1024; ++i) {
    // since address is page aligned, it will leave 12 bits zeroed.
    // zeroed bits used by the attributes
    first_page_table[i] =
        (i * 0x1000) | 3; // attributes: supervisor level, read/write, present
  }
}

void paging_init() {
  page_directory = kalloc_frame();
  int i;
  for (i = 0; i < 1024; ++i) {
    // This sets the following flags to the pages:
    //   Supervisor: Only kernel-mode can access them
    //   Write Enabled: It can be both read from and written to
    //   Not Present: The page table is not present
    page_directory[i] = 0x00000002;
  }

  first_page_table = kalloc_frame();

  fill_first_page_table();

  page_directory[0] = ((unsigned int)first_page_table) | 3;

  loadPageDirectory(page_directory);
  enablePaging();
}

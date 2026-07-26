# NyaOS - :3 (WIP)
## Done
- [x] adding integer printing to printf
- [x] interrupt/exception handling(APIC)
- [x] Page frame allocator(bitmap)
- [x] global descriptor table
- [x] PS/2 Controller Driver
    - Proper Keyboard depends on the heap

- [x] Paging setup, very primitive right now though
## TODO

- [ ] unmap_page implementation
- [ ] Slab allocator for kmalloc

## Further out goals (in no particular order)

- user space
- Round robin scheduler
- graphics?
- filesystem (ZFS?)
- multithreading
    - multithreaded paging (has other dependencies though)

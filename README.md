# NyaOS - :3 (WIP)

## TODO

- [x] adding integer printing to printf
- [x] interrupt/exception handling(APIC)
- [x] Page frame allocator(bitmap)
- [x] global descriptor table
- [x] PS/2 Controller Driver
    - Proper Keyboard depends on the heap
- [ ] virtual memory mapping
    - Paging setup, very primitive right now though
    - TODO: multithreaded paging (has other dependencies though)
- [ ] Heap
    - Current
    - kinda wanna build a slab allocator

## Further out goals (in no particular order)

- user space
- Round robin scheduler
- graphics?
- filesystem (ZFS?)
- multithreading

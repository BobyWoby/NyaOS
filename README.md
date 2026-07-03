# NyaOS - :3 (WIP)

Text rendering is currently assuming that the pixel format is 0x00RRGGBB

## TODO

- [x] adding integer printing to printf
- [x] interrupt/exception handling(very basic rn tho)
- [x] Page frame allocator
- [x] global descriptor table
- [x] PS/2 Controller Driver
    - Proper Keyboard depends on the heap
- [ ] virtual memory mapping
    - Paging setup, very primitive right now though
    - TODO: multithreaded paging (has other dependencies though)
- [ ] Heap
    - Current

## Further out goals (in no particular order)

- user space
- Round robin scheduler
- graphics?
- filesystem (ZFS?)
- multithreading

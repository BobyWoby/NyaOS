# Architecture and source map

[README](../README.md) · [Getting started](getting-started.md) · [Development](development.md)

## Find the relevant code

| Location | Responsibility |
| --- | --- |
| [kernel/kernel/kernel.c](../kernel/kernel/kernel.c) | Kernel entry point and initialization order |
| [kernel/arch/x86_64/](../kernel/arch/x86_64/) | Memory management, CPU setup, interrupts, framebuffer terminal |
| [kernel/arch/x86_64/drivers/](../kernel/arch/x86_64/drivers/) | PS/2 controller, keyboard, and serial routines |
| [kernel/include/kernel/](../kernel/include/kernel/) | Kernel interfaces, Limine protocol, and hardware structures |
| [kernel/include/drivers/](../kernel/include/drivers/) | Driver interfaces |
| [libc/](../libc/) | Small C library; the default build produces the kernel variant `libk.a` |
| [libc/include/](../libc/include/) | Project C library headers |
| [kernel/arch/x86_64/make.config](../kernel/arch/x86_64/make.config) | Architecture flags and explicit kernel object list |
| [kernel/arch/x86_64/linker.ld](../kernel/arch/x86_64/linker.ld) | Kernel layout, entry symbol, and retained Limine requests |
| [limine.conf](../limine.conf) | Boot menu, kernel path, and font module path |
| [limine-binary/](../limine-binary/) | Bundled bootloader assets and native installer source |
| [minecraft.psf](../minecraft.psf) | Bitmap font loaded as the first boot module |

`Minecraft.ttf` and `tamzen.psf` are also present, but `iso.sh` packages
`minecraft.psf`. The Multiboot headers in `kernel/include/kernel/` are not the
active boot protocol. Root-level `tmp.log` and `tmp.txt` are existing diagnostic
artifacts, not build inputs or an automated test suite.

## Follow a boot

Limine loads the ELF kernel, provides requested boot information, and enters
`kernel_main` directly, as specified by the linker script. `boot.S` contains
assembly helpers; it is not the entry point.

The current initialization order in `kernel_main` is:

1. Check support for the requested Limine base revision (6).
2. Enable SSE and initialize the Global Descriptor Table (GDT).
3. Initialize the framebuffer terminal using the first framebuffer and first
   module, assumed to be the PSF font.
4. Initialize the bitmap page frame allocator with `pfa_init()`.
5. Install the kernel's page tables with `paging_init()`.
6. Install the Interrupt Descriptor Table (IDT) and enable interrupts.
7. Initialize the local APIC and I/O APIC interrupt routing.
8. Print the greeting, initialize PS/2 devices, and enable keyboard scanning.
9. Enter `hcf()`, a loop executing `hlt` while interrupts remain enabled.

[requests.c](../kernel/arch/x86_64/requests.c) declares requests for the higher
half direct map, framebuffer, modules, memory map, executable addresses, and
ACPI root pointer. The request markers and revision check live in
`kernel_main`'s source; the linker keeps their sections in the ELF. Changes to
the bootloader, protocol header, requests, and linker layout need to remain
compatible with each other.

## Understand addresses before allocating memory

The kernel is linked at virtual address `0xffffffff80000000`. Limine supplies
its actual physical and virtual load bases. The **higher half direct map
(HHDM)** is a separate mapping where a physical address can be accessed at that
address plus Limine's HHDM offset.

[pfa.c](../kernel/arch/x86_64/pfa.c) tracks 4 KiB physical frames with a bitmap
derived from Limine's usable memory entries. `kalloc_frame()` and
`kalloc_frames()` return **physical addresses**, despite using the pointer type
`pageframe_t`. The free functions also expect physical addresses.

[pager.c](../kernel/arch/x86_64/pager.c) builds a new top-level page table,
maps the kernel with 4 KiB pages, builds a direct map with 2 MiB pages up to the
highest memory-map end address, and loads CR3. `map_page(paddr, vaddr, flags)`
adds a 4 KiB mapping; it always sets the present and writable bits.

After `paging_init()`, `phys_to_virt()` adds the HHDM offset and
`virt_to_phys()` subtracts it. These helpers apply to addresses in the direct
map; `virt_to_phys()` is not a general page-table lookup and must not be used
to translate arbitrary kernel virtual addresses. Early `pfa_init()` code uses
Limine's offset directly because the pager's global offset is not set yet.

## Interrupts, input, and output

[interrupts.S](../kernel/arch/x86_64/interrupts.S) provides the interrupt
stubs, and [idt.c](../kernel/arch/x86_64/idt.c) installs all 256 vectors.
[isr.c](../kernel/arch/x86_64/isr.c) dispatches hardware vectors `0x20`–`0x2f`,
acknowledges them through the local APIC, and prints register state for CPU
exceptions. Page faults also print CR2, the faulting virtual address.

[ioapic.c](../kernel/arch/x86_64/ioapic.c) reads ACPI tables to find the I/O
APIC and interrupt overrides. It routes keyboard IRQ 1 to vector `0x21`,
serial IRQ 3 to `0x22`, and serial IRQ 4 to `0x23`.

The output path is `printf` → `putchar` → `terminal_write` → framebuffer glyph
drawing in [tty.c](../kernel/arch/x86_64/tty.c). The terminal clears the screen
when it reaches the bottom. Serial routines exist but are not initialized by
`kernel_main`, and `printf` is not routed to them.

## Implementation boundaries

Treat these as constraints when choosing your first task:

| Area | Current boundary |
| --- | --- |
| Paging | `free_page()` is unfinished; there is no complete unmap API. `kalloc_pages()` returns `NULL`. |
| Heap | `libc/stdlib/heap.c` contains experimental `malloc`/`free`; `heap_init()` is not called during boot. |
| Slab allocation | `slab.c` is unfinished, `kfree()` is empty, and `slab.o` is absent from the kernel object list. |
| Keyboard | Raw scancode diagnostics are implemented; the command queue and character input are unfinished. |
| Serial | Basic register routines exist; interrupt handling is a stub and boot does not initialize a port. |
| C library | Only `libk.a` is built by default. This is a partial library, with incomplete routines and no hosted userspace runtime. |
| Concurrency | No scheduler, userspace, or multithreading implementation is present. |

A declaration in a header does not establish that an API is complete or usable
at boot. Check its implementation, build inclusion, and initialization before
using it.

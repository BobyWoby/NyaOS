# NyaOS :3

NyaOS is a work-in-progress x86-64 operating system kernel written in C and
assembly. It boots through Limine and includes a framebuffer text terminal,
physical memory allocation, basic paging, interrupt handling, and experimental
PS/2 input. There is no userspace or command shell yet.

## Start here

1. Follow [Getting started](docs/getting-started.md) to prepare your toolchain,
   build an ISO, and boot it in QEMU.
2. Read the [Architecture guide](docs/architecture.md) for the source map,
   boot sequence, and memory model.
3. Use the [Development guide](docs/development.md) for making changes,
   checking them, debugging, and troubleshooting.

With an `x86_64-elf` GCC/binutils toolchain, a native C compiler, GNU Make,
`xorriso`, and `qemu-system-x86_64` on your `PATH`, run these commands from the
repository root:

```sh
make -C limine-binary -B
./qemu.sh
```

The first command builds the native Limine installer. The second builds the
kernel and ISO before launching QEMU. The boot menu entry is currently named
`myOS`; after its five-second timeout, look for `Welcome to NyaOS` and PS/2
initialization messages in the graphical window. Keyboard handling prints raw
scancode diagnostics; it does not provide a text prompt.

## Current status and direction

The current boot path uses the GDT, bitmap page frame allocator, paging setup,
IDT, local APIC, I/O APIC, and PS/2 controller code. A small kernel C library
provides routines such as `printf` and string operations.

Immediate work remains on page unmapping, the slab allocator and `kmalloc`,
and allocator tests using a mock page frame allocator. The existing heap is
unfinished and is not initialized at boot. The slab source is not yet included
in the kernel build. See [implementation boundaries](docs/architecture.md#implementation-boundaries)
before depending on these APIs.

Longer-term goals include userspace, a round-robin scheduler, graphics beyond
the current text framebuffer, a filesystem (possibly ZFS), multithreading,
and paging that supports multithreading. These are future goals, not available
features.

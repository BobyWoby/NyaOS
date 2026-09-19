# Development workflow

[README](../README.md) · [Getting started](getting-started.md) · [Architecture](architecture.md)

## Make a first change

Start by building and booting the unmodified checkout to establish your local
baseline. A small first exercise is to change the greeting in
[kernel_main](../kernel/kernel/kernel.c), run `./qemu.sh`, and confirm that the
new text appears. This exercises compilation, installation, ISO packaging, and
boot without requiring a new subsystem.

For subsequent changes:

1. Read the subsystem's source, public header, and call sites. Check the
   [implementation boundaries](architecture.md#implementation-boundaries).
2. Match the surrounding C/assembly style. Kernel C is built as GNU C11 in a
   freestanding environment; use project headers and implemented library APIs.
3. Add new source files to the build's explicit object lists. Architecture
   objects go in `KERNEL_ARCH_OBJS` in
   [kernel/arch/x86_64/make.config](../kernel/arch/x86_64/make.config);
   generic kernel objects go in [kernel/Makefile](../kernel/Makefile).
   Library sources go in `FREEOBJS` in [libc/Makefile](../libc/Makefile).
4. Rebuild the ISO, boot it, and exercise the affected behavior.
5. Review `git diff --check` and `git status --short`. In your change description,
   record what you exercised, what appeared on screen, and any remaining limits.

The build generates `.d` header dependencies. Changing compiler flags does not
automatically invalidate existing objects, so use `./clean.sh` before rebuilding
after toolchain or flag changes. Also clean after library-only changes: the
kernel link target does not list `libk.a` as a dependency, so it may otherwise
retain the old library code.

## Validate changes

There is currently no automated test framework or `make test` target. The
baseline manual check is:

```sh
./clean.sh
./iso.sh
qemu-system-x86_64 -cdrom nyaos.iso
```

Confirm the greeting, PS/2 initialization, and keyboard scanning message. For
input changes, focus QEMU and press/release keys while watching scancode output.
For memory or interrupt changes, check for exception output, resets, or hangs
and exercise the specific code path you changed. A successful boot does not
test unused allocator code such as `slab.c`.

Existing compiler warnings include unfinished routines; compare warnings with
your baseline and investigate new ones. The planned allocator test framework
would use a mock page frame allocator, but it has not been implemented.

## Debug with GDB

Build a fresh ISO, then start QEMU paused with a debugger endpoint on localhost:

```sh
./iso.sh
qemu-system-x86_64 -cdrom nyaos.iso -S -gdb tcp:127.0.0.1:1234 -no-reboot -no-shutdown
```

In another terminal, from the repository root:

```sh
gdb kernel/nyaos.kernel
```

```gdb
target remote 127.0.0.1:1234
hbreak kernel_main
continue
```

The hardware breakpoint catches kernel entry after firmware and Limine execute;
the kernel's memory is not loaded when QEMU first pauses. Once there, use
`next`, `step`, `bt`, and `info registers` to inspect initialization. The default
`-O2 -g` keeps symbols but optimizes code, so some variables and source steps
may not be available as expected. For an optimization change, edit `config.sh`
and clean before rebuilding.

The QEMU `-S` option intentionally stops the CPU until the debugger continues.
Keep the ELF symbols and ISO from the same build.

For reset and exception investigation, collect QEMU diagnostics:

```sh
qemu-system-x86_64 -cdrom nyaos.iso -no-reboot -no-shutdown \
  -d int,cpu_reset -D /tmp/nyaos-qemu.log
```

This log records emulator diagnostics, not framebuffer `printf` output. Adding
`-serial stdio` does not expose the kernel terminal: serial output is not wired
into it. On a host without a graphical session, `-display none` can run a guest
for GDB inspection, but hides the framebuffer; a blank host terminal does not
establish whether the kernel booted.

## Troubleshooting

| Symptom | What to check |
| --- | --- |
| `x86_64-elf-gcc: not found` | Add the cross-toolchain's `bin` directory to `PATH`; verify `-dumpmachine` reports `x86_64-elf`. |
| Cannot find `libgcc` | Check `x86_64-elf-gcc -print-libgcc-file-name`; the cross-compiler needs its target runtime library. |
| Missing `config.sh`, `make.config`, or project directories | Run wrapper scripts from the repository root. Prefer them over invoking subdirectory Makefiles without the exported build configuration. |
| Limine installer cannot execute | Rebuild the native host tool with `make -C limine-binary -B` before running `./iso.sh`. |
| `xorriso: not found` | Install the host ISO creation tool; compiling the kernel alone does not create a bootable image. |
| QEMU cannot initialize its display | Run in a graphical session, or use `-display none` with GDB for debugging. |
| QEMU shows an old change | Recreate the ISO. For library or compiler-flag changes, run `./clean.sh` first. |
| QEMU stays paused before boot | If launched with `-S`, connect GDB and run `continue`. |
| Blank framebuffer or halt before greeting | Break at `kernel_main` and step through terminal, memory, and interrupt initialization. Check Limine responses and the packaged font module. |
| `*** EXCEPTION` appears | Record the vector, RIP, error code, and CR2 for page faults; inspect the matching ELF in GDB. |
| PS/2 initialization hangs | The driver polls without timeouts and assumes devices are present. Reproduce with the default QEMU device setup and inspect the polling loop in GDB. |

## Editor setup

The local `.clangd` configuration, if present, may contain absolute include
paths from another checkout. Configure clangd with your own `kernel/include`
and `libc/include` paths, GNU C11, `-ffreestanding`, and the relevant project
defines (`__is_kernel` for kernel code, `__is_libc` and `__is_libk` for the
kernel library). Editor diagnostics are useful, but the cross-compiler build
is the check for whether the project compiles.

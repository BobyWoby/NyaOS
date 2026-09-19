# Getting started

[README](../README.md) · [Architecture](architecture.md) · [Development](development.md)

## Prepare the host

Use a Linux environment with GNU command-line utilities. The scripts use
`/bin/sh`, GNU Make, and `cp --preserve=timestamps`. A graphical session is
needed for the default QEMU launcher. Other host environments may need changes
to the scripts and have not been verified here.

| Tool | Purpose |
| --- | --- |
| `x86_64-elf-gcc` | Compile and link the freestanding kernel and kernel library; must include target `libgcc` |
| `x86_64-elf-ar`, `x86_64-elf-as` | Target binutils used by the build/toolchain |
| `cc` | Native C compiler for the Limine installer |
| GNU `make` | Build the library, kernel, and installer |
| `xorriso` | Package the bootable ISO |
| `qemu-system-x86_64` | Run the kernel in an emulator |
| `gdb` with x86-64 support | Optional source-level debugging |

Install the host tools through your distribution's package manager. Obtain or
build GCC and binutils targeting **`x86_64-elf`** separately; this repository
does not bootstrap a cross-compiler. A normal Linux-targeting `gcc` is not the
configured kernel compiler. The cross-toolchain must support the flags in
[the architecture configuration](../kernel/arch/x86_64/make.config), including
`-fno-stack-clash-protection` and `-mcmodel=kernel`.

Put the cross-toolchain's `bin` directory on `PATH`. For example, if you
installed it under `$HOME/opt/cross`:

```sh
export PATH="$HOME/opt/cross/bin:$PATH"
command -v x86_64-elf-gcc x86_64-elf-ar x86_64-elf-as
command -v cc make xorriso qemu-system-x86_64
x86_64-elf-gcc -dumpmachine
x86_64-elf-gcc -print-libgcc-file-name
```

The target should be `x86_64-elf`; the last command should identify an existing
target `libgcc.a`. There are no pinned host-tool versions. The documentation
smoke check used GCC 16.1.0, QEMU 11.1.1, and xorriso 1.5.8; these are verified
versions, not declared minimums.

## Build and boot

Run all commands below from the repository root. The scripts resolve paths
relative to the current directory, so invoking them from elsewhere will fail.

```sh
make -C limine-binary -B
./qemu.sh
```

Limine's boot assets and installer source are included in `limine-binary/`.
The forced rebuild creates an installer executable for your host even when
the checked-in executable appears up to date. It can appear as a modified
tracked binary afterward; do not include that generated change in a source PR.

QEMU opens a graphical window. Select `myOS`, or wait for the five-second
timeout. The framebuffer should show:

```text
UwU Hallo :3Uwu
Welcome to NyaOS
```

PS/2 controller/device messages follow, including a message about enabling
keyboard scanning. Focus the window and press a key to exercise the experimental
scancode handler. There is no shell to enter commands into. After initialization,
the kernel waits in a `hlt` loop and handles interrupts. Close QEMU's window to
end the run.

Use the individual stages when you do not need to launch QEMU:

| Command | Result |
| --- | --- |
| `./headers.sh` | Copy library and kernel headers into `sysroot/usr/include/` |
| `./build.sh` | Install headers, build `libk.a`, then build and install the kernel |
| `./iso.sh` | Run the build and create `nyaos.iso` with Limine and the font module |
| `./qemu.sh` | Create the ISO and launch QEMU |
| `./clean.sh` | Remove library/kernel build outputs, `sysroot/`, `isodir/`, and `nyaos.iso` |

The scripts stop at the first failing command. Existing compiler warnings are
possible even when the build succeeds. Check the exit status and distinguish
warnings from compilation or linking errors.

## Build outputs and configuration

`libc/libk.a` is the kernel's C library. `kernel/nyaos.kernel` is the ELF kernel
with debug information. The build stages copies at `sysroot/usr/lib/libk.a`
and `sysroot/boot/nyaos.kernel`. The ISO additionally contains the Limine
configuration, BIOS/UEFI boot assets, and `sysroot/boot/minecraft.psf`.

Here, **sysroot** means the local staging directory that supplies the kernel's
headers and libraries to the cross-compiler. It is not the host's `/usr`, and
the build does not require `sudo`.

[config.sh](../config.sh) sets the default target from
[default-host.sh](../default-host.sh), builds compiler commands with the local
sysroot, and exports installation paths. It accepts `HOST` and `MAKE` from the
environment but assigns `CC`, `AR`, `AS`, `CFLAGS`, and `CPPFLAGS` itself.
Setting `CFLAGS` before running a wrapper script therefore does not override
its default `-O2 -g`; edit `config.sh` for a temporary flags experiment and
perform a clean rebuild. Only the x86-64 kernel is implemented, even though
some library files and helper scripts mention i386.

The ISO contains both BIOS and UEFI boot assets. The default QEMU command uses
its default BIOS firmware; including UEFI assets is not evidence that UEFI
boot has been tested.

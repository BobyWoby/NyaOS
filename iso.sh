#!/bin/sh
set -e
. ./build.sh

# Create a directory which will be our ISO root.
mkdir -p sysroot

# build.sh (make install) has already placed the kernel at
# sysroot/boot/nyaos.kernel; lay down the module and Limine boot files alongside it.
cp -v minecraft.bin sysroot/boot/minecraft.bin
mkdir -p sysroot/boot/limine
cp -v limine.conf limine-binary/limine-bios.sys limine-binary/limine-bios-cd.bin \
      limine-binary/limine-uefi-cd.bin sysroot/boot/limine/

# Create the EFI boot tree and copy Limine's EFI executables over.
mkdir -p sysroot/EFI/BOOT
cp -v limine-binary/BOOTX64.EFI sysroot/EFI/BOOT/
cp -v limine-binary/BOOTIA32.EFI sysroot/EFI/BOOT/

# Create the bootable ISO.
xorriso -as mkisofs -R -r -J -b boot/limine/limine-bios-cd.bin \
        -no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus \
        -apm-block-size 2048 --efi-boot boot/limine/limine-uefi-cd.bin \
        -efi-boot-part --efi-boot-image --protective-msdos-label \
        sysroot -o nyaos.iso

# Install Limine stage 1 and 2 for legacy BIOS boot.
./limine-binary/limine bios-install nyaos.iso


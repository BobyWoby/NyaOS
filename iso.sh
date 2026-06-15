#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/nyaos.kernel isodir/boot/nyaos.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "nyaos" {
	multiboot /boot/nyaos.kernel
}
EOF
grub-mkrescue -o nyaos.iso isodir

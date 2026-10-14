#!/bin/zsh
qemu-system-x86_64 -cdrom nyaos.iso -S -gdb tcp:127.0.0.1:1234 -no-reboot -no-shutdown

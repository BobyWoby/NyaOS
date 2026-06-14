
build:
	i686-elf-as boot.s -o boot.o
	i686-elf-g++ -c kernel.cpp -o kernel.o -ffreestanding -O2 -Wall -Wextra \
		-fno-exceptions -fno-rtti
	i686-elf-g++ -T linker.ld -o nyaos -ffreestanding -O2 -nostdlib boot.o \
		kernel.o -lgcc

	@if grub-file --is-x86-multiboot nyaos; then \
	  echo multiboot confirmed;\
	else\
	  echo the file is not multiboot;\
	fi

	cp nyaos isodir/boot/nyaos

	cp grub.cfg isodir/boot/grub/grub.cfg

	grub-mkrescue -o nyaos.iso isodir

run:
	qemu-system-i386 -cdrom nyaos.iso

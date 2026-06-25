/* The bootloader will look at this image and start execution at the symbol
   designated at the entry point. */
OUTPUT_FORMAT(elf64-x86-64)

ENTRY(kernel_main)
/* Define the program headers we want so the bootloader gives us the right */
/* MMU permissions; this also allows us to exert more control over the linking */
/* process. */
PHDRS
{
    limine_requests PT_LOAD;
    text PT_LOAD;
    rodata PT_LOAD;
    data PT_LOAD;
}

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	/* Begin putting sections at 1 MiB, a conventional place for kernels to be
	   loaded at by the bootloader. */
    kernel_start_virtual = .;
	. = 0xffffffff80000000;
    kernel_start_physical = .;

    .limine_requests : {
        KEEP(*(.limine_requests_start))
        KEEP(*(.limine_requests))
        KEEP(*(.limine_requests_end))
    } :limine_requests

    . = ALIGN(CONSTANT(MAXPAGESIZE));

	/* First put the multiboot header, as it is required to be put very early
	   early in the image or the bootloader won't recognize the file format.
	   Next we'll put the .text section. */
	.text :
	{
		*(.text .text.*)
	} :text

    . = ALIGN(CONSTANT(MAXPAGESIZE));
	/* Read-only data. */
	.rodata :
	{
		*(.rodata)
	}
    /* Add a .note.gnu.build-id output section in case a build ID flag is added to the */
    /* linker command. */
    .note.gnu.build-id : {
        *(.note.gnu.build-id)
    } :rodata

    . = ALIGN(CONSTANT(MAXPAGESIZE));

	/* Read-write data (initialized) */
	.data : 
	{
		*(.data)
	}

	/* Read-write data (uninitialized) and stack */
	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
        ebss = .;
        endkernel = .;
	}

    /* Discard .note.* and .eh_frame* since they may cause issues on some hosts. */
    /DISCARD/ : {
        *(.eh_frame*)
        *(.note .note.*)
    }
}

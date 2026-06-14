// Decalre constants for the multiboot header
.set ALIGN,     1<<0                #  align loaded modules on page boundaries
.set MEMINFO,   1<<1                #  provide memory map
.set FLAGS,     ALIGN | MEMINFO     #  This is the Multiboot 'flag' field
.set MAGIC,     0x1BADB002          #  'magic number' lets bootloader find the header
.set CHECKSUM,  -(MAGIC + FLAGS)    #  checksum of above, to prove we are multiboot



// Declare multiboot header that marks program as kernel
// These are magic values documented in multiboot standard.
// the bootloader will search for this signature in the first 8 KiB of the kernel file,
// aligned at a 32-bit boundary. The signature is in its own section so the header can
// be forced to be within the first 8 KiB of the kernel file.
.section    .multiboot
.align  4
.long   MAGIC
.long   FLAGS
.long   CHECKSUM


// multiboot standard !define stack pointer reg
// so we allocate room for a small stack, stack grows downward on x86.
// must be 16-byte aligned
.section    .bss
.align  16  
stack_bottom:
.skip   16384 # 16 KiB
stack_top:


// linker script specifies _start as entry point
// bootloader will jump to this position once kernel's loaded
.section    .text
.global     _start
.type   _start, @function
_start:
    //bootloader loads us into 32-protected mode on x86. Paging is disabled &
    // the state is as defined in multiboot standard, kernel has full control
    // of the cpu

    // setup the stack (set esp reg to point to top of the stack)
    mov $stack_top, %esp

    // initialize processor state here before high-level kernel is entered
    // processor !fully initialized yet, no FLOPS & instr set extensions
    // load GDT here & enable Paging here
    // C++ features like global constructors & exceptions need runtime support 
    // to work as well


    // Enter high level kernel. ABI requires stack to be 16-byte aligned at time
    // of call instr
    call kernel_main

    // if system has no more to do, spin forever

    cli     # clear interrupt enable in eflags
1:  hlt     # wait for next interrupt
    jmp 1b  # jump to hlt instruction

// set size of _start symbol to current location '.' minus the start
// useful for debugging / implementing call tracing
.size _start, . - _start


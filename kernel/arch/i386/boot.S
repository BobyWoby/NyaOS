# Declare constants for the multiboot header.
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

# Declare a header as in the Multiboot Standard.
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# Reserve a stack for the initial thread.
.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:


# The kernel entry point.
.section .text
.global _start
.type _start, @function
_start:
	movl $stack_top, %esp

    # setup GDT here

    # might be worth having kernel_main_early for setting up heap, log and other core features

	# Call the global constructors.
	call _init


	# Transfer control to the main kernel.
	call kernel_main

	# Hang if kernel_main unexpectedly returns.
	cli
1:	hlt
	jmp 1b
.size _start, . - _start


; // returns amount (in KB) of available RAM below 1MB (usually below 640KB)
; .global mem_lower
; mem_lower:
;     clc
;     int $0x12
;     jc .error
;     ret




# For the segment selector:
# bits 0-1: Requested Privelige Level
# bit 2: Table Index: 0 = GDT, 1 = LDT
# bits 3-15: Index of the GDT/LDT to use
.global flush_gdt
flush_gdt:
    movl    4(%esp), %eax
    lgdt    (%eax)
    ljmp     $0x08, $.reload_CS

.reload_CS:
    mov     $0x18, %ax
    mov     %ax, %ds
    mov     %ax, %es
    mov     %ax, %fs
    mov     %ax, %gs
    mov     %ax, %ss
    ret



.global flush_tss
flush_tss:
    mov    $((5 * 8) | 0), %ax
    ltr     %ax
    ret


.text
.globl loadPageDirectory
loadPageDirectory:
push %ebp
mov %esp, %ebp
mov 8(%esp), %eax
mov %eax, %cr3
mov %ebp, %esp
pop %ebp
ret

.text
.globl enablePaging
enablePaging:
push %ebp
mov %esp, %ebp
mov %cr0, %eax
or $0x80000000, %eax
mov %eax, %cr0
mov %ebp, %esp
pop %ebp
ret

# struct e820_entry{
#   uint64_t base; // physical base addr of the region
#   uint64_t length; // length in bytes
#   uint32_t type; // 1 = usable RAM, else => reserved
#   uint32_t acpi; // ACPI 3.x
# } __attribute__((packed)); nneds to be 24 bytes
# 

# uint16_t detect_e820(struct e820_entry *buffer, uint16_t max_entries);
#   buffer       : near pointer (offset within DS) to the entry array
#   max_entries  : capacity of that array (must be >= 1)
#   returns AX   : number of entries written (0 on failure)
.global detect_e820
detect_e820:
        push    %bp
        mov     %sp, %bp
        push    %di                     # callee-saved in cdecl
        push    %si
        push    %es

        mov     4(%bp), %di             # arg1: buffer offset -> DI
        mov     %ds, %ax
        mov     %ax, %es                # ES = DS, so ES:DI is the buffer
        xor     %ebx, %ebx              # continuation value, 0 to start
        xor     %si, %si                # SI = entry count (survives int 0x15)

        movl    $0x0534D4150, %edx      # "SMAP"
        movl    $0xe820, %eax
        movl    $1, %es:20(%di)         # pre-seed ACPI attr field
        movl    $24, %ecx
        int     $0x15
        jc      .Lfail                  # CF on first call => unsupported
        movl    $0x0534D4150, %edx
        cmpl    %edx, %eax              # eax must echo "SMAP"
        jne     .Lfail
        testl   %ebx, %ebx              # ebx==0 => useless 1-entry list
        je      .Lfail
        jmp     .Ljmpin
.Lloop:
        movl    $0xe820, %eax
        movl    $1, %es:20(%di)
        movl    $24, %ecx
        int     $0x15
        jc      .Ldone                  # CF now => end of list
        movl    $0x0534D4150, %edx
.Ljmpin:
        jcxz    .Lskip                  # 0-length BIOS reply
        cmpb    $20, %cl                # 24-byte ACPI 3.x entry?
        jbe     .Lnotext
        testb   $1, %es:20(%di)         # "ignore this data" bit clear?
        je      .Lskip
.Lnotext:
        movl    %es:8(%di), %ecx        # length low dword
        orl     %es:12(%di), %ecx       # | high dword
        jz      .Lskip                  # length 0 => skip
        inc     %si                     # ++count
        add     $24, %di                # next slot
        cmp     6(%bp), %si             # arg2: reached max_entries?
        jae     .Ldone                  # stop before overrunning buffer
.Lskip:
        testl   %ebx, %ebx              # ebx==0 => list complete
        jne     .Lloop
.Ldone:
        mov     %si, %ax                # return count
        jmp     .Lret
.Lfail:
        xor     %ax, %ax                # 0 on failure
.Lret:
        pop     %es                     # restore in reverse push order
        pop     %si
        pop     %di
        pop     %bp
        ret

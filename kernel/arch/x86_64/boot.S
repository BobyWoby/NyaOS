# Low-level assembly helpers used by the arch code (GDT/TSS/paging).
#
# Limine enters the kernel directly at kernel_main (see linker.ld) in 64-bit
# long mode with a stack and paging already set up, so there is no _start,
# stack reservation, or crt _init chain here anymore.
#
# WARNING: the routines below are still written for 32-bit (cdecl stack args,
# %esp, %cr0 paging-enable). They are not currently built into the kernel
# (gdt.c/mem.c are not in make.config yet) and MUST be ported to the x86_64
# SysV ABI (args in %rdi, 64-bit registers, etc.) before gdt_init() and
# paging_init() are re-enabled.

# For the segment selector:
# bits 0-1: Requested Privilege Level
# bit 2: Table Index: 0 = GDT, 1 = LDT
# bits 3-15: Index of the GDT/LDT to use
.text
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



.globl loadPageDirectory
loadPageDirectory:
    push %ebp
    mov %esp, %ebp
    mov 8(%esp), %eax
    mov %eax, %cr3
    mov %ebp, %esp
    pop %ebp
    ret



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

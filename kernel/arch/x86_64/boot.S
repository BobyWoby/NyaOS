.global flush_gdt
flush_gdt:
    lgdt    (%rdi)
    # Push code segment to stack
    push    $(2 * 8 | 0)

    leaq .reload_cs(%rip), %rax  # Load address of .reload_CS into RAX
    push %rax                  # Push this value to the stack
    retfq                     # Perform a far return, RETFQ or LRETQ depending on syntax

.reload_cs:
    mov     $(3 * 8 | 0), %ax
    mov     %ax, %ds
    mov     %ax, %es
    mov     %ax, %fs
    mov     %ax, %gs
    mov     %ax, %ss
    ret

.global flush_tss
flush_tss:
    mov     $(7 * 8 | 0), %ax
    ltr     %ax
    ret

.global enable_sse
enable_sse:
    # now enable SSE and the like
    mov %cr0, %rax
    and $0xFFFFFFFFFFFFFFFB, %rax   # clear coprocessor emulation CR0.EM (bit 2)
    or  $0x2, %rax                  # set coprocessor monitoring  CR0.MP (bit 1)
    mov %rax, %cr0
    mov %cr4, %rax
    or  $(3 << 9), %rax             # set CR4.OSFXSR and CR4.OSXMMEXCPT
    mov %rax, %cr4
    ret




.globl loadPageDirectory
loadPageDirectory:
    mov %rdi, %cr3
    ret


# .globl enablePaging
# enablePaging:
#     push %ebp
#     mov %esp, %ebp
#     mov %cr0, %eax
#     or $0x80000000, %eax
#     mov %eax, %cr0
#     mov %ebp, %esp
#     pop %ebp
#     ret

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

.extern exception_handler

.macro isr_err_stub arg1
isr_stub_\arg1:
    push $\arg1
    jmp isr_common
.endm

.macro isr_no_err_stub arg1
isr_stub_\arg1:
    push $0
    push $\arg1
    jmp isr_common
.endm

isr_common:
    push %rax
    push %rbx
    push %rcx
    push %rdx
    push %rsi
    push %rdi
    push %rbp
    push %r8
    push %r9
    push %r10
    push %r11
    push %r12
    push %r13
    push %r14
    push %r15

    mov %rsp, %rdi
    call exception_handler

    pop %r15
    pop %r14
    pop %r13
    pop %r12
    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rbp
    pop %rdi
    pop %rsi
    pop %rdx
    pop %rcx
    pop %rbx
    pop %rax
    add $16, %rsp
    iretq



isr_no_err_stub 0
isr_no_err_stub 1
isr_no_err_stub 2
isr_no_err_stub 3
isr_no_err_stub 4
isr_no_err_stub 5
isr_no_err_stub 6
isr_no_err_stub 7
isr_err_stub    8
isr_no_err_stub 9
isr_err_stub    10
isr_err_stub    11
isr_err_stub    12
isr_err_stub    13
isr_err_stub    14
isr_no_err_stub 15
isr_no_err_stub 16
isr_err_stub    17
isr_no_err_stub 18
isr_no_err_stub 19
isr_no_err_stub 20
isr_no_err_stub 21
isr_no_err_stub 22
isr_no_err_stub 23
isr_no_err_stub 24
isr_no_err_stub 25
isr_no_err_stub 26
isr_no_err_stub 27
isr_no_err_stub 28
isr_no_err_stub 29
isr_err_stub    30
isr_no_err_stub 31


.altmacro
.macro isr_entry num
    .quad isr_stub_\num     # use .quad if targeting 64-bit
.endm

.global isr_stub_table
isr_stub_table:
.set i, 0
.rept 32
    isr_entry %i
    .set i, i + 1
.endr


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

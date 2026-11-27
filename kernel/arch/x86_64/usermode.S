.global jump_usermode

# rdi = entry point, rsi = stack top
jump_usermode:
	mov $((3 * 8) | 3), %ax   # gdt ring 3 data selector, RPL 3
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs              # SS is set by iretq

	# iretq frame: SS, RSP, RFLAGS, CS, RIP
	pushq $((3 * 8) | 3)      # SS (user data)
	pushq %rsi                # RSP
	pushfq                    # RFLAGS
    popq %rax                  # RAX = current RFLAGS
    andq $~(3 << 12), %rax    # Clear IOPL bits 12 and 13
    pushq %rax                 # RFLAGS for iretq
	pushq $((4 * 8) | 3)      # CS (user code)
	pushq %rdi # RIP
	iretq

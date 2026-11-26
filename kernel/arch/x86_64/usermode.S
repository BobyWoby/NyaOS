.global jump_usermode
.extern test_user_function
jump_usermode:
	mov $((3 * 8) | 3), %ax   # gdt ring 3 data selector, RPL 3
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs              # SS is set by iretq

	# iretq frame: SS, RSP, RFLAGS, CS, RIP
	mov %rsp, %rax
	pushq $((3 * 8) | 3)      # SS (user data)
	pushq %rax                # RSP
	pushfq                    # RFLAGS
	pushq $((4 * 8) | 3)      # CS (user code)
	pushq $test_user_function # RIP
	iretq

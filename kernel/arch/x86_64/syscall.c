#include "kernel/syscall.h"

#include <stdio.h>
#include "kernel/syscall.h"


/*
 * rax = syscall
 * rdi = arg0
 * rsi = arg1
 * rdx = arg2
 * r10 = arg3
 * r8 = arg4
 * r9 = arg5
 * */
int syscall_handler(uint64_t syscall, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3,
                     uint64_t arg4, uint64_t arg5) {
    switch (syscall) {
        case 0:
            printf("syscall 0!\n");
            break;
        case 1:
            // write syscall
            break;
        default:
            printf("unknown syscall\n");
            return -1;
    }
    return 0;
}

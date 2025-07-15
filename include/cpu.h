#ifndef _CPU_H
#define _CPU_H

#include <stdint.h>

typedef struct {
    uint64_t int_no;
    uint64_t err_code;
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rdi, rsi, rbp, rbx, rdx, rcx, rax;

    uint64_t rip, cs, rflags, rsp, ss;
} cpu_registers_t;

#endif // _CPU_H
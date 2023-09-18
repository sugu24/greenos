#pragma once

#include <kernel/kernel.h>
#include <kernel/type.h>

// 割り込み・例外の原因を保存しているscauseレジスタのフィールド
#define SCAUSE_INSTRUCTION_ADDRESS_MISALIGNED 0
#define SCAUSE_INSTRUCTION_ACCESS_FAULT       1
#define SCAUSE_ILLEGAL_INSTRUCTION            2
#define SCAUSE_BREAKPOINT                     3
#define SCAUSE_LOAD_ACCESS_FAULT              5
#define SCAUSE_AMO_ADDRESS_MISALIGNED         6
#define SCAUSE_STORE_AMO_ACCESS_FAULT         7
#define SCAUSE_ENVIRONMENT_CALL               8
#define SCAUSE_INSTRUCTION_PAGE_FAULT         12
#define SCAUSE_LOAD_PAGE_FAULT                13
#define SCAUSE_STORE_AMO_PAGE_FAULT           15
#define SCAUSE_USER_SOFTWARE_INTERRUPT        ((1L << 31) | 0)
#define SCAUSE_SUPERVISOR_SOFTWARE_INTERRUPT  ((1L << 31) | 1)
#define SCAUSE_USER_TIMER_INTERRUPT           ((1L << 31) | 4)
#define SCAUSE_SUPERVISOR_TIMER_INTERRUPT     ((1L << 31) | 5)
#define SCAUSE_USER_EXTERNAL_INTERRUPT        ((1L << 31) | 8)
#define SCAUSE_SUPERVISOR_EXTERNAL_INTERRUPT  ((1L << 31) | 9)

static inline struct sbiret sbi_call(long arg0, long arg1, long arg2, long arg3, long arg4, long arg5, long fid,  long eid) {
    register long a0 __asm__("a0") = arg0;
    register long a1 __asm__("a1") = arg1;
    register long a2 __asm__("a2") = arg2;
    register long a3 __asm__("a3") = arg3;
    register long a4 __asm__("a4") = arg4;
    register long a5 __asm__("a5") = arg5;
    register long a6 __asm__("a6") = fid;
    register long a7 __asm__("a7") = eid;

    __asm__ __volatile__("ecall"
                        : "=r"(a0), "=r"(a1)
                        : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5),
                          "r"(a6), "r"(a7)
                        : "memory");

    return (struct sbiret) { .error = a0, .value = a1 };
}

static inline void write_stvec(uint32_t value) {
    __asm__ __volatile("csrw stvec, %0" ::"r"(value) );
}

static inline void asm_wfi(void) {
    __asm__ __volatile__("wfi");
}

#pragma once

#include <kernel/type.h>

// 割り込み・例外発生時の実行状態を保存する構造体
// riscv32における汎用レジスタとpcを保存する。
// riscv32_trap_handler関数で保存・復元される。
struct riscv32_trap_frame {
    uint32_t pc;       // 4 * 0(a0)
    uint32_t sstatus;  // 4 * 1(a0)
    uint32_t scause;   // 4 * 2(a0)
    uint32_t stval;    // 4 * 3(a0)
    uint32_t ra;       // 4 * 4(a0)
    uint32_t sp;       // 4 * 5(a0)
    uint32_t gp;       // 4 * 6(a0)
    uint32_t tp;       // 4 * 7(a0)
    uint32_t t0;       // 4 * 8(a0)
    uint32_t t1;       // 4 * 9(a0)
    uint32_t t2;       // 4 * 10(a0)
    uint32_t t3;       // 4 * 11(a0)
    uint32_t t4;       // 4 * 12(a0)
    uint32_t t5;       // 4 * 13(a0)
    uint32_t t6;       // 4 * 14(a0)
    uint32_t s0;       // 4 * 15(a0)
    uint32_t s1;       // 4 * 16(a0)
    uint32_t s2;       // 4 * 17(a0)
    uint32_t s3;       // 4 * 18(a0)
    uint32_t s4;       // 4 * 19(a0)
    uint32_t s5;       // 4 * 20(a0)
    uint32_t s6;       // 4 * 21(a0)
    uint32_t s7;       // 4 * 22(a0)
    uint32_t s8;       // 4 * 23(a0)
    uint32_t s9;       // 4 * 24(a0)
    uint32_t s10;      // 4 * 25(a0)
    uint32_t s11;      // 4 * 26(a0)
    uint32_t a0;       // 4 * 27(a0)
    uint32_t a1;       // 4 * 28(a0)
    uint32_t a2;       // 4 * 29(a0)
    uint32_t a3;       // 4 * 30(a0)
    uint32_t a4;       // 4 * 31(a0)
    uint32_t a5;       // 4 * 32(a0)
    uint32_t a6;       // 4 * 33(a0)
    uint32_t a7;       // 4 * 34(a0)
} __packed;


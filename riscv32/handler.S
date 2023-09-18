#include <riscv32/arch_cpuvar_offset.h>

// CPUのstvecに登録する割り込みハンドラ。
// 割り込みは自動的に無効化され、S-modeで実行される。

// まだsscratchやtpは未対応

.align 4 // パフォーマンスのため？
.global riscv32_trap_handler
riscv32_trap_handler:
    // S-modeで実行されているため、U-modeからこの関数に
    // ジャンプしてきた場合は、カーネルスタックで保存しなければいけない
    csrr a0, sstatus      // 割り込み発生時の動作モードがsstatusの一部にある
    andi a0, a0, (1 << 8) // SPPビット：1ならS|M-modeからきた (https://riscv.org/wp-content/uploads/2017/05/riscv-privileged-v1.10.pdf p.49)
    bnez a0, 1f

    // U-modeからきた場合のみココが実行される
    // ユーザからアクセスできないカーネルスタックをa0に設定する
    // lw a0, CPUVAR_SP_TOP(tp) // a0レジスタに実行中タスクのカーネルスタックを設定
    // j 2f
1:
    mv a0, sp
2:
    // 以降の割り込みハンドラで破壊される汎用レジスタをカーネルスタックに退避する。
    // 後で復元できるようにする。
    // 各レジスタの保存先はriscv32_trap_frame構造体に従う。

    addi a0, a0, -(33 * 4)
    // SCR(Control and Status Register)以外を保存する
    sw ra,  4 * 4(a0)
    sw sp,  4 * 5(a0)
    sw gp,  4 * 6(a0)
    sw tp,  4 * 7(a0)
    sw t0,  4 * 8(a0)
    sw t1,  4 * 9(a0)
    sw t2,  4 * 10(a0)
    sw t3,  4 * 11(a0)
    sw t4,  4 * 12(a0)
    sw t5,  4 * 13(a0)
    sw t6,  4 * 14(a0)
    sw s0,  4 * 15(a0)
    sw s1,  4 * 16(a0)
    sw s2,  4 * 17(a0)
    sw s3,  4 * 18(a0)
    sw s4,  4 * 19(a0)
    sw s5,  4 * 20(a0)
    sw s6,  4 * 21(a0)
    sw s7,  4 * 22(a0)
    sw s8,  4 * 23(a0)
    sw s9,  4 * 24(a0)
    sw s10, 4 * 25(a0)
    sw s11, 4 * 26(a0)
    sw a1,  4 * 28(a0)
    sw a2,  4 * 29(a0)
    sw a3,  4 * 30(a0)
    sw a4,  4 * 31(a0)
    sw a5,  4 * 32(a0)
    sw a6,  4 * 33(a0)
    sw a7,  4 * 34(a0)

    // spを退避させたのでspにスタックポインタをセットする
    mv sp, a0

    // 割り込み・例外発生時のscrを退避させる
    csrr a0, sepc
    sw a0, 4 * 0(sp)
    csrr a0, sstatus
    sw a0, 4 * 1(sp)
    csrr a0, scause
    sw a0, 4 * 2(sp)
    csrr a0, stval
    sw a0, 4 * 3(sp)

    // 割り込み・例外発生時の処理の続きをriscv32_handle_trapで行う
    // 引数はa0に渡す(riscvの関数の引数はa0~)。
    mv a0, sp
    call riscv32_handle_trap

    // 以下でriscv32_trap_frameから割り込み・例外処理前のCPUの状態に戻す

    // CSR(Control and State Register)を復元する
    lw a0, 4 * 0(sp)
    csrw sepc, a0
    lw a0, 4 * 1(sp)
    csrw sstatus, a0
    lw a0, 4 * 2(sp)
    csrw scause, a0    
    lw a0, 4 * 3(sp)
    csrw stval, a0

    // 汎用レジスタを復元する
    lw ra,  4 * 4(sp)
    lw sp,  4 * 5(sp)
    lw gp,  4 * 6(sp)
    lw tp,  4 * 7(sp)
    lw t0,  4 * 8(sp)
    lw t1,  4 * 9(sp)
    lw t2,  4 * 10(sp)
    lw t3,  4 * 11(sp)
    lw t4,  4 * 12(sp)
    lw t5,  4 * 13(sp)
    lw t6,  4 * 14(sp)
    lw s0,  4 * 15(sp)
    lw s1,  4 * 16(sp)
    lw s2,  4 * 17(sp)
    lw s3,  4 * 18(sp)
    lw s4,  4 * 19(sp)
    lw s5,  4 * 20(sp)
    lw s6,  4 * 21(sp)
    lw s7,  4 * 22(sp)
    lw s8,  4 * 23(sp)
    lw s9,  4 * 24(sp)
    lw s10, 4 * 25(sp)
    lw s11, 4 * 26(sp)
    lw a1,  4 * 28(sp)
    lw a2,  4 * 29(sp)
    lw a3,  4 * 30(sp)
    lw a4,  4 * 31(sp)
    lw a5,  4 * 32(sp)
    lw a6,  4 * 33(sp)
    lw a7,  4 * 34(sp)

    // The MRET, SRET, or URET instructions are used to return 
    // from traps in M-mode, S-mode, or U-mode respectively.
    // https://riscv.org/wp-content/uploads/2017/05/riscv-privileged-v1.10.pdf p20
    sret
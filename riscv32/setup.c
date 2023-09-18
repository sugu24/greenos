#include <riscv32/asm.h>
#include <riscv32/handler.h>

void riscv32_setup() {
    // 割り込み・例外処理ハンドラの設定
    write_stvec((uint32_t) riscv32_trap_handler);
}
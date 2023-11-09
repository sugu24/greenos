#include <riscv32/asm.h>
#include <riscv32/handler.h>

void riscv32_setup() {
    printf("riscv32_setup() start ...\n");
    // 割り込み・例外処理ハンドラの設定
    write_stvec((uint32_t) riscv32_trap_handler);

    // read_mstatus();

    printf("riscv32_setup() end ...\n\n");
}
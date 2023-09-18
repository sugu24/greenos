#include <riscv32/trap.h>
#include <riscv32/asm.h>
#include <kernel/kernel.h>

void riscv32_handle_trap(struct riscv32_trap_frame *frame) {
    // ---------- 未対応：スタックオーバーフローの確認 ---------- //

    //
    // 注意：カーネルロックの取得・解放
    // (mp_lock/mp_unlock 関数)を忘れないこと
    // 

    // 割り込み・例外の原因を取得
    uint32_t scause = frame->scause;
    switch (scause) {
        // 実行中タスクが原因の例外
        case SCAUSE_INSTRUCTION_ADDRESS_MISALIGNED:
            PANIC("SCAUSE_INSTRUCTION_ADDRESS_MISALIGNED");
        case SCAUSE_INSTRUCTION_ACCESS_FAULT:
            PANIC("SCAUSE_INSTRUCTION_ACCESS_FAULT");
        case SCAUSE_ILLEGAL_INSTRUCTION:
            PANIC("illegal instruction: scause=%x, stval=%x, sepc=%x\n", frame->scause, frame->stval, frame->pc);
        case SCAUSE_BREAKPOINT:
            PANIC("SCAUSE_BREAKPOINT");
        case SCAUSE_LOAD_ACCESS_FAULT:
            PANIC("SCAUSE_LOAD_ACCESS_FAULT");
        case SCAUSE_AMO_ADDRESS_MISALIGNED:
            PANIC("SCAUSE_AMO_ADDRESS_MISALIGNED");
        case SCAUSE_STORE_AMO_ACCESS_FAULT:
            PANIC("SCAUSE_STORE_AMO_ACCESS_FAULT");
        // システムコール
        case SCAUSE_ENVIRONMENT_CALL:
        // ページフォルト
        case SCAUSE_INSTRUCTION_PAGE_FAULT:
        case SCAUSE_LOAD_PAGE_FAULT:
        case SCAUSE_STORE_AMO_PAGE_FAULT:

        case SCAUSE_USER_SOFTWARE_INTERRUPT:
            PANIC("SCAUSE_USER_SOFTWARE_INTERRUPT");
        // ソフトウェア割り込み
        case SCAUSE_SUPERVISOR_SOFTWARE_INTERRUPT:
        case SCAUSE_USER_TIMER_INTERRUPT:
            PANIC("SCAUSE_USER_TIMER_INTERRUPT");
        case SCAUSE_SUPERVISOR_TIMER_INTERRUPT:
            PANIC("SCAUSE_SUPERVISOR_TIMER_INTERRUPT");
        case SCAUSE_USER_EXTERNAL_INTERRUPT:
            PANIC("SCAUSE_USER_EXTERNAL_INTERRUPT");
        // 外部割り込み
        case SCAUSE_SUPERVISOR_EXTERNAL_INTERRUPT:
        default:
            PANIC("unknown trap: scause=%p, stval=%p", frame->scause, frame->stval);
            break;
    }

    // ---------- 未対応：スタックオーバーフローの確認 ---------- //
}
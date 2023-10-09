#include <kernel/type.h>
#include <riscv32/asm.h>
#include <riscv32/lock.h>
#include <riscv32/switch.h>

// アイドルスレッドのメイン処理
// 割り込みが発生するまでCPUを休ませる
void arch_idle(void) {
    // 割り込みハンドラが自身でカーネルロックをとっているので、
    // ここではロックを持っていたら解除する
    bool locked = kernel_unlock();

    // 割り込みを有効かして待つ。割り込みが来るまでCPUを休ませる
    // S-modeでこのプログラムは動作するので、
    // sstatus.SIEに1を設定してS-modeでも割り込みを許可するようにする
    write_sstatus(read_sstatus() | SSTATUS_SIE);
    asm_wfi();

    // 割り込みハンドラが処理を終えてここに戻ってきた。
    // 割り込みを無効化してカーネルロックを元々取得していたら取得する
    write_sstatus(read_sstatus() | ~SSTATUS_SIE);
    if (locked) kernel_lock();
}

// スレッドをスイッチする準備を行う
void arch_thread_switch(struct thread *current, struct thread *next) {
    // ページテーブルを入れ替える
    // satpが示すページテーブルを変更したなら、
    // satpの書き換え前にsfence.vmaを実行する必要があるかもしれない。
    // 引用：https://riscv.org/wp-content/uploads/2017/05/riscv-privileged-v1.10.pdf p. 58
    asm_sfence_vma();
    // write_satp();

    // sfence.vmaより前の命令は、sfence.vma以降の
    // satpの参照前に順序付けされることが保証される。
    // なので、念のためsatpの書き換えの順序を入れ替えられないように
    // sfence.vmaを実行する。
    asm_sfence_vma();

    /* 
        // 割り込みハンドラで使うカーネルスタックを切り替える。
        // システムコールハンドラ内で入力待ち等でスリープ状態に
        // 入ることがあるため、スレッド毎にそのスレッドの
        // 実行コンテキストを保持する専用のカーネルスタックが必要。
        // CPUVAR->arch.sp_top = next->arch.sp_top;
        // ↓↓↓↓↓↓↓↓↓↓ とりあえず、カーネルスタックだけスレッドnextのものにする ↓↓↓↓↓↓↓↓↓↓
    */
    __asm__ __volatile__(
        "csrw sscratch, %[sscratch]\n"
        :: [sscratch] "r" ((uint32_t) &next->kernel_stack_info.sp)
    );

    // コンテキストスイッチを行う。
    // コンテキストスイッチを行ったスレッドは、
    // 再度実行される時はココから戻ってきて実行を続行する。
    riscv32_context_switch(&current->kernel_stack_info.sp, &next->kernel_stack_info.sp);
}
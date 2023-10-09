// コンテキストスイッチ
// void riscv32_context_switch(vaddr_t *current->arch.sp, vaddr_t *next->arc.sp)
//                                      ~~~~~~~~~~~~~~~~~         ~~~~~~~~~~~~~
//                                             a0                       a1 
.align 4
.global riscv32_context_switch
riscv32_context_switch:
    ///// ↓↓ ここからが実行していたスレッドのコンテキストの保存処理 ↓↓ /////
    add sp, sp, -13 * 4 // スタックの領域を確保

    // 現在実行しているスレッドのコンテキストを保存する
    sw  ra,  0 * 4(sp)
    sw s11,  1 * 4(sp)
    sw s10,  2 * 4(sp)
    sw  s9,  3 * 4(sp)
    sw  s8,  4 * 4(sp)
    sw  s7,  5 * 4(sp)
    sw  s6,  6 * 4(sp)
    sw  s5,  7 * 4(sp)
    sw  s4,  8 * 4(sp)
    sw  s3,  9 * 4(sp)
    sw  s2, 10 * 4(sp)
    sw  s1, 11 * 4(sp)
    sw  s0, 12 * 4(sp)

    sw sp, (a0) // spをcurrent->arc.spに保存する
    ///// ↑↑ ここまでが実行していたスレッドのコンテキストの保存処理 ↑↑ /////

    ///// ↓↓ ここからが実行するスレッドのコンテキストの復元処理 ↓↓ /////
    lw sp, (a1) // 実行するスレッドのコンテキストが保存されているスタックポインタをspに設定する

    lw  ra,  0 * 4(sp)
    lw s11,  1 * 4(sp)
    lw s10,  2 * 4(sp)
    lw  s9,  3 * 4(sp)
    lw  s8,  4 * 4(sp)
    lw  s7,  5 * 4(sp)
    lw  s6,  6 * 4(sp)
    lw  s5,  7 * 4(sp)
    lw  s4,  8 * 4(sp)
    lw  s3,  9 * 4(sp)
    lw  s2, 10 * 4(sp)
    lw  s1, 11 * 4(sp)
    lw  s0, 12 * 4(sp)

    add sp, sp, 13 * 4 // スタックの領域を解放

    ret // 復元したスレッドを再開する（arch_thread_switchに戻る）


// カーネルタスクのエントリポイント
// sp はカーネルスタックを指す
// この関数はカーネルが生成命令を出したときに実行される
// この関数を通ってカーネルが実行したいプログラムへと入っていく
.align 4
.global riscv32_kernel_entry_trampoline
riscv32_kernel_entry_trampoline:
    // スタックから引数を取り出す
    lw a0, 0 * 4(sp) // a0 <- arg
    lw a1, 1 * 4(sp) // a1 <- 実行するプログラムのエントリアドレス
    add sp, sp, 2 * 4 // 取り出した分スタックを解放する
    jr a1 // a1にジャンプする
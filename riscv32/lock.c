#include <kernel/type.h>
#include <riscv32/lock.h>

static uint32_t big_lock = BKL_UNLOCKED;

/*
    カーネルロックを取り扱うプログラム
    
    カーネルロック：
    カーネル内の共有リソースへのアクセス権のこと
*/

// カーネルロックを解除する
// ロックを取得していて、解除したら true を返す
// ロックを元々取得していなかったら、 false を返す
bool kernel_unlock(void) {
    // CPUのIDが一致していたらロックを解除できる
    // ↑↑↑↑↑ TO DO ↑↑↑↑↑

    // ココ以前にメモリ読み書きがロック解除前に行われるようにする
    // メモリバリア
    // これがないとCPUやコンパイラが実行順序を変えてしまう恐れがある
    full_memory_barrier();

    // ロック解除
    return compare_and_swap(&big_lock, BKL_LOCKED, BKL_UNLOCKED);
}

// カーネルロックを取得する
void kernel_lock(void) {
    // カーネルロックを取得するまでチャレンジする
    while (!compare_and_swap(&big_lock, BKL_UNLOCKED, BKL_LOCKED));

    // ココ以降のメモリ読み書きが上のロック取得前に行われないようにする
    // メモリバリア
    // これがないとCPUやコンパイラが実行順序を変えてしまう恐れがある
    full_memory_barrier();
}
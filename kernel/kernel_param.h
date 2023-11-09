#pragma once

// ページサイズ
#define PAGE_SIZE (1 << 12)

// プロセスの最大個数
#define MAX_NUM_PROCESS 32

// スレッドの最大個数
#define MAX_NUM_THREAD  8

// スレッドが持つカーネル空間のメモリ（スタック）のサイズ
#define THREAD_KERNEL_STACK_SIZE (4 * PAGE_SIZE)

// メモリサイズ
#define RAM_SIZE (128 * 1024 * 1024)
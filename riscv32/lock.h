#pragma once

#define BKL_LOCKED   0x12ab // カーネルロックをいずれかのCPUが使用中
#define BKL_UNLOCKED 0xc0be // カーネルロックを誰も使用していない
#define BKL_HALTED   0xdead // システムが停止した状態 (ロックを取らず停止する)

bool kernel_unlock(void);
void kernel_lock(void);
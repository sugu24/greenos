#pragma once

#include <kernel/type.h>
#include <kernel/memory.h>
#include <kernel/thread.h>
#include <kernel/kernel_param.h>
#include <kernel/common.h>

#include <libs/list.h>

struct kernel_object;

enum process_state {
    PROCESS_UNUSED,
    PROCESS_USED,
};

// プロセスの構造体
struct process {
    struct two_way_list two_way_list;
    paddr_t vm_root_table;  // 仮想アドレス変換ルートテーブルのベースアドレス
    struct page *pages;          // 確保しているページ
    enum process_state state; // プロセスの状態（使用・未使用）
    struct thread threads[MAX_NUM_THREAD]; // プロセスが持つスレッドの情報を持つ構造体
    struct kernel_object *kernel_objects;  // fdなどを表す構造体
    int ref_count; // 参照カウンタ
};

extern struct process processes[MAX_NUM_PROCESS];

process_t create_process(void *entry_addr, void *arg);
struct process *get_process_by_pid(process_t pid);
thread_t find_thread_id_by_pid(process_t pid);
error_t delete_process(process_t pid);

void init_process_manager(void);
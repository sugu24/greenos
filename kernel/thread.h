#pragma once

#include <kernel/type.h>
#include <libs/list.h>

// スレッドの状態を示す列挙型
enum thread_state {
    THREAD_UNUSED,
    THREAD_RUNNABLE,
    THREAD_WAITING,
};

// スレッドが管理するカーネルスタックの
// 情報を格納する構造体
struct kernel_stack_info {
    vaddr_t sp;       // カーネルスタックのsp(これを毎回spに設定する)
    vaddr_t sp_top;   // カーネルスタックのベースポインタ
    vaddr_t sp_bottom;// カーネルスタックが成長できる上限
};

// スレッドの構造体
struct thread {
    struct two_way_list two_way_list; // 双方向連結リストのための構造体
    process_t pid;     // 所属するプロセスのID
    enum thread_state state;  // スレッドの状態
    struct kernel_stack_info kernel_stack_info; // スレッド単位で管理するリソースを記憶する場所を示すスタックポインタ
    
};

/*
    カーネルスタックに記憶する順番
    スレッドの初期化はこのようにプッシュする
    スレッド実行途中のコンテキストスイッチでは

    low  sp_bottom -> |              |
     /\               |       :      |
    /||\              |   ra:entry   | : スレッドが実行される前に実行するプロローグ
     ||               |      s11     |
     ||               |      s10     |
     ||               |      s9      |
     ||               |      s8      |
     ||               |      s7      |
     ||               |      s6      |
    addr              |      s5      |
     ||               |      s4      |
     ||               |      s3      |
     ||               |      s2      |
     ||               |      s1      |
     ||               |      s0      |
    \||/              |      arg     | : スレッドのメインプログラムの引数(a0に設定)
     \/               |  entry_addr  | : スレッドのメインプログラムのエントリ
    high   sp_top  -> |--------------|
*/

void init_thread_manager(void);

thread_t create_thread(process_t pid, void *entry_addr, void *arg);
void free_threads(process_t pid);

void thread_runnable(process_t pid, thread_t tid);
void thread_switch(void);

void idle_thread_process(void);
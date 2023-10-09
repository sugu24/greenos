/*
 * 一旦ユーザプロセスは考えない 
 * ユーザスタックやそのspは考慮しないでコンテキストスイッチを行う
 * 
 * カーネルプロセスはカーネルスタックで動作させる
*/

#include <kernel/thread.h>
#include <kernel/proc.h>
#include <kernel/type.h>

#include <riscv32/thread.h>
#include <riscv32/switch.h>

#include <libs/list.h>

// 実行可能スレッドの連結リスト
// 先頭のスレッドを実行中とする
static struct thread *runnable_threads;

// アイドルスレッド
static struct thread IDLE_THREAD;

// 現在実行しているスレッド
static struct thread *CURRENT_THREAD;

// 今実行しているスレッドを返す
static struct thread *get_current_thread(void) {
    return CURRENT_THREAD;
}

static struct thread *get_thread_by_pid(process_t pid, thread_t tid) {
    struct process *proc = get_process_by_pid(pid);
    if (!proc)
        PANIC("error get_thread_by_pid(): get_process_by_pid() failure");

    return &proc->threads[tid - 1];
}

// 次に実行するスレッドを返す
static struct thread *scheduler(void) {
    // 次に実行するスレッドがなければアイドルタスクを返す
    if (!runnable_threads)
        return &IDLE_THREAD;

    // 現在実行中のスレッドの次に実行されるスレッドを取得
    // 実行中スレッドが返される場合もある
    return runnable_threads;
}

// スレッドを実行可能状態にする
// そして、実行可能スレッドの末尾に追加する
// 対象スレッドはprocesses[pid-1].threads[tid - 1]
void thread_runnable(process_t pid, thread_t tid) {
    struct thread *thread = get_thread_by_pid(pid, tid);
    if (thread->state == THREAD_UNUSED)
        PANIC("error thread_runnable(): thread->state == THREAD_UNUSED");
    
    thread->state = THREAD_RUNNABLE;

    // 実行可能スレッドの末尾に追加する
    two_way_list_push_back((struct two_way_list **)&runnable_threads, (struct two_way_list *)thread);
}

// スレッド構造体を初期化する
static void init_thread_struct(struct thread *thread, 
                               process_t pid, 
                               paddr_t sp_bottom,
                               void *entry_addr,
                               void *arg) {
    thread->pid = pid;
    thread->state = THREAD_WAITING;
    thread->two_way_list.next = NULL;
    thread->two_way_list.prev = NULL;

    /*
        カーネル空間のマッピングは物理アドレスと論理アドレスを一致させる
    */
    // スレッドのカーネルスタックのアドレスを計算する
    uint32_t sp_top = sp_bottom + THREAD_KERNEL_STACK_SIZE;
    uint32_t *sp    = (uint32_t *) sp_top;

    // スレッドのカーネルスタックにスレッドの実行に必要なものを記憶する
    // (riscv32_task_switch関数で復元される実行コンテキスト)
    // thread.hに示す通りに設定する
    void *prologue_entry = riscv32_kernel_entry_trampoline;
    *--sp = (uint32_t)entry_addr; 
    *--sp = (uint32_t)arg;
    *--sp = 0;         // s0
    *--sp = 0;         // s1
    *--sp = 0;         // s2
    *--sp = 0;         // s3
    *--sp = 0;         // s4
    *--sp = 0;         // s5
    *--sp = 0;         // s6
    *--sp = 0;         // s7
    *--sp = 0;         // s8
    *--sp = 0;         // s9
    *--sp = 0;         // s10
    *--sp = 0;         // s11
    *--sp = (uint32_t)prologue_entry; // ra

    // スレッドのカーネルスタックのアドレスを設定する
    thread->kernel_stack_info.sp_bottom = sp_bottom;
    thread->kernel_stack_info.sp_top = sp_bottom + THREAD_KERNEL_STACK_SIZE;
    thread->kernel_stack_info.sp = (uint32_t) sp;
}

// スレッド構造体を正しく生成する
// pid == 0 ならアイドルタスクの生成を行う
thread_t create_thread(process_t pid, void *entry_addr, void *arg) {
    thread_t tid;
    struct thread *thread;
    if (pid == 0) {
        // アイドルスレッドの生成
        thread = &IDLE_THREAD;
        tid = 0;
    } else {
        tid = find_thread_id_by_pid(pid);
        if ((int)tid == ERROR)
            PANIC("tid:%d is not used process id.", tid);

        // スレッド構造体を得る
        thread = get_thread_by_pid(pid, tid);
    }

    // カーネルスタックを確保する
    // このカーネルスタックはスレッド毎に
    // 管理するリソースを記憶するために用いる
    // sp_bottom: 確保したメモリの最小値でスタックが成長できる上限
    paddr_t sp_bottom = pm_alloc(THREAD_KERNEL_STACK_SIZE, 1);

    // スレッド構造体を初期化する
    init_thread_struct(thread, pid, sp_bottom, entry_addr, arg);

    // 初めに持つスレッドを RUNNABLE にする
    if (pid > 0 && pid <= MAX_NUM_PROCESS) 
        thread_runnable(pid, tid);

    return tid;
}

// スレッドをリセットする
static void free_thread(process_t pid, thread_t tid) {
    struct thread *thread = get_thread_by_pid(pid, tid);
    if (thread->state == THREAD_UNUSED)
        PANIC("error thread_runnable(): thread->state == THREAD_UNUSED");

    thread->two_way_list.prev = NULL;
    thread->two_way_list.next = NULL;
    thread->pid = 0;
    thread->state = THREAD_UNUSED;
    thread->kernel_stack_info.sp = 0;
    thread->kernel_stack_info.sp_top = 0;
    thread->kernel_stack_info.sp_bottom = 0;
} 

// num_thread 個のスレッドをリセットする
void free_threads(process_t pid) {
    for (thread_t tid = 0; tid <= MAX_NUM_THREAD; tid++) {
        free_thread(pid, tid);
    }
}

// スイッチするスレッドを選択する
void thread_switch(void) {
    // 現在実行しているのスレッドを得る
    struct thread *current = get_current_thread();

    // スイッチ後に実行するスレッドを取得
    struct thread *next = scheduler();
    
    // 次に実行するスレッドがない場合
    // すなわち実行中スレッドを継続して実行する
    if (next == current)
        return;
    
    // 実行可能スレッドの末尾にアイドルスレッド以外なら
    // 実行中スレッドを追加する
    if (current != &IDLE_THREAD && current->state == THREAD_RUNNABLE) 
        two_way_list_push_back((struct two_way_list **)&runnable_threads, (struct two_way_list *)current);

    // current を runnable_threads から取って
    two_way_list_left_shift((struct two_way_list **)&runnable_threads);
    
    // スレッドのスイッチの準備をする
    CURRENT_THREAD = next;
    arch_thread_switch(current, next);
}

// アイドルタスクの実態
void idle_thread_process(void) {
    // 終わりのない無限ループ
    for (;;) {
        // スイッチで切るスレッドがあればスイッチする
        thread_switch();

        // 割り込みを待つ
        arch_idle();
    }
}

// スレッド機構を初期化する
void init_thread_manager(void) {
    // 実行待ちリストを初期化する
    runnable_threads = NULL;

    // アイドルスレッドを生成する
    create_thread(0, idle_thread_process, NULL);
    CURRENT_THREAD = &IDLE_THREAD;
    printf("IDLE_THREAD:%x\n", CURRENT_THREAD);
}
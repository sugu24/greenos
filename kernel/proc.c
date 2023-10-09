#include <kernel/proc.h>
#include <kernel/thread.h>
#include <kernel/type.h>
#include <kernel/kernel_param.h>
#include <libs/list.h>

struct process processes[MAX_NUM_PROCESS];

// プロセスの構造体の初期化
static errno_t init_process_struct(struct process *process, 
                            paddr_t vm_root_table) {
    // ベースルートテーブルを表すページ構造体を取得する
    struct page *page = find_page_by_paddr(vm_root_table);
    
    process->vm_root_table = vm_root_table;
    process->pages = page;
    process->state = PROCESS_USED;
    // スレッドの初期化によって次にprocess->threads[tid]を初期化する
    process->kernel_objects = NULL;
    process->ref_count = 0;
    process->two_way_list.next = NULL;
    process->two_way_list.prev = NULL;

    // 仮想アドレス変換ルートテーブルをマップしたため、
    // ref_count を加算する
    if (page) page->ref_count++;

    return OK;
}

// 空いているプロセスを調べて返す
static paddr_t find_process_id(void) {
    for (int i = 0; i < MAX_NUM_PROCESS; i++) {
        if (processes[i].state == PROCESS_UNUSED)
            return i + 1;
    }
    return ERROR;
}

// 渡されたプロセスIDが示すプロセス構造体を返す
struct process *get_process_by_pid(process_t pid) {
    return &processes[pid - 1];
}

// 渡されたプロセスIDが示すプロセスで使用していないスレッドのIDを返す
thread_t find_thread_id_by_pid(process_t pid) {
    struct process *process = get_process_by_pid(pid);
    if (!process)
        PANIC("failure find_thread_id_by_pid()");

    for (int i = 0; i < MAX_NUM_THREAD; i++) {
        if (process->threads[i].state == THREAD_UNUSED)
            return i + 1;
    }
    return ERROR;
}

// プロセスを生成する
// 引数は、仮想アドレス変換ページのsaptに設定する
// ルートページとエントリアドレス
process_t create_process(void *entry_addr, void *arg) {
    // 空いているプロセスIDを得る
    process_t pid = find_process_id();
    if ((int)pid == ERROR)
        PANIC("find_process_id() failure");

    // プロセスの構造体を得る
    struct process *proc = get_process_by_pid(pid);
    if (!proc)
        PANIC("get_process_by_pid() failure");

    // ルートページテーブルトなるメモリを確保する
    paddr_t vm_root_table = pm_alloc(PAGE_SIZE, 1);
    if (!vm_root_table)
        PANIC("error create_process(): failure pm_alloc()");

    // プロセスの構造体を初期化する
    errno_t err = init_process_struct(proc, vm_root_table);
    if (err != OK)
        PANIC("init_process_struct() failure.");

    // スレッドを生成する
    thread_t tid = create_thread(pid, entry_addr, arg);

    return pid;
}

// プロセスを削除する
errno_t delete_process(process_t pid) {
    // 対称のプロセスを設定する
    struct process *proc = get_process_by_pid(pid);
    if (!proc)
        PANIC("error delete_process(): failure get_process_by_pid()");
    if (proc->state != PROCESS_USED)
        PANIC("pid:%d is not used process id.", pid);

    //　↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ 削除できない場合 ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
    // 他のプロセスから参照されている
    if (proc->ref_count > 0)
        PANIC("ref_count > 0 proc is tryed to deleted.");
    // CPUで実行中
    
    // ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ 削除する時の処理 ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
    // stete を UNUSED にする
    proc->state = PROCESS_UNUSED;
    // RUNNABLE なスレッドがあればスケジュールリストから削除
    // スレッドの解放
    // 暫定的
    free_threads(pid);
    // 保持するページの削除
    list_pm_free(proc->pages);
    // カーネルオブジェクトの解放


    return OK;
}

// プロセスを管理する機構の初期化
void init_process_manager() {
    memset(processes, 0, sizeof(struct process) * MAX_NUM_PROCESS);
    init_thread_manager();
}
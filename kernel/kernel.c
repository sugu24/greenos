#include "kernel.h"
#include "common.h"
#include <kernel/type.h>
#include <kernel/memory.h>
#include <kernel/proc.h>
#include <kernel/thread.h>
#include <riscv32/handler.h>
#include <riscv32/asm.h>

extern void riscv32_setup();

void putchar(char c) {
    // a0:c, a6:FID, a7:EID (sbi_console_putchar: a6:0, a7:1)
    sbi_call(c, 0, 0, 0, 0, 0, 0, 1);
}

void fb() {
    for (int i = 0; i < 10; i++) {
        printf("%d", i);
        thread_switch();
    }
}

void fa(char *c) {
    for (int i = 0; i < 10; i++) {
        printf("%s", c);
        thread_switch();
    }
}

void kernel_main(void) {
    /* CPUのセットアップ */
    riscv32_setup();

    /* .bss領域を0クリアしておく */
    memset(__bss, 0, (size_t) __bss_end - (size_t) __bss);

    // メモリ機構の初期化
    memory_init();

    // プロセス機構の初期化
    init_process_manager();

    printf("\n\nfrom sbi: %s %s %s\n", "Hello", "World", "!");
    printf("1 + 2 = %d, %x\n", 1 + 2, 0x1234abcd);

    char s1[5] = "abcd";
    char s2[5];
    char s3[5] = "efgh";
    int n1[5] = { 1, 2, 3, 4, 5 };
    int n2[5] = { 4, 4, 5, 5, 5 };
    memset(n2, 0, sizeof(n2));
    printf("%d %d %d %d %d\n", n2[0], n2[1], n2[2], n2[3], n2[4]);
    memcpy(n2, n1, sizeof(n1));
    printf("%d %d %d %d %d\n", n2[0], n2[1], n2[2], n2[3], n2[4]);
    strcpy_n(s2, s1, sizeof(s1));
    printf("%s\n", s2);
    printf("%d\n", strcmp(s2, s3));

    paddr_t paddr1 = pm_alloc(4 * 1024 + 1, 1);
    paddr_t paddr2 = pm_alloc(1000, 1);
    printf("paddr1 = %x\n", paddr1);
    printf("paddr2 = %x\n", paddr2);
    pm_free(paddr1);
    paddr1 = pm_alloc(8 * 1024 + 1, 1);
    printf("paddr1 = %x\n", paddr1);
    printf("paddr2 = %x\n", paddr2);
    struct page *page1 = find_page_by_paddr(paddr1);
    struct page *page2 = find_page_by_paddr(paddr2);
    printf("page1 = %x\n", page1);
    printf("page1->base = %x\n", page1->base);
    printf("page1->next = %x\n", page1->next);
    printf("page1->next->base = %x\n", page1->next->base);
    printf("page1->next->next = %x\n", page1->next->next);
    printf("page1->next->next->base = %x\n", page1->next->next->base);
    printf("page1->next->next->next = %x\n", page1->next->next->next);
    printf("page2 = %x\n", page2);
    printf("page2->base = %x\n", page2->base);
    printf("page2->next = %x\n", page2->next);

    // __asm__ __volatile__("unimp"); // illegal instruction
    process_t fa_pid = create_process(fa, "A");
    process_t fb_pid = create_process(fb, NULL);
    printf("fa_pid=%d, fb_pid=%d\n", fa_pid, fb_pid);
    create_thread(fa_pid, fa, "B");

    // アイドルスレッドの実行
    idle_thread_process();
}

/* .text.boot = 0x80200000 */
/* naked:スタック情報やレジスタ情報の記憶や呼び出し元に戻るための
スタック情報やレジスタ情報を戻すために生成される命令をなくす */
/* スタックポインタに__stack_topを設定する */
/* その後、kernel_mainにジャンプする */
__attribute__((section(".text.boot")))
__attribute__((naked))
void boot(void) {
    __asm__ __volatile__(
        "mv sp, %[stack_top]\n"
        "j kernel_main\n"
        :
        : [stack_top] "r" (__stack_top)
    );
}
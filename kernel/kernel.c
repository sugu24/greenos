#include "kernel.h"
#include "common.h"
#include "type.h"

/* リンカで__bss, __bss_end, __stack_topのアドレスが指定されている*/
extern char __bss[], __bss_end[], __stack_top[];

struct sbiret sbi_call(long arg0, long arg1, long arg2, long arg3, 
                       long arg4, long arg5, long fid,  long eid) {
    register long a0 __asm__("a0") = arg0;
    register long a1 __asm__("a1") = arg1;
    register long a2 __asm__("a2") = arg2;
    register long a3 __asm__("a3") = arg3;
    register long a4 __asm__("a4") = arg4;
    register long a5 __asm__("a5") = arg5;
    register long a6 __asm__("a6") = fid;
    register long a7 __asm__("a7") = eid;

    __asm__ __volatile__("ecall"
                         : "=r"(a0), "=r"(a1)
                         : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5),
                           "r"(a6), "r"(a7)
                         : "memory");
    
    return (struct sbiret) { .error = a0, .value = a1 };
}

void putchar(char c) {
    // a0:c, a6:FID, a7:EID (sbi_console_putchar: a6:0, a7:1)
    sbi_call(c, 0, 0, 0, 0, 0, 0, 1);
}

void kernel_main(void) {
    /* .bss領域を0クリアしておく */
    memset(__bss, 0, (size_t) __bss_end - (size_t) __bss);

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

    PANIC("booted!");

    printf("inter loop\n");
    for (;;) {
        __asm__ __volatile__("wfi");
    }
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
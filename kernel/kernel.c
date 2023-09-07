typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef uint32_t size_t;

/* リンカで__bss, __bss_end, __stack_topのアドレスが指定されている*/
extern char __bss[], __bss_end[], __stack_top[];

void *memset(void *buf, char c, size_t n) {
    uint8_t *p = (uint8_t *) buf;
    while (n--)
        *p++ = c;
    return buf;
}

/* .bss領域を0クリアしておく */
void kernel_main(void) {
    memset(__bss, 0, (size_t) __bss_end - (size_t) __bss);

    for (;;);
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
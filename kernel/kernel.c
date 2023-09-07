extern void KernelMain() {
    while(1) __asm__("ecall");
}
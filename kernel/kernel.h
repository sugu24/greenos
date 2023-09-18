#pragma once

#include <kernel/memory.h>

/* リンカでアドレスが指定されている*/
extern char __bss[];
extern char __bss_end[];
extern char __stack_top[];
extern char __free_ram[];
extern char __free_ram_end[];

#define PANIC(fmt, ...) \
    do { \
        printf("PANIC: %s:%d:" fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
        while (1) {} \
    } while (0)

struct sbiret {
    long error;
    long value;
};

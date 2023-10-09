#pragma once

#include <kernel/memory.h>

/* リンカでアドレスが指定されている*/
extern char __bss[];
extern char __bss_end[];
extern char __stack_top[];
extern char __free_ram[];
extern char __free_ram_end[];

struct sbiret {
    long error;
    long value;
};

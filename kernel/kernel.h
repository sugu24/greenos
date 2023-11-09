#pragma once

#include <kernel/memory.h>

/* リンカでアドレスが指定されている*/
extern char __text[];
extern char __text_end[];
extern char __data[];
extern char __data_end[];
extern char __bss[];
extern char __bss_end[];
extern char __stack_bottom[];
extern char __stack_top[];
extern char __free_ram[];
extern char __free_ram_end[];

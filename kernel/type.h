#pragma once

typedef int bool;
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef uint32_t size_t;
typedef unsigned long long uint64_t;
typedef uint32_t size_t;
typedef uint32_t errno_t;
typedef uint32_t paddr_t;
typedef uint32_t vaddr_t;
typedef uint32_t process_t;
typedef uint32_t thread_t;

#define true  1
#define false 0
#define NULL ((void *)0)

#define OK     1
#define ERROR  0

// alignment処理に関するdefine
// https://clang.llvm.org/docs/LanguageExtensions.html
#define ALIGN_UP(value, alignment) __builtin_align_up(value, alignment)
#define ALIGN_DOWN(value, alignment) __builtin_align_up(value, alignment)
#define IS_ALIGNED __builtin_is_aligned(value, alignment)

// これまでのスレッドの処理を同期させるメモリバリア
// https://www.ibm.com/docs/ja/xl-c-aix/13.1.0?topic=functions-sync-synchronize
#define full_memory_barrier __sync_synchronize

// ptrのアドレスが持つ値とcompValの値が同じならtrueを返して
// *ptr <- exchVal とする
// https://www.ibm.com/docs/ja/xl-c-aix/13.1.0?topic=functions-sync-bool-compare-swap
#define compare_and_swap(ptr, compVal, exchVal)                                        \
    __sync_bool_compare_and_swap(ptr, compVal, exchVal)
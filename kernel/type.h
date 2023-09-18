#pragma once

typedef int bool;
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef uint32_t size_t;
typedef unsigned long long uint64_t;
typedef uint32_t size_t;
typedef uint32_t paddr_t;
typedef uint32_t vaddr_t;

#define true  1
#define false 0
#define NULL ((void *)0)

// alignment処理に関するdefine
// https://clang.llvm.org/docs/LanguageExtensions.html
#define ALIGN_UP(value, alignment) __builtin_align_up(value, alignment)
#define ALIGN_DOWN(value, alignment) __builtin_align_up(value, alignment)
#define IS_ALIGNED __builtin_is_aligned(value, alignment)

// // アサーション。exprが偽ならエラーメッセージを出力してプログラムを終了する。
// #define ASSERT(expr)                                                           \
//     do {                                                                       \
//         if (!(expr)) {                                                         \
//             printf("[%s] %s:%d: ASSERTION FAILURE: %s\n",     \
//                    __program_name(), __FILE__, __LINE__, #expr);               \                      \
//         }                                                                      \
//     } while (0)
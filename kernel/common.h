#pragma once

#include <kernel/type.h>

#define align_up(value, align)   __builtin_align_up(value, align)
#define is_aligned(value, align) __builtin_is_aligned(value, align)
#define offsetof(type, member)   __builtin_offsetof(type, member)

#define va_list  __builtin_va_list
#define va_start __builtin_va_start
#define va_end   __builtin_va_end
#define va_arg   __builtin_va_arg

#define PANIC(fmt, ...) \
    do { \
        printf("PANIC: %s:%d:" fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
        while (1) {} \
    } while (0)

void *memset(void *buf, char c, size_t n);
void *memcpy(void *dst, const void *src, size_t n);
char *strcpy(char *dst, const char *src);
char *strcpy_n(char *dst, const char *src, size_t n);
int   strcmp(const char *s1, const char *s2);
void  printf(const char *fmt, ...);

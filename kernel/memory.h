#pragma once

#include <kernel/type.h>
#include <kernel/proc.h>

enum page_type {
    FREE_PAGE,
};

// pageの情報を保持する構造体
// paddingはいらない（全てのメンバが4Byteであるため）
struct page {
    struct page *next;  // ownerあり：owner->pageのリストを成す。ownerなし：次の空きページを指す
    enum page_type type;// ページの属性
    struct process *owner; // pageの保有者
    int ref_count;      // 参照カウンタ: 0:未使用, 1:確保されている, 2:マップされている, 3:複数で参照されている
    paddr_t base;       // 管理するpageの物理アドレス
};

void memory_init();
paddr_t pm_alloc(size_t size, uint8_t flag);
void pm_free(paddr_t paddr);
void list_pm_free(struct page *pages);
struct page *find_page_by_paddr(paddr_t paddr);
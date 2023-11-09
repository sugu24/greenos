#include <kernel/kernel.h>
#include <kernel/memory.h>
#include <kernel/type.h>
#include <kernel/common.h>

// __free_ramの頭をstruct pageの格納場所とする
// つまり、それより下はページの本体
struct page *pages = (struct page *)__free_ram;
int whole_num_page;
paddr_t start_page_addr;

// paddr_tを指すstruct pageを返す
struct page *find_page_by_paddr(paddr_t paddr) {
    // paddr を指す struct page のインデックスを計算
    int index = (int)(paddr - start_page_addr) / PAGE_SIZE;

    if (pages[index].base != paddr)
        PANIC("pages[index].base != paddr");
    return &pages[index];
}

// pages から size Byte のページの先頭アドレスを返す
// flag != 0 なら 0 で埋めてから物理アドレスを返す
paddr_t pm_alloc(size_t size, uint8_t flag) {
    // 割り当てるページ数を計算
    size_t aligned_size = ALIGN_UP(size, PAGE_SIZE);
    size_t num_page = aligned_size / PAGE_SIZE;

    // num_page分のフリーのページがあればPANICを避ける
    struct page *head_free_page = NULL;
    struct page *free_page;
    for (int i = 0; i < whole_num_page && num_page > 0; i++) {
        // 使用中のページを対象にcontinueする
        if (pages[i].ref_count > 0) continue;
        
        num_page--;

        // 割り当てる先頭のページを管理する構造体pageを保存する
        if (!head_free_page) 
            head_free_page = &pages[i];
        // 次のページをnextに保存する
        else
            free_page->next = &pages[i];

        free_page = &pages[i];

        // 確保済みであることをref_countに反映する
        pages[i].ref_count = 1;

        // flagがセットされていれば0クリアする
        if (flag)
            memset((void *) pages[i].base, 0, PAGE_SIZE);
    }
    if (num_page > 0) {
        PANIC("There are no pages of the size allocated.");
    }

    return head_free_page->base;
}

// paddrから始まる物理アドレスを解放する
// paddrを指すstruct pageのref_countを0にすることと
// nextに NULL を設定する
void pm_free(paddr_t paddr) {
    // paddrを指すstruct pageを得る
    struct page *page = find_page_by_paddr(paddr);

    // 各ページを解放する
    struct page *temp_page;
    for (; page; page = temp_page) {
        page->ref_count = 0;
        temp_page = page->next;
        page->next = NULL;
    }
}

// 連結リストで表されたページを一括で解放する
void list_pm_free(struct page *pages) {
    struct page *page = pages;
    struct page *buff;

    while (page) {
        buff = page->next;
        pm_free(page->base);
        page = buff;
    }
}

// __free_ram ~ __free_ram_end までの動的に扱えるメモリを
// ページング方式：Sv32 で扱えるように抽象化することを行う
// メモリ管理システムの初期化を行う
// __free_ram ~ __free_ram_end を ページとページを管理する構造体を記憶する
// ための領域として初期化する
void memory_init() {
    printf("memory_init() start ...\n");
    // 自由に使えるram領域のサイズを計算
    size_t free_ram_size = (size_t)(__free_ram_end - __free_ram);

    // 実際に扱えるページ数を計算
    whole_num_page = free_ram_size / (PAGE_SIZE + sizeof(struct page));

    // 実際のpageの始まるアドレスを計算
    start_page_addr = ALIGN_UP((paddr_t)(__free_ram + sizeof(struct page) * whole_num_page), PAGE_SIZE);
    printf("whole_num_page = %x %d\n", whole_num_page, whole_num_page);
    printf("start_page_addr = %x\n", start_page_addr);
    printf("ram_end = %x\n", __free_ram_end);
    printf("ram_size = %x\n", __free_ram_end - start_page_addr);
    
    // page[0~num_page]を初期化する
    for (int i = 0; i < whole_num_page; i++) {
        pages[i].type = FREE_PAGE;
        pages[i].owner = NULL;
        pages[i].next = NULL;
        pages[i].ref_count = 0;
        pages[i].base = start_page_addr + PAGE_SIZE * i;
    }

    printf("memory_init() end ...\n\n");
}
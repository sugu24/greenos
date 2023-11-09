/*
    仮想メモリを扱うプログラム群
    (Virtual Memory: VM)
*/

#include <kernel/type.h>
#include <kernel/kernel.h>
#include <kernel/thread.h>

#include <riscv32/asm.h>
#include <riscv32/vm.h>

// カーネルプログラムが動作するメモリ領域がマップされる
// ページテーブルの先頭を指す変数
static paddr_t kernel_vm_table;

// ページテーブルの1段目を割り当てて、
// ページテーブルの初期化を行う
error_t arch_vm_init(paddr_t *vm_table) {
    // ページテーブルの1段目を割り当てる
    *vm_table = pm_alloc(PAGE_SIZE, 1);
    if (!*vm_table)
        return ERR_NO_MEMORY;

    // カーネル領域のページをマッピング
    // printf("vm_table:%x, kernel_vm_table:%x\n", *vm_table, kernel_vm_table);
    memcpy((void *)*vm_table, (void *)kernel_vm_table, PAGE_SIZE);
    // printf("----------------------------------------\n");
    // for (int i = 0; i < PAGE_SIZE; i += 4)
    //     printf("<<<<< %x(%x) %x(%x)\n", *(pte_t *)(*vm_table + i), (*vm_table + i), *(pte_t *)(kernel_vm_table + i), (kernel_vm_table + i));
    return OK;
}

// 仮想アドレスvaddrからレベルlevelに応じて
// ページテーブルからのオフセットを返す 
static inline int page_table_offset(int level, vaddr_t vaddr) {
    // vaddrからオフセットを取り除いて下層ページ番号を取得
    switch (level) {
        case 0:
            return (vaddr >> VPN0_LEVEL) & VPN_MASK;
        case 1:
            return (vaddr >> VPN1_LEVEL) & VPN_MASK;
        default:
            PANIC("error page_table_offset");
    }
}

// PTEから次のページテーブルのアドレスを取得する
static inline paddr_t pte_to_paddr(pte_t pte) {
    return (paddr_t)((pte >> 10) << 12);
}

// ページテーブルエントリの値を構築する
static pte_t construct_pte(paddr_t paddr, uint32_t flags) {
    // vaddr & 0x3ff の12bitがoffsetなので
    // paddr & ~0x3ff | vaddr & 0x3ff で物理アドレスに変換される
    return ((paddr >> 12) << 10) | flags;
}

// ルートページテーブルvm_tableから探索して2段目のPTEをpteにセットする
// ついでに alloc が true なら PTE が有効でない場合は設定する
// vaddrは仮想アドレス
static error_t walk(paddr_t vm_table, vaddr_t vaddr, bool alloc, pte_t **pte) {
    // pteは4バイトのため、ページテーブルの型を変更する
    pte_t *first_vm_table = (pte_t *)vm_table;

    // 1段目のPTEを取得する
    int offset_pte1 = page_table_offset(1, vaddr);
    pte_t *pte1 = &first_vm_table[offset_pte1];
    
    // 1段目のPTEが設定されていなければ2段目のページテーブルを設定する
    if (!(*pte1 & PTE_V)) {
        // 2段目のページテーブルを割り当てずにエラーを返す場合
        if (!alloc)
            return ERR_NOT_FOUND;
        
        // 2段目のページテーブルを設定する
        paddr_t paddr = pm_alloc(PAGE_SIZE, 1);
        if (!paddr)
            return ERR_NO_MEMORY;
        
        // pte.R, pte.W, pte.X が0なら次のレベルの
        // ページテーブルへのポインタになるため、PTE_VのみでOK
        *pte1 = construct_pte(paddr, PTE_V);
    }
    // 2段目のページテーブルを取得する
    pte_t *second_vm_table = (pte_t *)pte_to_paddr(*pte1);
    
    // 2段目のPTEをpteにセットする
    int offset_pte2 = page_table_offset(0, vaddr);
    *pte = &second_vm_table[offset_pte2];

    return OK;
}

// ページングをマップする
error_t arch_vm_map(paddr_t vm_table, paddr_t paddr, vaddr_t vaddr, uint32_t attr) {
    // leaf-PTEを得る
    pte_t *pte;
    error_t err = walk(vm_table, vaddr, true, &pte);
    if (err != OK)
        return err;

    // leaf-PTEが既に使われているなら中断する
    if (*pte & PTE_V)
        return ERR_ALREADY_EXISTS;

    // pteにページテーブルエントリを設定する
    *pte = construct_pte(paddr, attr | PTE_V);

    // sfence.vmaより前の命令は、sfence.vma以降の
    // satpの参照前に順序付けされることが保証される。
    asm_sfence_vma();

    // 他のCPUにTLBをクリアするように通知する
    // arch_send_ipi(IPI_TLB_FLUSH);
    return OK;
}

// ページテーブルの解放
error_t arch_vm_free(paddr_t vm_table) {
    // 1段目のページテーブルを取得する
    pte_t *first_vm_table = (pte_t *)vm_table;

    // 1段目のページテーブルを走査して、使用中PTEを無効化する
    for (int i = 0; i < PAGE_SIZE / 32; i++) {
        pte_t pte1 = first_vm_table[i];

        // 無効化PTEなら無視
        // または、pte1が2段目のページテーブルへのポインタではなかった場合無視
        if (!(pte1 & PTE_V) || 
            !(pte1 & PTE_R || pte1 & PTE_W || pte1 & PTE_X))
            continue;
        
        // 2段目のページテーブルを走査して、使用中PTEを無効化する
        pte_t *second_vm_table = (pte_t *)pte_to_paddr(pte1);
        for (int j = 0; j < PAGE_SIZE / 32; j++) {
            pte_t pte2 = second_vm_table[j];

            // ユーザ空間のページでなければ解放しない
            if (pte2 & PTE_V && pte2 & PTE_U)
                continue;

            paddr_t paddr = pte_to_paddr(pte2);
            pm_free(paddr);
        }
    }

    // 1段目のページテーブルを格納している物理ページを解放する
    pm_free(vm_table);

    return OK;
}

// 連続領域のマッピング
// vm_table(物理アドレス)を先頭アドレスとして、
// vaddrからsize分の領域にpaddrをマッピングする
// attrはPTEの設定に使用する
error_t map_pages(paddr_t vm_table, paddr_t paddr, vaddr_t vaddr, size_t size, uint32_t attr) {
    // 各ページを1つづつマッピングする
    for (size_t offset = 0; offset < size; offset += PAGE_SIZE) {
        error_t err = arch_vm_map(vm_table, paddr + offset, vaddr + offset, attr);
        if (err != OK) 
            PANIC("error %d map_pages(): arch_vm_map() failure", err);
    }

    return OK;
}

// ページング管理機構を初期化する
void riscv32_vm_init(void) {
    printf("riscv32_vm_init() start ...\n");
    // カーネル領域のマッピングをkernel_vm_tableに行う
    kernel_vm_table = pm_alloc(PAGE_SIZE, 1);
    
    // リンカスクリプトで定義されているカーネル領域のアドレスを得る
    vaddr_t kernel_text = (vaddr_t)__text;
    vaddr_t kernel_text_end = (vaddr_t)__text_end;
    vaddr_t kernel_data = (vaddr_t)__data;
    vaddr_t kernel_data_end = (vaddr_t)__data_end;
    vaddr_t kernel_stack_bottom = (vaddr_t)__stack_bottom;
    vaddr_t kernel_stack_top = (vaddr_t)__stack_top;
    vaddr_t kernel_free_ram = (vaddr_t)__free_ram;
    vaddr_t kernel_free_ram_end = (vaddr_t)__free_ram_end;

    printf("kernel_text         %x\n", kernel_text);
    printf("kernel_text_end     %x\n", kernel_text_end);
    printf("kernel_data         %x\n", kernel_data);
    printf("kernel_data_end     %x\n", kernel_data_end);
    printf("kernel_stack_bottom %x\n", kernel_stack_bottom);
    printf("kernel_stack_top    %x\n", kernel_stack_top);
    printf("kernel_free_ram     %x\n", kernel_free_ram);
    printf("kernel_free_ram_end %x\n", kernel_free_ram_end);

    // 各カーネル領域のサイズを得る
    size_t kernel_text_size = kernel_text_end - kernel_text;
    size_t kernel_data_size = kernel_data_end - kernel_data;
    size_t kernel_stack_size = kernel_stack_top - kernel_stack_bottom;
    size_t kernel_free_ram_size = kernel_free_ram_end - kernel_free_ram;

    // 各カーネル領域によって適切に権限を変えながらマッピングする
    // (paddr_t vm_table, paddr_t paddr, vaddr_t vaddr, size_t size, uint32_t attr)
    // カーネルのコード領域
    printf("kernel_text mapping (%x ~ %x)...\n", kernel_text, kernel_text_end);
    map_pages(kernel_vm_table, 
              kernel_text, kernel_text, kernel_text_size, 
              PTE_R | PTE_W | PTE_X);
    // カーネルのデータ領域
    printf("kernel_data mapping ...\n");
    map_pages(kernel_vm_table, 
              kernel_data, kernel_data, kernel_data_size, 
              PTE_R | PTE_W);
    // カーネルのデータ領域
    printf("kernel_stack_bottom mapping ...\n");
    map_pages(kernel_vm_table, 
              kernel_stack_bottom, kernel_stack_bottom, kernel_stack_size, 
              PTE_R | PTE_W);
    // カーネルのデータ領域
    printf("kernel_free_ram mapping ...\n");
    map_pages(kernel_vm_table, 
              kernel_free_ram, kernel_free_ram, kernel_free_ram_size, 
              PTE_R | PTE_W);

    printf("riscv32_vm_init() end ...\n\n");
}
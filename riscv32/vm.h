/*
    仮想メモリを扱うプログラム群
    (Virtual Memory: VM)
*/
#pragma once

#include <kernel/type.h>

#define VPN_MASK 0x3ff // VPNは10bitのため0x3ffだと適切にVPNをマスクできる
#define SATP_PPN_SHIFT 12 // satpにセットする物理アドレスはページ単位でかくためこれでシフトする
#define VPN0_LEVEL 12  // vaddrをこれでシフトしてVPN0を取り出しやすくする
#define VPN1_LEVEL 22  // vaddrをこれでシフトしてVPN1を取り出しやすくする

/*
    ・When all three(R,W,X) are zero, the PTE is a pointer to the next level of the page table.
    ・Writable pages must also be marked readable.
    ・If the SUM bit in the ssatus register is set, supervisor mode software may also access pages with U=1.
    ・For non-leaf PTEs, the D, A, and U bits are reserved for future use and must be cleared by software for forward compatibility.
    引用：https://riscv.org/wp-content/uploads/2017/05/riscv-privileged-v1.10.pdf pp.60.61
*/
#define SATP_MODE_SV32 (1u << 31) // ページングタイプ：Sv32
#define PTE_V          (1 << 0)   // ページテーブルが有効であるか
#define PTE_R          (1 << 1)   // 読み込み可能物理アドレス
#define PTE_W          (1 << 2)   // 書き込み可能物理アドレス
#define PTE_X          (1 << 3)   // 実行可能物理アドレス
#define PTE_U          (1 << 4)   // U-modeでアクセス可能な物理アドレス
#define PTE_G          (1 << 5)   // グローバルマッピング（全てのアドレス空間に存在するマッピングのこと）
#define PTE_A          (1 << 6)
#define PTE_D          (1 << 7)  

typedef uint32_t pte_t;

error_t arch_vm_init(paddr_t *vm_table);
void riscv32_vm_init();
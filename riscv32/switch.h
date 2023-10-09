#pragma once

void riscv32_context_switch(vaddr_t *current_sp, vaddr_t *next_sp);
void riscv32_kernel_entry_trampoline(void);
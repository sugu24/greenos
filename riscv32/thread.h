#pragma once

#include <kernel/type.h>
#include <kernel/thread.h>

void arch_idle(void);
void arch_thread_switch(struct thread *current, struct thread *next);
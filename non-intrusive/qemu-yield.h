
#ifndef __QEMU_YIELD_h__
#define __QEMU_YIELD_h__

#include "qemu-common.h"
#include "qom/cpu.h"
#include "qemu/coroutine.h"
#include "non-intrusive/loader.h"

void cpu_yield(CPUState *env);

void qemu_init_yield_pool(void);
void qemu_destroy_yield_pool(void);

typedef struct CPUYield CPUYield;

struct CPUYield
{
    Coroutine* coroutine;
    uint64_t origin_pc, magic_pc;
    int* ignore_next;
    QTAILQ_ENTRY(CPUYield) next;
};

CPUYield* allocate_yield(void);
void release_yield(CPUYield*);

#endif

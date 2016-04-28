
#include "qemu/osdep.h"
#include "qemu-yield.h"

#include <string.h>
#include <stdio.h>

typedef struct CPUYieldPool CPUYieldPool;

struct CPUYieldPool
{
    QTAILQ_HEAD(free_head, CPUYield) free;
    QTAILQ_HEAD(used_head, CPUYield) used;
};

static CPUYieldPool yield_pool;

void qemu_init_yield_pool(void)
{
    QTAILQ_INIT(&yield_pool.free);
    QTAILQ_INIT(&yield_pool.used);
}

void qemu_destroy_yield_pool(void)
{
    CPUYield *yield, *tmp;
    CPUYieldPool *pool = &yield_pool;
    QTAILQ_FOREACH_SAFE(yield, &pool->free, next, tmp) {
        QTAILQ_REMOVE(&pool->free, yield, next);
        free(yield);
    }
    QTAILQ_FOREACH_SAFE(yield, &pool->used, next, tmp) {
        QTAILQ_REMOVE(&pool->used, yield, next);
        free(yield);
    }
}

static CPUYield* create_yield(uint64_t magic_pc)
{
    CPUYield* yield = (CPUYield*)calloc(1, sizeof(CPUYield));
    yield->coroutine = qemu_coroutine_create(NULL);
    yield->magic_pc = magic_pc;
    return yield;
}

CPUYield* allocate_yield(void)
{
    CPUYieldPool *pool = &yield_pool;

    if (QTAILQ_EMPTY(&pool->free)) {
        CPUYield* yield = create_yield(YIELD_MAGIC_PC);
        QTAILQ_INSERT_TAIL(&pool->free, yield, next);
    }

    CPUYield* yield = QTAILQ_FIRST(&pool->free);
    QTAILQ_REMOVE(&pool->free, yield, next);
    QTAILQ_INSERT_HEAD(&pool->used, yield, next);

    return yield;
}

void release_yield(CPUYield* yield)
{
    yield->ignore_next = 0;
    CPUYieldPool* pool = &yield_pool;

    QTAILQ_REMOVE(&pool->used, yield, next);
    QTAILQ_INSERT_HEAD(&pool->free, yield, next);
}

static CPUYield* find_yield(CPUState* env, uint64_t magic_pc)
{
    CPUYieldPool* pool = &yield_pool;
    CPUYield* yield = 0;

    QTAILQ_FOREACH(yield, &pool->used, next) {
        if (yield->magic_pc == magic_pc)
            break;
    }
    return yield;
}

void cpu_yield(CPUState *env)
{
#if defined(TARGET_ARM)
    int thumb = ARM_CPU(env)->env.thumb;
#endif

    /* TODO: debug mode. */
    qemu_coroutine_yield();

#if defined(TARGET_ARM)
    ARM_CPU(env)->env.thumb = thumb;
#endif
}

void cpu_finish_yield(CPUState *env)
{
    uint64_t pc = cpu_get_pc(env);
    CPUYield* yield = find_yield(env, pc);
    if (!yield) {
        fprintf(stderr, "Cannot find yield environment: spurios MAGIC_PC.\n");
        abort();
    }

    CoroutineResult status = qemu_coroutine_continue(yield->coroutine, 0, 0);
    if (status == COROUTINE_RESULT_FINISH) {
        if (yield->ignore_next) {
            pc = cpu_get_pc(env);
#ifdef TARGET_ARM
            pc &= ~1;
#endif
            if (pc == yield->origin_pc)
                *yield->ignore_next = 1;
        }
        release_yield(yield);
    }
}

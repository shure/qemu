/* -*- mode: c; c-basic-offset: 4; -*- */

#include "qemu/osdep.h"
#include "cpu.h"
#include "qom/cpu.h"
#include "translate-all.h"
#include "qemu-yield.h"
#include "qemu-tracepoint.h"
#include <glib.h>

typedef QTAILQ_HEAD(tracepoints_head, CPUTracepoint) CPUTracepointList;

typedef struct CPUTracepointGroup
{
    vaddr pc;
    CPUTracepointList list;
    int ignore_next;
} CPUTracepointGroup;

static void page_invalidate(CPUState *cpu, vaddr pc)
{
#ifdef TARGET_ARM
    CPUARMState *env = &ARM_CPU(cpu)->env;
    if (IS_M(env) && pc >= 0xffff0000) {
        tb_flush(0);
        return;
    }
#endif
#ifndef CONFIG_USER_ONLY
    hwaddr phys_addr = cpu_get_phys_page_debug(cpu, pc);
    if (phys_addr != -1) {
        phys_addr |= (pc & ~TARGET_PAGE_MASK);
        tb_invalidate_phys_page_range(phys_addr, phys_addr + 1, false);
    }
#else
    tb_invalidate_phys_page_range(pc, pc + 1, false);
#endif
}

CPUTracepoint* cpu_tracepoint_insert(CPUState* env,
                                     vaddr pc,
                                     TraceCallback callback,
                                     TraceCondition condition)
{
    CPUTracepoint *tp;
    CPUTracepointGroup *group;
    page_invalidate(env, pc);

    if (!env->tracepoints) {
        env->tracepoints = g_hash_table_new_full(g_int64_hash,
                                                 g_int64_equal,
                                                 NULL, g_free);
    }

    group = g_hash_table_lookup(env->tracepoints, &pc);
    if (!group) {
        group = g_new0(CPUTracepointGroup, 1);
        group->pc = pc;
        QTAILQ_INIT(&group->list);
        g_hash_table_insert(env->tracepoints, &group->pc, group);
    }

    tp = g_new0(CPUTracepoint, 1);
    tp->pc = pc;
    tp->callback = callback;
    tp->condition = condition;

    QTAILQ_INSERT_TAIL(&group->list, tp, entry);
    return tp;
}

void cpu_tracepoint_remove(CPUState* env,
                           CPUTracepoint* tracepoint)
{
    CPUTracepointGroup *group;
    vaddr pc = tracepoint->pc;

    if (!env->tracepoints) {
        return;
    }

    group = g_hash_table_lookup(env->tracepoints, &pc);
    if (!group) {
        return;
    }

    page_invalidate(env, pc);
    QTAILQ_REMOVE(&group->list, tracepoint, entry);
    if (QTAILQ_EMPTY(&group->list)) {
        group->ignore_next = 0;
    }
}

static void group_caller(void* opaque)
{
    CPUTracepointGroup *group = (CPUTracepointGroup *)opaque;
    CPUTracepointList *list = &group->list;
    CPUTracepoint *tp;

    QTAILQ_FOREACH(tp, list, entry) {
        tp->callback.callback(tp->callback.opaque);
    }
}

static void group_caller_yield(void* opaque)
{
    CPUTracepointGroup *group = (CPUTracepointGroup *)opaque;
    if (group->ignore_next) {
        group->ignore_next = 0;
        return;
    }

    CPUYield* yield = allocate_yield();
    yield->ignore_next = &group->ignore_next;
    yield->origin_pc = group->pc;
    CoroutineResult result = qemu_coroutine_continue(yield->coroutine, &group_caller, opaque);
    if (result == COROUTINE_RESULT_FINISH) {
        release_yield(yield);
    }
}

static void group_caller_cond(void* opaque)
{
    CPUTracepointGroup *group = (CPUTracepointGroup *)opaque;
    CPUTracepointList *list = &group->list;
    CPUTracepoint *tp;

    QTAILQ_FOREACH(tp, list, entry) {
        if (!tp->condition.condition || tp->condition.condition(tp->condition.opaque)) {
            tp->callback.callback(tp->callback.opaque);
        }
    }
}

static void group_caller_cond_yield(void* opaque)
{
    CPUTracepointGroup *group = (CPUTracepointGroup *)opaque;
    if (group->ignore_next) {
        group->ignore_next = 0;
        return;
    }

    CPUYield* yield = allocate_yield();
    yield->ignore_next = &group->ignore_next;
    yield->origin_pc = group->pc;
    CoroutineResult status = qemu_coroutine_continue(yield->coroutine, &group_caller_cond, opaque);
    if (status == COROUTINE_RESULT_FINISH) {
        release_yield(yield);
    }
}

TraceCallback cpu_get_tracepoints_code(CPUState* env,
                                       vaddr pc)
{
    CPUTracepoint *tp, *single_tp;
    CPUTracepointGroup *group;

    TraceCallback hook;
    hook.callback = 0;
    hook.opaque = 0;
    hook.flags = 0;

    if (!env->tracepoints) {
        return hook;
    }

    group = g_hash_table_lookup(env->tracepoints, &pc);
    if (!group) {
        return hook;
    }

    CPUTracepointList *list = &group->list;
    if (QTAILQ_EMPTY(list)) {
        return hook;
    }

    single_tp = 0;
    QTAILQ_FOREACH(tp, list, entry) {
        if (single_tp) {
            single_tp = 0;
            break;
        }
        single_tp = tp;
    }

    if (single_tp && !single_tp->condition.condition) {
        hook = single_tp->callback;
    } else {
        hook.callback = &group_caller;
        QTAILQ_FOREACH(tp, list, entry) {
            if (tp->condition.condition) {
                hook.callback = &group_caller_cond;
                break;
            }
        }
        hook.opaque = group;
        QTAILQ_FOREACH(tp, list, entry) {
            hook.flags |= tp->callback.flags;
        }
    }

    if (hook.flags & TP_VOLATILE) {
        if (hook.callback == &group_caller)
            hook.callback = &group_caller_yield;
        else if (hook.callback == group_caller_cond)
            hook.callback = &group_caller_cond_yield;
        else {
            hook.callback = &group_caller_yield;
            hook.opaque = group;
        }
    }

    return hook;
}

void cpu_run_tracepoints(CPUState* env)
{
    uint64_t pc = cpu_get_pc(env);
#ifdef TARGET_ARM
    pc &= ~1;
#endif

    TraceCallback cb = cpu_get_tracepoints_code(env, pc);
    if (cb.callback) {
        cb.callback(cb.opaque);
    }
}

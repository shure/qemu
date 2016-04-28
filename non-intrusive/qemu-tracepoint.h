/* -*- mode: c++; c-basic-offset: 4; -*- */

#ifndef __QEMU_TRACEPOINT_H__
#define __QEMU_TRACEPOINT_H__

#include "qemu-common.h"
#include "qom/cpu.h"

CPUTracepoint* cpu_tracepoint_insert(CPUState* env,
                                     vaddr pc,
                                     TraceCallback callback,
                                     TraceCondition condition);

void cpu_tracepoint_remove(CPUState* env,
                           CPUTracepoint* obj);

#endif


#ifndef _QEMU_SYMBOL_TRACE_H_
#define _QEMU_SYMBOL_TRACE_H_

#include "qemu-common.h"
#include "qom/cpu.h"

void cpu_add_symbol_trace(CPUState* env, SymbolTraceApiTable* trace);
void cpu_remove_symbol_trace(CPUState* env, SymbolTraceApiTable* trace);

#endif

/* -*- mode: c++; c-basic-offset: 4; -*- */

#include "qemu/osdep.h"
#include "qemu-symbol-trace.h"
#include "exec/exec-all.h"

void cpu_add_symbol_trace(CPUState* env, SymbolTraceApiTable* new_trace)
{
  SymbolTraceApiTable** ptr;
  unsigned count = env->symbol_trace_count;
  env->symbol_trace =
    (SymbolTraceApiTable**)g_realloc(env->symbol_trace, (count+1) * sizeof(*ptr));
  env->symbol_trace[count] = new_trace;
  env->symbol_trace_count = count + 1;
  tb_flush(0);
}

void cpu_remove_symbol_trace(CPUState* env, SymbolTraceApiTable* trace_to_remove)
{
  unsigned i, old_count, new_count = 0;
  SymbolTraceApiTable **old_ptr, **old_array, **new_array;

  old_array = env->symbol_trace;
  if (!old_array) {
    return;
  }

  old_count = env->symbol_trace_count;

  for (i = 0, old_ptr = old_array; i < old_count; ++i, ++old_ptr) {
    if (*old_ptr != trace_to_remove) {
      new_count++;
    }
  }

  if (new_count) {
    SymbolTraceApiTable **new_ptr;
    new_array = new_ptr = (SymbolTraceApiTable**)g_malloc0(new_count * sizeof(*old_ptr));
    for (i = 0, old_ptr = old_array; i < old_count; ++i, ++old_ptr) {
      if (*old_ptr != trace_to_remove) {
        *new_ptr++ = *old_ptr;
      }
    }
  } else {
    new_array = 0;
  }

  env->symbol_trace = new_array;
  env->symbol_trace_count = new_count;
  g_free(old_array);
  tb_flush(0);
}

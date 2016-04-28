
#ifndef _NON_INTRUSIVE_LOADER_H_
#define _NON_INTRUSIVE_LOADER_H_

#include "qemu-common.h"
#include "qom/cpu.h"

#define YIELD_MAGIC_PC 0xfffffff8
void cpu_finish_yield(CPUState *cpu);

TraceCallback cpu_get_tracepoints_code(CPUState* env,
                                       vaddr pc);

void cpu_run_tracepoints(CPUState *cpu);

void cpu_notify_mode_change(CPUState* cpu);
bool cpu_has_watchpoints_overriding_mmu(CPUState *cs, vaddr address, int size);

void nit_load_plugin(const char *library, int argc, const char **argv);
void nit_init_thread(void);

#endif

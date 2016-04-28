
#ifndef __QEMU_RAW_CONTEXT_h__
#define __QEMU_RAW_CONTEXT_h__

#include "qemu-common.h"
#include "qom/cpu.h"

void cpu_get_raw_context(CPUState* env, RawContext* context, int is_smp);

#endif

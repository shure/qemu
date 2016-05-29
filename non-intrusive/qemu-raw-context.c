/* -*- mode: c++; c-basic-offset: 4; -*- */

#include "qemu/osdep.h"
#include "cpu.h"
#include "qom/cpu.h"
#include "exec/cpu-defs.h"
#include "qemu-raw-context.h"
#include "qemu-yield.h"
#include "non-intrusive/loader.h"

#ifndef CONFIG_USER_ONLY
static void cpu_simulate_page_fault(CPUState *cs, target_ulong addr, int access_mode)
{
    int mmu_idx, page_index;
    target_ulong* tlb_addr;

    CPUArchState *cpu = cs->env_ptr;
    mmu_idx = cpu_mmu_index(cpu, false);
    page_index = (addr >> TARGET_PAGE_BITS) & (CPU_TLB_SIZE - 1);
    switch (access_mode) {
    case 1:
        tlb_addr = &cpu->tlb_table[mmu_idx][page_index].addr_write;
        break;
    case 2:
        tlb_addr = &cpu->tlb_table[mmu_idx][page_index].addr_code;
        break;
    case 0:
    default:
        tlb_addr = &cpu->tlb_table[mmu_idx][page_index].addr_read;
        break;
    }

    if (likely((addr & TARGET_PAGE_MASK) ==
               (*tlb_addr & (TARGET_PAGE_MASK | TLB_INVALID_MASK)))) {
        return;
    }

    int old_exception_index = cs->exception_index;
    sigjmp_buf old_jmp_env;
    *old_jmp_env = *cs->jmp_env;
    if (sigsetjmp(cs->jmp_env, 0) == 0) {
        tlb_fill(cs, addr, access_mode, mmu_idx, 0);
    }
    *cs->jmp_env = *old_jmp_env;

    if (unlikely(old_exception_index != cs->exception_index)) {
        target_ulong pc = cpu_get_pc(cs);
        cpu_set_pc(cs, YIELD_MAGIC_PC);
        CPU_GET_CLASS(cs)->do_interrupt(cs);
        cs->exception_index = -1;
        cpu_yield(cs);
        cs->exception_index = old_exception_index;
        cpu_set_pc(cs, pc);
    }
}

static void cpu_simulate_exception(CPUState *cs, int exception_index)
{
    target_ulong pc = cpu_get_pc(cs);
    cpu_set_pc(cs, YIELD_MAGIC_PC);
    int old_exception_index = cs->exception_index;
    cs->exception_index = exception_index;
#ifdef TARGET_ARM
    CPUArchState *cpu = cs->env_ptr;
    uint32_t old_syndrome = cpu->exception.syndrome;
    uint32_t old_target_el = cpu->exception.target_el;
    cpu->exception.syndrome = (0x15 << 26) | (1 << 25) | (0 & 0xffff);
    cpu->exception.target_el = 1;
#endif
    CPU_GET_CLASS(cs)->do_interrupt(cs);
    cs->exception_index = -1;
    cpu_yield(cs);
    cs->exception_index = old_exception_index;
#ifdef TARGET_ARM
    cpu->exception.syndrome = old_syndrome;
    cpu->exception.target_el = old_target_el;
#endif
    cpu_set_pc(cs, pc);
}
#endif

inline static void swap(uint8_t* a, uint8_t* b)
{
    uint8_t t;
    t = *a; *a = *b; *b = t;
}

inline static void swap_buf(uint8_t* buf, unsigned len)
{
    switch (len) {
    case 2:
        swap(buf + 0, buf + 1);
        break;
    case 4:
        swap(buf + 0, buf + 3); swap(buf + 1, buf + 2);
        break;
    case 8:
        swap(buf + 0, buf + 7); swap(buf + 1, buf + 6);
        swap(buf + 2, buf + 5); swap(buf + 3, buf + 4);
        break;
    default:
        break;
    }
}

#if defined(TARGET_ARM) && !defined(CONFIG_USER_ONLY)
static int cpu_memory_read_escalated(CPUState *cpu, target_ulong addr,
                                     uint8_t *buf, int len)
{
    int l;
    hwaddr phys_addr;
    target_ulong page;

    while (len > 0) {
        page = addr & TARGET_PAGE_MASK;
        phys_addr = arm_cpu_get_phys_page_debug_escalated(cpu, page);
        /* if no physical page mapped, return an error */
        if (phys_addr == -1)
            return -1;
        l = (page + TARGET_PAGE_SIZE) - addr;
        if (l > len)
            l = len;
        phys_addr += (addr & ~TARGET_PAGE_MASK);
        address_space_rw(cpu->as, phys_addr, MEMTXATTRS_UNSPECIFIED,
                         buf, l, 0);
        len -= l;
        buf += l;
        addr += l;
    }
    return 0;
}
#else
static inline int cpu_memory_read_escalated(CPUState *cpu, target_ulong addr,
                                     uint8_t *buf, int len)
{
    return cpu_memory_rw_debug(cpu, addr, buf, len, 0);
}
#endif

static int cpu_read(CPUState* cpu, uint64_t addr, uint8_t* buf, unsigned len,
                    int escalate_mode)
{
    int retv;
    if (escalate_mode)
        retv = cpu_memory_read_escalated(cpu, addr, buf, len);
    else
        retv = cpu_memory_rw_debug(cpu, addr, buf, len, 0);

#ifdef TARGET_WORDS_BIGENDIAN
    swap_buf(buf, len);
#endif
    return retv;
}

static int cpu_fetch(CPUState* cpu, uint64_t addr, uint8_t* buf, unsigned len,
                     int escalate_mode)
{
    int retv = cpu_memory_rw_debug(cpu, addr, buf, len, 2);
#ifdef TARGET_WORDS_BIGENDIAN
    swap_buf(buf, len);
#endif
    return retv;
}

static int cpu_write(CPUState* cpu, uint64_t addr, uint8_t* buf, unsigned len,
                     int escalate_mode)
{
#ifdef TARGET_WORDS_BIGENDIAN
    swap_buf(buf, len);
#endif
    return cpu_memory_rw_debug(cpu, addr, buf, len, 1);
}

static void cpu_notify_mode_change_impl(void *opaque)
{
    CPUState* cpu = opaque;
    CPUEventCallback* cb;

    cpu->mode_callbacks_in_traverse++;

    QTAILQ_FOREACH(cb, &cpu->mode_callbacks, entry) {
        if (cb->deleted)
            continue;
        cb->func(cb->opaque);
    }

    cpu->mode_callbacks_in_traverse--;
    if (!cpu->mode_callbacks_in_traverse) {
    again:
        QTAILQ_FOREACH(cb, &cpu->mode_callbacks, entry) {
            if (cb->deleted) {
                QTAILQ_REMOVE(&cpu->mode_callbacks, cb, entry);
                g_free(cb);
                goto again;
            }
        }
    }
}

void cpu_notify_mode_change(CPUState* cpu)
{
    if (QTAILQ_EMPTY(&cpu->mode_callbacks))
        return;

      CPUYield* yield = allocate_yield();
      CoroutineResult result =
          qemu_coroutine_continue(yield->coroutine, &cpu_notify_mode_change_impl, cpu);
      if (result == COROUTINE_RESULT_FINISH) {
          release_yield(yield);
      }
}

static void cpu_register_mode_callback(CPUState* cpu, void (*func)(void*), void* opaque)
{
    CPUEventCallback* cb = g_new0(CPUEventCallback, 1);
    cb->func = func;
    cb->opaque = opaque;
    cb->deleted = 0;
    QTAILQ_INSERT_TAIL(&cpu->mode_callbacks, cb, entry);
}

static void cpu_unregister_mode_callback(CPUState* cpu, void (*func)(void*), void* opaque)
{
    CPUEventCallback* cb;
    QTAILQ_FOREACH(cb, &cpu->mode_callbacks, entry) {
        if (cb->func == func && cb->opaque == opaque) {
            if (!cpu->mode_callbacks_in_traverse) {
                QTAILQ_REMOVE(&cpu->mode_callbacks, cb, entry);
                g_free(cb);
            } else {
                cb->deleted = 1;
            }
            break;
        }
    }
}

static void smp_register_mode_callback(CPUState* env, void (*func)(void*), void* opaque)
{
    CPUState* cpu;
    CPU_FOREACH(cpu) {
        cpu_register_mode_callback(cpu, func, opaque);
    }
}

static void smp_unregister_mode_callback(CPUState* env, void (*func)(void*), void* opaque)
{
    CPUState* cpu;
    CPU_FOREACH(cpu) {
        cpu_unregister_mode_callback(cpu, func, opaque);
    }
}

static CPUState **local_cpu;

void nit_init_thread(void)
{
    local_cpu = &current_cpu;
}

void cpu_get_raw_context(CPUState* cs, RawContext* context, int is_smp)
{
    int smp_cores = 0;
    if (is_smp) {
        CPUState* cpu;
        CPU_FOREACH(cpu) {
            smp_cores++;
        }
    }

    __attribute__ ((unused)) CPUArchState *env = cs ? cs->env_ptr : 0;
    __attribute__ ((unused)) static char buf[120];

#define PUTS(X) context->puts(context->opaque, X)
#define ADD_SYMBOL(X, Y) context->add_symbol(context->opaque, X, Y)
#define PRINTF(F, ...) { sprintf(buf, F, __VA_ARGS__); context->puts(context->opaque, buf); }

    /* CPU state pointer. */
    PUTS("typedef struct CPUState CPUState;");
    if (!smp_cores) {
        PRINTF("unsigned long CPU_LOC = %p;\n", cs);
        PUTS("#define CPU ((CPUState*)CPU_LOC)");
    } else {
        PUTS("extern CPUState **local_cpu;");
        ADD_SYMBOL("local_cpu", &local_cpu);
        PUTS("#define CPU (*local_cpu)");
    }

    /* CPUArchState is constant offset from CPU. */
    PUTS("typedef struct CPUArchState CPUArchState;");
    PRINTF("#define ENV ((CPUArchState*)((unsigned long)CPU + %ld))",
           (unsigned long)ENV_OFFSET);

    /* Generic host types. */
    PUTS("typedef unsigned long long uint64_t;");
    PUTS("typedef unsigned int uint32_t;");
    PUTS("typedef unsigned short uint16_t;");
    PUTS("typedef unsigned char uint8_t;");
    PUTS("typedef unsigned long ulong_t;");

    PUTS("typedef long long int64_t;");
    PUTS("typedef int int32_t;");
    PUTS("typedef short int16_t;");
    PUTS("typedef char int8_t;");

#ifdef TARGET_WORDS_BIGENDIAN
    PUTS("#define TARGET_BIGENDIAN 1");
#endif

#if defined(TARGET_ARM)
    PUTS("extern int __get_current_el(CPUArchState*);");
    PUTS("inline int get_current_el() { return __get_current_el(ENV); }");
    ADD_SYMBOL("__get_current_el", arm_current_el);
#endif

#if defined(TARGET_ARM) && !defined(TARGET_AARCH64)

    int i;

    /* Target specific types. */
    PUTS("typedef uint32_t addr_t;");
    PUTS("typedef uint32_t target_ulong_t;");
    PUTS("typedef float target_float32;");
    PUTS("typedef double target_float64;");

    /* Mode enumeration. */
    PUTS("typedef enum cpu_mode_t {");
    PUTS("  MODE_USR = 0x10,");
    PUTS("  MODE_FIQ = 0x11,");
    PUTS("  MODE_IRQ = 0x12,");
    PUTS("  MODE_SVC = 0x13,");
    PUTS("  MODE_MON = 0x16,");
    PUTS("  MODE_ABT = 0x17,");
    PUTS("  MODE_UND = 0x1b,");
    PUTS("  MODE_SYS = 0x1f,");
    PUTS("} cpu_mode_t;");

    /* Register aliases. */
    if (!smp_cores) {
        PUTS("extern target_ulong_t r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,"
             "r10,r11,r12,r13,r14,r15;");
        for (i = 0; i <= 15; i++) {
            sprintf(buf, "r%d", i);
            ADD_SYMBOL(buf, &env->regs[i]);
            PRINTF("#define R%d r%d", i, i);
        }

        PUTS("extern target_float32 S0,S1,S2,S3,S4,S5,S6,S7,S8,S9,"
             "S10,S11,S12,S13,S14,S15,S16,S17,S18,S19,S20,S21,S22,"
             "S23,S24,S25,S26,S27,S28,S29,S30,S31;");
        for (i = 0; i <= 31; i++) {
            sprintf(buf, "S%d", i);
            ADD_SYMBOL(buf, (void*)(unsigned long)env + vfp_reg_offset(0, i));
        }

        PUTS("extern target_float64 D0,D1,D2,D3,D4,D5,D6,D7,D8,D9,"
             "D10,D11,D12,D13,D14,D15;");
        for (i = 0; i <= 15; i++) {
            sprintf(buf, "D%d", i);
            ADD_SYMBOL(buf, (void*)(unsigned long)env + vfp_reg_offset(1, i));
        }
        char reg[16];
        for (i = 0; i <= 31; i++) {
            sprintf(reg, "_S%d", i);
            PRINTF("extern uint32_t %s;", reg);
            ADD_SYMBOL(reg, (void*)(unsigned long)env + vfp_reg_offset(0, i));
        }
        for (i = 0; i <= 15; i++) {
            sprintf(reg, "_D%d", i);
            PRINTF("extern uint64_t %s;", reg);
            ADD_SYMBOL(reg, (void*)(unsigned long)env + vfp_reg_offset(1, i));
        }

    } else {
        for (i = 0; i <= 15; i++) {
            PRINTF("#define R%d *((uint32_t*)((unsigned long)ENV + %ld))",
                   i, (unsigned long)offsetof(CPUARMState, regs[i]));
        }

        for (i = 0; i <= 31; i++) {
            PRINTF("#define S%d *((target_float32*)((unsigned long)ENV + %ld))",
                   i, vfp_reg_offset(0, i));
        }

        for (i = 0; i <= 15; i++) {
            PRINTF("#define D%d *((target_float64*)((unsigned long)ENV + %ld))",
                   i, vfp_reg_offset(1, i));
        }

        for (i = 0; i <= 31; i++) {
            PRINTF("#define _S%d *((uint32_t*)((unsigned long)ENV + %ld))",
                   i, vfp_reg_offset(0, i));
        }

        for (i = 0; i <= 15; i++) {
            PRINTF("#define _D%d *((uint64_t*)((unsigned long)ENV + %ld))",
                   i, vfp_reg_offset(1, i));
        }
    }

    /* More register aliases. */
    if (!smp_cores) {
        PUTS("extern target_ulong_t sp, lr, pc;");
        ADD_SYMBOL("sp", &env->regs[13]);
        ADD_SYMBOL("lr", &env->regs[14]);
        ADD_SYMBOL("pc", &env->regs[15]);
        PUTS("#define SP sp");
        PUTS("#define LR lr");
        PUTS("#define PC pc");
    } else {
        PUTS("#define SP R13");
        PUTS("#define LR R14");
        PUTS("#define PC R15");
    }

    /* Thumb field. */
    if (!smp_cores) {
        PUTS("extern int THUMB;");
        ADD_SYMBOL("THUMB", &env->thumb);
    } else {
        PRINTF("#define THUMB *((int*)((unsigned long)ENV + %ld))",
               (unsigned long)offsetof(CPUARMState, thumb));
    }

    /* General API for register for registers. */
    PUTS("uint64_t get_program_counter() { return PC; }");
    PUTS("uint64_t get_stack_pointer() { return SP; }");
    PUTS("uint64_t get_return_address() { return LR & ~1; }");

    PUTS("extern cpu_mode_t __get_mode(CPUArchState*);");
    PUTS("extern target_ulong_t __get_sys_reg(CPUArchState*, cpu_mode_t, int);");
    PUTS("extern void __set_sys_reg(CPUArchState*, cpu_mode_t, int, target_ulong_t);");
    PUTS("extern target_ulong_t __get_cp15_reg(CPUArchState*, int, int, int, int);");
    PUTS("extern void __set_cp15_reg(CPUArchState*, int, int, int, int, target_ulong_t);");
    ADD_SYMBOL("__get_mode", arm32_current_mode);
    ADD_SYMBOL("__get_sys_reg", arm32_get_sys_reg);
    ADD_SYMBOL("__set_sys_reg", arm32_set_sys_reg);
    ADD_SYMBOL("__get_cp15_reg", arm32_get_cp15_reg);
    ADD_SYMBOL("__set_cp15_reg", arm32_set_cp15_reg);

    PUTS("inline cpu_mode_t get_mode() { return __get_mode(ENV); }");

    PUTS("inline uint32_t get_sp(cpu_mode_t m) { return __get_sys_reg(ENV, m, 13); }");
    PUTS("inline void set_sp(cpu_mode_t m, uint32_t v) { __set_sys_reg(ENV, m, 13, v); }");

    PUTS("inline uint32_t get_lr(cpu_mode_t m) { return __get_sys_reg(ENV, m, 14); }");
    PUTS("inline void set_lr(cpu_mode_t m, uint32_t v) { __set_sys_reg(ENV, m, 14, v); }");

    PUTS("inline uint32_t get_spsr(cpu_mode_t m) { return __get_sys_reg(ENV, m, 16); }");
    PUTS("inline void set_spsr(cpu_mode_t m, uint32_t v) { __set_sys_reg(ENV, m, 16, v); }");

    PUTS("inline uint32_t get_reg(cpu_mode_t m, int r) { return __get_sys_reg(ENV, m, r); }");
    PUTS("inline void set_reg(cpu_mode_t m, int r, uint32_t v) { __set_sys_reg(ENV, m, r, v); }");

    PUTS("inline uint32_t get_cp15_reg(int a, int b, int c, int d) { return __get_cp15_reg(ENV, a, b, c, d); }");
    PUTS("inline void set_cp15_reg(int a, int b, int c, int d, uint32_t v) { __set_cp15_reg(ENV, a, b, c, d, v); }");
#endif

#if defined(TARGET_ARM) && defined(TARGET_AARCH64)

    PUTS("#define TARGET_AARCH64 1");

    int i;
    char reg[10];

    /* Target specific types. */
    PUTS("typedef uint64_t addr_t;");
    PUTS("typedef uint64_t target_ulong_t;");
    PUTS("typedef float target_float32;");
    PUTS("typedef double target_float64;");

    if (!smp_cores) {
        for (i = 0; i <= 30; i++) {
            sprintf(reg, "X%d", i);
            PRINTF("extern target_ulong_t %s;", reg);
            ADD_SYMBOL(reg, &env->xregs[i]);
            sprintf(reg, "W%d", i);
            PRINTF("extern uint32_t %s;", reg);
            ADD_SYMBOL(reg, &env->xregs[i]);
        }
        PUTS("extern target_ulong_t SP, PC;");
        ADD_SYMBOL("SP", &env->xregs[31]);
        ADD_SYMBOL("PC", &env->pc);
        /* Floating point registers */
        for (i = 0; i <= 31; i++) {
            sprintf(reg, "D%d", i);
            PRINTF("extern target_float64 %s;", reg);
            ADD_SYMBOL(reg, &env->vfp.regs[i * 2]);
            sprintf(reg, "S%d", i);
            PRINTF("extern target_float32 %s;", reg);
            ADD_SYMBOL(reg, &env->vfp.regs[i * 2]);
            sprintf(reg, "_D%d", i);
            PRINTF("extern uint64_t %s;", reg);
            ADD_SYMBOL(reg, &env->vfp.regs[i * 2]);
            sprintf(reg, "_S%d", i);
            PRINTF("extern uint32_t %s;", reg);
            ADD_SYMBOL(reg, &env->vfp.regs[i * 2]);
        }
    } else {
        for (i = 0; i <= 30; i++) {
            PRINTF("#define X%d *((target_ulong_t*)((unsigned long)ENV + %ld))",
                   i, (unsigned long)offsetof(CPUARMState, xregs[i]));
            PRINTF("#define W%d *((uint32_t*)((unsigned long)ENV + %ld))",
                   i, (unsigned long)offsetof(CPUARMState, xregs[i]));
        }
        PRINTF("#define SP *((target_ulong_t*)((unsigned long)ENV + %ld))",
               (unsigned long)offsetof(CPUARMState, xregs[31]));
        PRINTF("#define PC *((target_ulong_t*)((unsigned long)ENV + %ld))",
               (unsigned long)offsetof(CPUARMState, pc));
        /* Floating point registers */
        for (i = 0; i <= 31; i++) {
            PRINTF("#define D%d *((target_float64*)((unsigned long)ENV + %ld))",
                   i, (unsigned long)offsetof(CPUARMState, vfp.regs[i * 2]));
            PRINTF("#define S%d *((target_float32*)((unsigned long)ENV + %ld))",
                   i, (unsigned long)offsetof(CPUARMState, vfp.regs[i * 2]));
            PRINTF("#define _D%d *((uint64_t*)((unsigned long)ENV + %ld))",
                   i, (unsigned long)offsetof(CPUARMState, vfp.regs[i * 2]));
            PRINTF("#define _S%d *((uint32_t*)((unsigned long)ENV + %ld))",
                   i, (unsigned long)offsetof(CPUARMState, vfp.regs[i * 2]));
        }
    }

    /* Register aliases. */
    PUTS("#define LR X30");
    for (i = 0; i <= 30; i++) {
        PRINTF("#define R%d X%d", i, i);
    }
    PUTS("#define X31 SP");

    // General API for register for registers.
    PUTS("uint64_t get_program_counter() { return PC; }");
    PUTS("uint64_t get_stack_pointer() { return SP; }");
    PUTS("uint64_t get_return_address() { return LR; }");

    if (!smp_cores) {
        PUTS("extern target_ulong_t SP_EL0, SP_EL1, SP_EL2, SP_EL3;");
        ADD_SYMBOL("SP_EL0", &env->sp_el[0]);
        ADD_SYMBOL("SP_EL1", &env->sp_el[1]);
        ADD_SYMBOL("SP_EL2", &env->sp_el[2]);
        ADD_SYMBOL("SP_EL3", &env->sp_el[3]);
    } else {
        for (i = 0; i < 4; i++) {
            PRINTF("#define SP_EL%d *((target_ulong_t*)((unsigned long)ENV + %ld))",
                   i, (unsigned long)offsetof(CPUARMState, sp_el[i]));
        }
    }
#endif

    /* Set next PC: Backward compatibility. Use PC. */
    PUTS("inline void set_next_pc(target_ulong_t val) { PC = val; }");

    PUTS("extern int _cpu_read(CPUState*, uint64_t, uint8_t*, unsigned, int);");
    PUTS("extern int _cpu_fetch(CPUState*, uint64_t, uint8_t*, unsigned, int);");
    PUTS("extern int _cpu_write(CPUState*, uint64_t, uint8_t*, unsigned, int);");
    ADD_SYMBOL("_cpu_read", cpu_read);
    ADD_SYMBOL("_cpu_fetch", cpu_fetch);
    ADD_SYMBOL("_cpu_write", cpu_write);
    PUTS("inline int cpu_read(uint64_t addr, uint8_t* buf, unsigned len, int escalate_mode)"
         "{ return _cpu_read(CPU, addr, buf, len, escalate_mode);}");
    PUTS("inline int cpu_write(uint64_t addr, uint8_t* buf, unsigned len, int escalate_mode)"
         "{ return _cpu_write(CPU, addr, buf, len, escalate_mode);}");
    PUTS("inline int cpu_fetch(uint64_t addr, uint8_t* buf, unsigned len, int escalate_mode)"
         "{ return _cpu_fetch(CPU, addr, buf, len, escalate_mode);}");

    /* ldr, str */
    PUTS("inline uint8_t ldr8(addr_t a) { uint8_t v; cpu_read(a, (uint8_t*)&v, 1, 1); return v; }");
    PUTS("inline uint16_t ldr16(addr_t a) { uint16_t v; cpu_read(a, (uint8_t*)&v, 2, 1); return v; }");
    PUTS("inline uint32_t ldr32(addr_t a) { uint32_t v; cpu_read(a, (uint8_t*)&v, 4, 1); return v; }");
    PUTS("inline uint64_t ldr64(addr_t a) { uint64_t v; cpu_read(a, (uint8_t*)&v, 8, 1); return v; }");
    PUTS("inline uint8_t ldr8_code(addr_t a) { uint8_t v; cpu_fetch(a, (uint8_t*)&v, 1, 1); return v; }");
    PUTS("inline uint16_t ldr16_code(addr_t a) { uint16_t v; cpu_fetch(a, (uint8_t*)&v, 2, 1); return v; }");
    PUTS("inline uint32_t ldr32_code(addr_t a) { uint32_t v; cpu_fetch(a, (uint8_t*)&v, 4, 1); return v; }");
    PUTS("inline uint64_t ldr64_code(addr_t a) { uint64_t v; cpu_fetch(a, (uint8_t*)&v, 8, 1); return v; }");
    PUTS("inline void str8(addr_t a, uint8_t v) { cpu_write(a, (uint8_t*)&v, 1, 1); }");
    PUTS("inline void str16(addr_t a, uint16_t v) { cpu_write(a, (uint8_t*)&v, 2, 1); }");
    PUTS("inline void str32(addr_t a, uint32_t v) { cpu_write(a, (uint8_t*)&v, 4, 1); }");
    PUTS("inline void str64(addr_t a, uint64_t v) {  cpu_write(a, (uint8_t*)&v, 8, 1); }");

#ifndef CONFIG_USER_ONLY
    /* simulate_page_fault */
    PUTS("#define ACCESS_READ 0");
    PUTS("#define ACCESS_WRITE 1");
    PUTS("#define ACCESS_CODE 2");
    PUTS("extern void __simulate_page_fault(CPUState*, target_ulong_t, int);");
    PUTS("inline void simulate_page_fault(target_ulong_t addr, int access) {");
    PUTS("__simulate_page_fault(CPU, addr, access); }");
    ADD_SYMBOL("__simulate_page_fault", cpu_simulate_page_fault);

    PUTS("extern void __simulate_exception(CPUState*, int);");
    PUTS("inline void simulate_exception(int exception_index) {");
    PUTS("__simulate_exception(CPU, exception_index); }");
    ADD_SYMBOL("__simulate_exception", cpu_simulate_exception);
#endif

    PUTS("extern void __yield(CPUState*);");
    PRINTF("#define YIELD_MAGIC_PC 0x%x", YIELD_MAGIC_PC);
    PUTS("inline void yield(addr_t a) {");
    PUTS("addr_t pc0 = PC, lr0 = LR;");
#if defined(TARGET_ARM) && !defined(TARGET_AARCH64)
    PUTS("int thumb0 = THUMB;");
    PUTS("if (a & 1) { THUMB = 1; a &= ~1; } else THUMB = 0;");
#endif
    PUTS("PC = a; LR = YIELD_MAGIC_PC;");
    PUTS("__yield(CPU);");
    PUTS("PC = pc0; LR = lr0;");
#if defined(TARGET_ARM) && !defined(TARGET_AARCH64)
    PUTS("THUMB = thumb0;");
#endif
    PUTS("}");
    ADD_SYMBOL("__yield", cpu_yield);

    PUTS("extern void __register_mode_callback(CPUState*, void (*)(void*), void*);");
    PUTS("void register_mode_callback(void (*func)(void*), void* opaque) { "
         "__register_mode_callback(CPU, func, opaque); }");
    PUTS("extern void __unregister_mode_callback(CPUState*, void (*)(void*), void*);");
    PUTS("void unregister_mode_callback(void (*func)(void*), void* opaque) { "
         "__unregister_mode_callback(CPU, func, opaque); }");
    if (!smp_cores) {
        ADD_SYMBOL("__register_mode_callback", cpu_register_mode_callback);
        ADD_SYMBOL("__unregister_mode_callback", cpu_unregister_mode_callback);
    } else {
        ADD_SYMBOL("__register_mode_callback", smp_register_mode_callback);
        ADD_SYMBOL("__unregister_mode_callback", smp_unregister_mode_callback);
    }

    if (!smp_cores) {
        PUTS("#define SMP_CORES 1");
        PUTS("#define SMP_CURRENT 0");
    } else {
        PRINTF("#define SMP_CORES %d", smp_cores);
        PRINTF("#define SMP_CURRENT *((const uint32_t*)((unsigned long)CPU + %ld))",
               (unsigned long)offsetof(CPUState, cpu_index));
    }

    /* Watchpoint parameters. */
    if (!smp_cores) {
        PUTS("extern uint64_t IO_ADDR;");
        PUTS("extern int IO_SIZE;");
        PUTS("extern uint64_t IO_VALUE;");
        PUTS("extern int IO_ABORT;");
        ADD_SYMBOL("IO_ADDR", &cs->IO_ADDR);
        ADD_SYMBOL("IO_SIZE", &cs->IO_SIZE);
        ADD_SYMBOL("IO_VALUE", &cs->IO_VALUE);
        ADD_SYMBOL("IO_ABORT", &cs->IO_ABORT);
    } else {
        PRINTF("#define IO_ADDR *((target_ulong_t*)((unsigned long)CPU + %ld))",
               (unsigned long)offsetof(CPUState, IO_ADDR));
        PRINTF("#define IO_SIZE *((int*)((unsigned long)CPU + %ld))",
               (unsigned long)offsetof(CPUState, IO_SIZE));
        PRINTF("#define IO_VALUE *((uint64_t*)((unsigned long)CPU + %ld))",
               (unsigned long)offsetof(CPUState, IO_VALUE));
        PRINTF("#define IO_ABORT *((int*)((unsigned long)CPU + %ld))",
               (unsigned long)offsetof(CPUState, IO_ABORT));
    }

    PUTS("inline void ret_insn(void) { PC = LR; }");

#undef PUTS
#undef ADD_SYMBOL
}

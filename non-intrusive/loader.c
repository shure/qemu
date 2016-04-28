/* -*- c-basic-offset: 4 -*- */

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <glib.h>

#include "qemu/osdep.h"
#include "qemu/compiler.h"
#include "qemu/timer.h"
#include "non-intrusive/loader.h"
#include "non-intrusive/trace-api.h"
#include "qemu-symbol-trace.h"
#include "qemu-yield.h"
#include "qemu-tracepoint.h"
#include "qemu-raw-context.h"
#include "qemu-common.h"
#include "qom/cpu.h"

static void GCC_FMT_ATTR(1, 2) fatal(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    exit(2);
}

struct TraceSource {
    char *name;
    CPUState *cpu;
    bool is_smp;
};

static GHashTable *trace_source_map;

static void register_core_trace_source(const char *name, CPUState *cpu, bool is_smp)
{
    TraceSource *source = g_new(TraceSource, 1);
    source->name = g_strdup(name);
    source->cpu = cpu;
    source->is_smp = is_smp;

    if (!trace_source_map)
        trace_source_map = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, g_free);
    g_hash_table_insert(trace_source_map, source->name, source);
}

static void register_core_trace_sources(void)
{
    CPUState *cpu;
    CPU_FOREACH(cpu) {
        char name[20];
        sprintf(name, "core%d", cpu->cpu_index);
        register_core_trace_source(name, cpu, false);
    }
    register_core_trace_source("smp", 0, true);
}

static void get_trace_sources(void (*out)(void*, TraceSource *), const char* object_path_regexp,
                              const char* supported_trace_kind_regexp, int tree, void* opaque)
{
}

static TraceSource *find_trace_source(const char* name)
{
    if (trace_source_map) {
        TraceSource *trace_source = g_hash_table_lookup(trace_source_map, name);
        if (trace_source)
            return trace_source;
    }
    /* Find in peripheral space */
    return 0;
}

static const char *get_name(TraceSource *source)
{
    return source->name;
}

#ifndef CONFIG_USER_ONLY
static U64 sc_now(void)
{
    return qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
}

static U64 get_time_stamp(void)
{
    return qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
}

static U64 get_time_stamp_res(void)
{
    return qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
}
#endif

static const char *get_architecture(TraceSource *source)
{
#if defined(TARGET_ARM) && defined(TARGET_AARCH64)

#if defined(TARGET_WORDS_BIGENDIAN)
    return "aarch64eb";
#else
    return "aarch64";
#endif

#elif defined(TARGET_ARM) && !defined(TARGET_AARCH64)

#if defined(TARGET_WORDS_BIGENDIAN)
    return "armeb";
#else
    return "arm";
#endif

#else
    return "unknown";
#endif
}

static void get_raw_context(TraceSource *source, RawContext* context)
{
    cpu_get_raw_context(source->cpu, context, source->is_smp);
}

static const char* get_debug_elf_image(TraceSource *source)
{
    return 0;
}

static void add_symbol_trace(TraceSource *source, SymbolTraceApiTable* symbol_trace)
{
    if (source->is_smp) {
        CPUState* cpu;
        CPU_FOREACH(cpu) {
            cpu_add_symbol_trace(cpu, symbol_trace);
        }
    } else {
        cpu_add_symbol_trace(source->cpu, symbol_trace);
    }
}

static void remove_symbol_trace(TraceSource *source, SymbolTraceApiTable* symbol_trace)
{
    if (source->is_smp) {
        CPUState* cpu;
        CPU_FOREACH(cpu) {
            cpu_remove_symbol_trace(cpu, symbol_trace);
        }
    } else {
        cpu_remove_symbol_trace(source->cpu, symbol_trace);
    }
}

static void invalidate_symbol_trace(TraceSource *source, SymbolTraceApiTable* symbol_trace)
{
    tb_flush(0);
}

static unsigned long insert_tracepoint(TraceSource *source, U64 pc,
                                       TraceCallback callback,
                                       TraceCondition condition)
{
    if (source->is_smp) {
        CPUTracepoint *first = 0, *prev = 0;
        CPUState* cpu;
        CPU_FOREACH(cpu) {
            CPUTracepoint* object =
                cpu_tracepoint_insert(cpu, pc, callback, condition);
            if (!first)
                first = object;
            if (prev)
                prev->smp_next = object;
            prev = object;
        }
        return (unsigned long)first;
    } else {
        return (unsigned long)cpu_tracepoint_insert(source->cpu, pc, callback, condition);
    }
}

static void remove_tracepoint(TraceSource *source, unsigned long ref)
{
    CPUTracepoint* obj = (CPUTracepoint*)ref;
    if (source->is_smp) {
        CPUState* cpu;
        CPU_FOREACH(cpu) {
            CPUTracepoint* next = obj->smp_next;
            cpu_tracepoint_remove(cpu, obj);
            obj = next;
        }
    } else {
        cpu_tracepoint_remove(source->cpu, obj);
    }
}

static unsigned long insert_watchpoint(TraceSource *source,
                                       U64 addr, U64 length,
                                       int is_read, int is_write,
                                       TraceCallback pre_callback,
                                       TraceCallback post_callback,
                                       TraceCondition condition)
{
    int flags = 0;
    if (is_read) flags |= BP_MEM_READ;
    if (is_write) flags |= BP_MEM_WRITE;

    if (source->is_smp) {
        CPUWatchpoint *first = 0, *prev = 0;
        CPUState* cpu;
        CPU_FOREACH(cpu) {
            CPUWatchpoint* object;
            if (cpu_watchpoint_insert(cpu, addr, length, flags, &object) != 0)
                return 0; // Error should occur in first core.
            object->pre_callback = pre_callback;
            object->post_callback = post_callback;
            object->condition = condition;
            if (!first)
                first = object;
            if (prev)
                prev->smp_next = object;
            prev = object;
        }
        return (unsigned long)first;
    } else {
        CPUWatchpoint* object;
        if (cpu_watchpoint_insert(source->cpu, addr, length, flags, &object) != 0)
            return 0;
        object->pre_callback = pre_callback;
        object->post_callback = post_callback;
        object->condition = condition;
        return (unsigned long)object;
    }
}

static void remove_watchpoint(TraceSource *source, unsigned long ref)
{
    CPUWatchpoint* obj = (CPUWatchpoint*)ref;
    if ((obj->pre_callback.flags | obj->post_callback.flags) & TP_MULTICORE) {
        CPUState* cpu;
        CPU_FOREACH(cpu) {
            CPUWatchpoint* next = obj->smp_next;
            cpu_watchpoint_remove_by_ref(cpu, obj);
            obj = next;
        }
    } else {
        cpu_watchpoint_remove_by_ref(source->cpu, obj);
    }
}

static void request_shutdown(TraceSource *source)
{
    printf("NIT: request_shutdown\n");
}

static void request_sim_stop(void)
{
    printf("NIT: request_sim_stop\n");
    exit(0);
}

static void request_reset(TraceSource *source)
{
    printf("NIT: request_reset\n");
}

static void debug_interrupt(TraceSource *source, int signal, const char* message)
{
    printf("NIT: debug_interrupt\n");
}

static const char* get_representative_name(TraceSource *source)
{
    return source->name;
}

static const char* get_parameter(const char* name)
{
    return 0;
}

static void set_parameter(const char* name, const char* value)
{
}

static TraceApiTable* api;

void nit_load_plugin(const char *name, int argc, const char **argv)
{
    register_core_trace_sources();
    qemu_init_yield_pool();

    void* handle = dlopen(name, RTLD_LOCAL|RTLD_LAZY);
    if (!handle) {
        fatal("Cannon load trace library: %s\n.", name);
    }

    typedef TraceApiTable* (*main_func_t)(void);
    main_func_t main_func;
    main_func = (main_func_t) dlsym(handle, "get_trace_api_table");
    if (!main_func) {
        fatal("get_trace_api_table function not found in: %s.\n", name);
    }

    api = (*main_func)();
    if (!api) {
        fatal("get_trace_api_table returned null in: %s.\n", name);
    }

    if (api->api_version != TRACE_API_VERSION) {
        fatal("trace api version in %s is %X and not %X, as expected.\n", name,
              api->api_version, TRACE_API_VERSION);
    }

    api->get_trace_sources = get_trace_sources;
    api->find_trace_source = find_trace_source;

#ifndef CONFIG_USER_ONLY
    api->sc_now = sc_now;
    api->get_time_stamp = get_time_stamp;
    api->get_time_stamp_res = get_time_stamp_res;
#endif

    api->request_sim_stop = request_sim_stop;

    api->get_trace_source_name = get_name;
    api->get_supported_traces = NULL;

    api->get_architecture = get_architecture;
    api->get_raw_context = get_raw_context;
    api->get_debug_elf_image = get_debug_elf_image;
    api->add_symbol_trace = add_symbol_trace;
    api->remove_symbol_trace = remove_symbol_trace;
    api->invalidate_symbol_trace = invalidate_symbol_trace;
    api->insert_tracepoint = insert_tracepoint;
    api->remove_tracepoint = remove_tracepoint;
    api->insert_watchpoint = insert_watchpoint;
    api->remove_watchpoint = remove_watchpoint;
    api->request_shutdown = request_shutdown;
    api->request_reset = request_reset;
    api->debug_interrupt = debug_interrupt;
    api->get_representative_name = get_representative_name;
    api->get_parameter = get_parameter;
    api->set_parameter = set_parameter;

    int status = api->init(argc, argv);
    if (status != 0) {
        fatal("%s: trace init function return error status: %d\n", name, status);
    }
}

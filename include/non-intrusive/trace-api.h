/*
 * Trace library API.
 *
 * Copyright (c) 2016 Alex Rozenman,
 * Copyright (c) 2016 Mentor Graphics corp.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/* General discussion:

    Note: This is an API file. Keep it completely standalone.

    An updated version of this file is always available at:

    https://raw.githubusercontent.com/shure/qemu/non-intrusive/\
    include/non-intrusive/trace-api.h

    This file describes an API b/w 'simulator' and 'trace library'.
    The simulator should load the trace library using dlopen and perform
    the initialization step using the following (or equivalent) code:

    #include "trace-api.h"

    void* handle = dlopen(path_to_trace_library, RTLD_LOCAL|RTLD_LAZY);
    if (!handle) {
        fatal("Cannon load trace library: %s\n.", path_to_trace_library);
    }

    typedef TraceApiTable* (*main_func_t)(void);
    main_func_t main_func;
    main_func = (main_func_t) dlsym(handle, "get_trace_api_table");
    if (!main_func) {
        fatal("get_trace_api_table function not found in: %s.\n",
              name);
    }

    api = (*main_func)();
    if (!api) {
        fatal("get_trace_api_table returned null in: %s.\n", name);
    }

    if (api->api_version != TRACE_API_VERSION) {
        fatal("trace api version in %s is %X and not %X, as expected.\n",
              name, api->api_version, TRACE_API_VERSION);
    }

    Then it should fill the function pointers marked as "filled by
    simulator" and "mandatory" inside TraceApiTable and call to "init"
    function:

    int status = api->init(argc, argv);
    if (status != 0) {
        fatal("%s: trace init function return error status: %d\n", name, status);
    }

    The API is based on a notion of TraceSource. TraceSource is handle inside simulator
    that can provide a traceing services. In a simplest case, simulator should provide
    one trace source (CPU core). In this case "find_trace_source" should look like
    the following:

    static TraceSource *find_trace_source(const char* name) {
        if (!strcmp(name, "core")) {
            static TraceSource core_trace_source;
            return &core_trace_source;
        } else {
            return 0;
        }
    }

    The simulator should describe the CPU "context" (i.e. registers, memory IO,
    special purpose registers) for the trace library.
    A notion of RawContext is used for this purpose. The RawContext is defined as
    set of "C" definitions (variables, functions, defines) together with their
    implementations (addresses of variables and functions) inside the simulator core.
    The definition are added to the RawContext during get_raw_context callback which
    gets "RawContext" object. The RawContext objects allows to add "C" definition and
    provide their addresses. For example, in order to define registers implementated
    inside simulator as variables, the following code can be added in "get_raw_context"
    function:

    static void get_raw_context(TraceSource* source, RawContext* context)
    {
        static char buf[120];
        #define PUTS(X) context->puts(context->opaque, X)
        #define ADD_SYMBOL(X, Y) context->add_symbol(context->opaque, X, Y)
        #define PRINTF(F, ...) { sprintf(buf, F, __VA_ARGS__); context->puts(context->opaque, buf); }

        PUTS("extern target_ulong_t r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,"
             "r10,r11,r12,r13,r14,r15;");
        for (i = 0; i <= 15; i++) {
            sprintf(buf, "r%d", i);
            // Suppose that env->regs[i] it's the place where simulator stores "Ri" register.
            ADD_SYMBOL(buf, &env->regs[i]);
            PRINTF("#define R%d r%d", i, i);
        }
    }

    The following definitions are mandatory inside the RawContext:

    // generic host types:
    typedef unsigned long long uint64_t;
    typedef unsigned int uint32_t;
    typedef unsigned short uint16_t;
    typedef unsigned char uint8_t;
    typedef unsigned long ulong_t;
    typedef long long int64_t;
    typedef int int32_t;
    typedef short int16_t;
    typedef char int8_t;

    // target address type:
    typedef uint32_t addr_t;

    // Memory IO:
    uint8_t ldr8(addr_t a);
    uint16_t ldr16(addr_t a);
    uint32_t ldr32(addr_t a);
    uint64_t ldr64(addr_t a);
    void str8(addr_t a, uint8_t v);
    void str16(addr_t a, uint16_t v);
    void str32(addr_t a, uint32_t v);
    void str64(addr_t a, uint64_t v);

    // Code fetch IO:
    uint8_t ldr8_code(addr_t a);
    uint16_t ldr16_code(addr_t a);
    uint32_t ldr32_code(addr_t a);
    uint64_t ldr64_code(addr_t a);

    // Get current PC:
    uint64_t get_program_counter();

    // Get stack pointer:
    uint64_t get_stack_pointer();

    // Get link register:
    uint64_t get_return_address();
*/

#pragma once

#ifdef __cplusplus
namespace trace_api {
#endif

#define TRACE_API_VERSION_MAJOR 0x01
#define TRACE_API_VERSION_MINOR 0x11
#define TRACE_API_VERSION ((TRACE_API_VERSION_MAJOR << 16) | TRACE_API_VERSION_MINOR)

typedef long long I64;
typedef unsigned long long U64;

/* Symbol trace is used to trace inline function calls.
   It is not mandatory to implement symbol trace in the simulator.
   When it implemented, simulator should call lookup_symbol function of every registered
   symbol trace on every PC and store the returned "Symbol" in a internal data structure.
   When a code is executed by the simulator it should keep tracking of currently executing
   "Symbol" and call to "symbol_changed" callback each time when current symbol is changed. */
typedef const void* Symbol;
typedef struct SymbolTraceApiTable SymbolTraceApiTable;
struct SymbolTraceApiTable
{
  /* Called by simulator to translate PC into Symbol. */
  Symbol (*lookup_symbol)(SymbolTraceApiTable* obj, U64 pc);

  /* Called by simulator when symbol is changed in execution. */
  void (*symbol_changed)(SymbolTraceApiTable* obj);

  Symbol current_symbol;
};

typedef struct ValueTraceApiTable ValueTraceApiTable;
struct ValueTraceApiTable
{
  void (*value_changed)(ValueTraceApiTable* obj, void* value);
};

/* Optional Socket trace. */
typedef struct SocketTraceApiTable SocketTraceApiTable;
struct SocketTraceApiTable
{
  void (*start_read)(SocketTraceApiTable* obj, U64 tid, U64 time, U64 address, unsigned length, U64 data);
  void (*end_read)(SocketTraceApiTable* obj, U64 tid, U64 time, U64 address, unsigned length, U64 data);

  void (*start_write)(SocketTraceApiTable* obj, U64 tid, U64 time, U64 address, unsigned length, U64 data);
  void (*end_write)(SocketTraceApiTable* obj, U64 tid, U64 time);

  void (*interrupt)(SocketTraceApiTable* obj, U64 tid, U64 time, int on);
};

/* Optional and deprecated "register" trace. Replaced by Value trace. */
typedef struct RegisterTraceApiTable RegisterTraceApiTable;
struct RegisterTraceApiTable
{
  void (*value_changed)(RegisterTraceApiTable* obj, void* value);
};

/* Optional Cache trace. */
typedef struct CacheTraceApiTable CacheTraceApiTable;
struct CacheTraceApiTable
{
  /* Filled by trace client. */
  void (*read_hit)(CacheTraceApiTable* obj, U64 addr);
  void (*write_hit)(CacheTraceApiTable* obj, U64 addr);
  void (*read_miss)(CacheTraceApiTable* obj, U64 addr);
  void (*write_miss)(CacheTraceApiTable* obj, U64 addr);
  void (*line_fill)(CacheTraceApiTable* obj, U64 addr);

  /* Incremented by simulator. */
  U64 read_hit_count;
  U64 write_hit_count;
  U64 read_miss_count;
  U64 write_miss_count;
  U64 line_fill_count;
};

typedef struct RawContext RawContext;
struct RawContext {
  void* opaque;
  void (*puts)(void* opaque, const char* str);
  void (*add_symbol)(void* opaque, const char* symbol, const void* address);
};

typedef enum CacheSpec
{
  CACHE_L1_DATA = 100 + 'D',
  CACHE_L1_INSN = 100 + 'I',
  CACHE_L2_DATA = 200 + 'D',
  CACHE_L2_INSN = 200 + 'I',
  CACHE_L3_DATA = 300 + 'D',
  CACHE_L3_INSN = 300 + 'I',
} CacheSpec;

#ifdef __cplusplus
inline int get_cache_level(CacheSpec cacheSpec)
{
  return ((unsigned int)cacheSpec) / 100;
}
inline char get_cache_type(CacheSpec cacheSpec)
{
  return (char) (((unsigned int)cacheSpec) % 100);
}
#endif

#define TP_NONE (0)
#define TP_VOLATILE (1 << 1)
#define TP_MULTICORE (1 << 2)
#define WP_OVERRIDE_MMU (1 << 3)

struct TraceCallback {
    void (*callback)(void*);
    void* opaque;
    unsigned flags; /* TP_ flags here */

#ifdef __cplusplus
    TraceCallback()
        : callback(0), opaque(0), flags(0) {}
    TraceCallback(void (*callback)(void*), void* opaque, unsigned flags = 0)
        : callback(callback), opaque(opaque), flags(flags) {}
#endif
};

struct TraceCondition {
    int (*condition)(void*);
    void* opaque;

#ifdef __cplusplus
    TraceCondition()
        : condition(0), opaque(0) {
    }
    TraceCondition(int (*condition)(void*), void* opaque)
        : condition(condition), opaque(opaque) {
    }
    bool evaluate() const {
        return !condition || condition(opaque);
    }
    bool is_empty() {
        return (condition == 0);
    }
    operator bool() const {
        return (condition != 0);
    }
#endif
};

typedef struct TraceCallback TraceCallback;
typedef struct TraceCondition TraceCondition;

struct TraceOpcode
{
    /* Architecture index. */
    int arch_index;

    /* Instruction data. */
    U64 mask, value;
    unsigned int length;

#ifdef __cplusplus
  TraceOpcode() : arch_index(0), mask(0), value(0), length(0) {
  }
#endif
};

struct ValueType {

  enum TraceValueType {
    UnsignedType,
    IntegerType,
    DoubleType,
    StringType,
  } TraceValueType;

  enum TraceValueType m_type;
  union { U64 unsigned_value; I64 int_value; double double_value; const char* string_value; } m_value;

#ifdef __cplusplus
  ValueType(U64 value) : m_type(UnsignedType) { m_value.unsigned_value = value; }
  ValueType(I64 value) : m_type(IntegerType) { m_value.int_value = value; }
  ValueType(double value) : m_type(DoubleType) { m_value.double_value = value; }
  ValueType(const char* value) : m_type(StringType) { m_value.string_value = value; }
  template <typename T> void get_value(T& value) {
    if (m_type == IntegerType) value = m_value.int_value;
    else if (m_type == UnsignedType) value = m_value.unsigned_value;
    else if (m_type == DoubleType) value = m_value.double_value;
    else value = 0;
  }
#endif
};

typedef struct TraceOpcode TraceOpcode;
typedef struct ValueType ValueType;

typedef struct TraceSource TraceSource;
typedef struct TraceApiTable TraceApiTable;

struct TraceApiTable
{
  unsigned api_version;
  const char* library_name;

  /* Filled by trace library. */

  /* Return 0 or error code. */
  int (*init)(int argc, const char** argv);
  int (*evaluate_command)(const char* command, int argc, const char** argv,
                          void (*out)(const char*));
  void (*flush)(int close_section);
  void (*garbage_collect)(void);

  /* Filled by simulator before calling init. */

  /* Mandatory: Return current simulation time in nanoseconds. */
  U64 (*sc_now)(void);

  /* Mandatory: Return current simulation time in nanoseconds. */
  U64 (*get_time_stamp)(void);

  /* Return current simulation time in simulation resolution. */
  U64 (*get_time_stamp_res)(void);

  /* Request to finish the simulation. */
  void (*request_sim_stop)(void);

  /* Enumerate trace objects available in the simulator. */
  void (*get_trace_sources)(void (*out)(void*, TraceSource*), const char* object_path_regexp,
                            const char* supported_trace_kind_regexp, int tree, void* opaque);

  /* Mandatory: Find trace object. This function should return at least one trace object.
     See generic discussion for more details. */
  TraceSource* (*find_trace_source)(const char* path);

  /* Return trace source name. */
  const char* (*get_trace_source_name)(TraceSource* source);

  /* Return an array of strings descring trace kinds supported by
     trace source. */
  const char** (*get_supported_traces)(TraceSource* source);

  /* Mandatory: Return architecture name of a CPU core.
     Possible architecture names are: aarch64, arm, nec, ppc, sh4, tricore. */
  const char* (*get_architecture)(TraceSource* source);

  /* Mandatory: Describe CPU architecture.
     When called (by trace library; implemented in simulator), this function
     should describe CPU context. The context is described in terms of "C" variables
     and functions. See general discussion for more details. */
  void (*get_raw_context)(TraceSource* source, RawContext* context);

  /* Register and unregister symbol trace. */
  void (*add_symbol_trace)(TraceSource* source, SymbolTraceApiTable* trace);
  void (*remove_symbol_trace)(TraceSource* source, SymbolTraceApiTable* symbol_file);
  void (*invalidate_symbol_trace)(TraceSource* source, SymbolTraceApiTable* symbol_file);

  /* Mandatory: Insert tracepoint calling to a callback before CPU is executing an
     instruction at the specifid PC. If condition is provided, tracepoint should be called
     only when the condition is true. Should return a handle (ref) for removal. */
  unsigned long (*insert_tracepoint)(TraceSource* source, U64 pc,
                                     TraceCallback callback,
                                     TraceCondition condition);
  void (*remove_tracepoint)(TraceSource* source, unsigned long ref);

  /* Insert memory watchpoint. */
  unsigned long (*insert_watchpoint)(TraceSource* source,
                                     U64 addr, U64 length,
                                     int is_read, int is_write,
                                     TraceCallback pre_callback,
                                     TraceCallback post_callback,
                                     TraceCondition condition);
  void (*remove_watchpoint)(TraceSource* source, unsigned long ref);

  /* Insert instruction trace. */
  unsigned long (*insert_insn_trace)(TraceSource* source,
                                     TraceOpcode opcode,
                                     TraceCallback pre_callback,
                                     TraceCallback post_callback,
                                     TraceCondition condition);
  void (*remove_insn_trace)(TraceSource* source, unsigned long ref);

  /* Get path to an ELF file currently running in simulator. */
  const char* (*get_debug_elf_image)(TraceSource* source);

  /* Optional and deprecated. */
  TraceSource* (*get_associated_cache)(TraceSource* core, CacheSpec cacheSpec);

  /* Request shutdown, reset and debug interrupt. */
  void (*request_shutdown)(TraceSource* core);
  void (*request_reset)(TraceSource* core);
  void (*debug_interrupt)(TraceSource* core, int signal, const char* message);

  /* Value trace api. */
  void (*register_value_trace)(TraceSource* source, ValueTraceApiTable* trace);
  int (*write_value)(TraceSource* source, const ValueType value);
  int (*read_value)(TraceSource* source, ValueType* value);

  /* Socket trace api. */
  void (*register_socket_trace)(TraceSource* source, SocketTraceApiTable* trace);

  /* Register trace api. */
  void (*register_mb_register_trace)(TraceSource* source, RegisterTraceApiTable* trace);

  /* Cache trace api. */
  void (*register_cache_trace)(TraceSource* source, CacheTraceApiTable* trace);
  void (*unregister_cache_trace)(TraceSource* source, CacheTraceApiTable* trace);

  const char* (*get_representative_name)(TraceSource* source);
  void (*disable_trace)(TraceSource* source);
  void *(*get_initial_value)(TraceSource* source);

  /* Access to parameters from trace library. */
  const char* (*get_parameter)(const char* name);
  void (*set_parameter)(const char* name, const char* value);
};

#ifdef __cplusplus
}
#endif


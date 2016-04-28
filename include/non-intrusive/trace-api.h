
/* This is an API file. Keep it standalone. */

#pragma once

#ifdef __cplusplus
namespace trace_api {
#endif

#define TRACE_API_VERSION_MAJOR 0x01
#define TRACE_API_VERSION_MINOR 0x11
#define TRACE_API_VERSION ((TRACE_API_VERSION_MAJOR << 16) | TRACE_API_VERSION_MINOR)

typedef long long I64;
typedef unsigned long long U64;

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

typedef struct SocketTraceApiTable SocketTraceApiTable;
struct SocketTraceApiTable
{
  void (*start_read)(SocketTraceApiTable* obj, U64 tid, U64 time, U64 address, unsigned length, U64 data);
  void (*end_read)(SocketTraceApiTable* obj, U64 tid, U64 time, U64 address, unsigned length, U64 data);

  void (*start_write)(SocketTraceApiTable* obj, U64 tid, U64 time, U64 address, unsigned length, U64 data);
  void (*end_write)(SocketTraceApiTable* obj, U64 tid, U64 time);

  void (*interrupt)(SocketTraceApiTable* obj, U64 tid, U64 time, int on);
};

typedef struct RegisterTraceApiTable RegisterTraceApiTable;
struct RegisterTraceApiTable
{
  void (*value_changed)(RegisterTraceApiTable* obj, void* value);
};

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

  /* Return 0 or error code. */
  int (*init)(int argc, const char** argv);
  int (*evaluate_command)(const char* command, int argc, const char** argv,
                          void (*out)(const char*));
  void (*flush)(int close_section);
  void (*garbage_collect)(void);

  /* Filled by simulator before calling init. */

  U64 (*sc_now)(void);
  U64 (*get_time_stamp)(void);
  U64 (*get_time_stamp_res)(void);

  /* Request to finish the simulation. */
  void (*request_sim_stop)(void);

  void (*get_trace_sources)(void (*out)(void*, TraceSource*), const char* object_path_regexp, const char* supported_trace_kind_regexp, int tree, void* opaque);

  TraceSource* (*find_trace_source)(const char* path);

  const char* (*get_trace_source_name)(TraceSource* source);
  const char** (*get_supported_traces)(TraceSource* source);

  const char* (*get_architecture)(TraceSource* source);
  void (*get_raw_context)(TraceSource* source, RawContext* context);

  void (*add_symbol_trace)(TraceSource* source, SymbolTraceApiTable* symbol_file);
  void (*remove_symbol_trace)(TraceSource* source, SymbolTraceApiTable* symbol_file);
  void (*invalidate_symbol_trace)(TraceSource* source, SymbolTraceApiTable* symbol_file);

  unsigned long (*insert_tracepoint)(TraceSource* source, U64 pc,
                                     TraceCallback callback,
                                     TraceCondition condition);
  void (*remove_tracepoint)(TraceSource* source, unsigned long ref);

  unsigned long (*insert_watchpoint)(TraceSource* source,
                                     U64 addr, U64 length,
                                     int is_read, int is_write,
                                     TraceCallback pre_callback,
                                     TraceCallback post_callback,
                                     TraceCondition condition);
  void (*remove_watchpoint)(TraceSource* source, unsigned long ref);

  unsigned long (*insert_insn_trace)(TraceSource* source,
                                     TraceOpcode opcode,
                                     TraceCallback pre_callback,
                                     TraceCallback post_callback,
                                     TraceCondition condition);
  void (*remove_insn_trace)(TraceSource* source, unsigned long ref);

  const char* (*get_debug_elf_image)(TraceSource* source);

  TraceSource* (*get_associated_cache)(TraceSource* core, CacheSpec cacheSpec);

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


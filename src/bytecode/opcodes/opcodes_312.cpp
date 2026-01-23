// Python 3.11-3.13 opcode table implementation (with inline caches)
// Reference version is 3.12

#include "opcodes_312.hpp"

namespace pyc {
namespace bytecode {
namespace opcodes {

using op = operand_type;
using F = insn_flag;

const insn_def table_311_313[] = {
    // Cache pseudo-instruction
    { 0,   "CACHE",                op::none,       0, F::flag_none, 0 },
    
    // Stack manipulation
    { 1,   "POP_TOP",              op::none,      -1, F::flag_pop, 0 },
    { 2,   "PUSH_NULL",            op::none,       1, F::flag_push, 0 },
    { 3,   "INTERPRETER_EXIT",     op::none,       0, F::flag_stop, 0 },
    { 4,   "END_FOR",              op::none,      -1, F::flag_pop, 0 },
    { 5,   "END_SEND",             op::none,      -1, F::flag_pop, 0 },
    { 9,   "NOP",                  op::none,       0, F::flag_none, 0 },
    { 11,  "UNARY_NEGATIVE",       op::none,       0, F::flag_none, 0 },
    { 12,  "UNARY_NOT",            op::none,       0, F::flag_none, 0 },
    { 15,  "UNARY_INVERT",         op::none,       0, F::flag_none, 0 },
    { 25,  "BINARY_SUBSCR",        op::none,      -1, F::flag_cache, 1 },
    { 26,  "BINARY_SLICE",         op::none,      -2, F::flag_none, 0 },
    { 27,  "STORE_SLICE",          op::none,      -4, F::flag_store, 0 },
    { 30,  "GET_LEN",              op::none,       1, F::flag_push, 0 },
    { 31,  "MATCH_MAPPING",        op::none,       1, F::flag_push, 0 },
    { 32,  "MATCH_SEQUENCE",       op::none,       1, F::flag_push, 0 },
    { 33,  "MATCH_KEYS",           op::none,       1, F::flag_push, 0 },
    { 35,  "PUSH_EXC_INFO",        op::none,       1, F::flag_push | F::flag_except, 0 },
    { 36,  "CHECK_EXC_MATCH",      op::none,      -1, F::flag_except, 0 },
    { 37,  "CHECK_EG_MATCH",       op::none,       0, F::flag_except, 0 },
    { 50,  "WITH_EXCEPT_START",    op::none,       1, F::flag_except, 0 },
    { 51,  "GET_AITER",            op::none,       0, F::flag_none, 0 },
    { 52,  "GET_ANEXT",            op::none,       1, F::flag_push, 0 },
    { 53,  "BEFORE_ASYNC_WITH",    op::none,       1, F::flag_push, 0 },
    { 54,  "BEFORE_WITH",          op::none,       1, F::flag_push, 0 },
    { 55,  "END_ASYNC_FOR",        op::none,      -2, F::flag_except, 0 },
    { 56,  "CLEANUP_THROW",        op::none,      -2, F::flag_except, 0 },
    { 60,  "STORE_SUBSCR",         op::none,      -3, F::flag_store | F::flag_cache, 1 },
    { 61,  "DELETE_SUBSCR",        op::none,      -2, F::flag_del, 0 },
    { 68,  "GET_ITER",             op::none,       0, F::flag_none, 0 },
    { 69,  "GET_YIELD_FROM_ITER",  op::none,       0, F::flag_none, 0 },
    { 71,  "LOAD_BUILD_CLASS",     op::none,       1, F::flag_push, 0 },
    { 75,  "LOAD_ASSERTION_ERROR", op::none,       1, F::flag_push, 0 },
    { 76,  "RETURN_GENERATOR",     op::none,       0, F::flag_ret, 0 },
    { 83,  "RETURN_VALUE",         op::none,      -1, F::flag_ret | F::flag_stop, 0 },
    { 86,  "SETUP_ANNOTATIONS",    op::none,       0, F::flag_none, 0 },
    { 89,  "POP_EXCEPT",           op::none,      -1, F::flag_pop | F::flag_except, 0 },
    
    // Opcodes with arguments
    { 90,  "STORE_NAME",           op::name_idx,  -1, F::flag_store, 0 },
    { 91,  "DELETE_NAME",          op::name_idx,   0, F::flag_del, 0 },
    { 92,  "UNPACK_SEQUENCE",      op::byte,       0, F::flag_cache, 1 },
    { 93,  "FOR_ITER",             op::jrel,       1, F::flag_jump | F::flag_cond | F::flag_cache, 1 },
    { 94,  "UNPACK_EX",            op::byte,       0, F::flag_none, 0 },
    { 95,  "STORE_ATTR",           op::name_idx,  -2, F::flag_store | F::flag_cache, 4 },
    { 96,  "DELETE_ATTR",          op::name_idx,  -1, F::flag_del, 0 },
    { 97,  "STORE_GLOBAL",         op::name_idx,  -1, F::flag_store, 0 },
    { 98,  "DELETE_GLOBAL",        op::name_idx,   0, F::flag_del, 0 },
    { 99,  "SWAP",                 op::byte,       0, F::flag_none, 0 },
    { 100, "LOAD_CONST",           op::const_idx,  1, F::flag_load | F::flag_push, 0 },
    { 101, "LOAD_NAME",            op::name_idx,   1, F::flag_load | F::flag_push, 0 },
    { 102, "BUILD_TUPLE",          op::byte,       0, F::flag_none, 0 },
    { 103, "BUILD_LIST",           op::byte,       0, F::flag_none, 0 },
    { 104, "BUILD_SET",            op::byte,       0, F::flag_none, 0 },
    { 105, "BUILD_MAP",            op::byte,       0, F::flag_none, 0 },
    { 106, "LOAD_ATTR",            op::name_idx,   0, F::flag_load | F::flag_cache, 9 },
    { 107, "COMPARE_OP",           op::compare,   -1, F::flag_cache, 1 },
    { 108, "IMPORT_NAME",          op::name_idx,  -1, F::flag_none, 0 },
    { 109, "IMPORT_FROM",          op::name_idx,   1, F::flag_push, 0 },
    { 110, "JUMP_FORWARD",         op::jrel,       0, F::flag_jump, 0 },
    { 114, "POP_JUMP_IF_FALSE",    op::jrel,      -1, F::flag_jump | F::flag_cond, 0 },
    { 115, "POP_JUMP_IF_TRUE",     op::jrel,      -1, F::flag_jump | F::flag_cond, 0 },
    { 116, "LOAD_GLOBAL",          op::name_idx,   1, F::flag_load | F::flag_push | F::flag_cache, 4 },
    { 117, "IS_OP",                op::byte,      -1, F::flag_none, 0 },
    { 118, "CONTAINS_OP",          op::byte,      -1, F::flag_none, 0 },
    { 119, "RERAISE",              op::byte,      -1, F::flag_stop | F::flag_except, 0 },
    { 120, "COPY",                 op::byte,       1, F::flag_push, 0 },
    { 121, "RETURN_CONST",         op::const_idx,  0, F::flag_ret | F::flag_stop, 0 },
    { 122, "BINARY_OP",            op::byte,      -1, F::flag_cache, 1 },
    { 123, "SEND",                 op::jrel,       0, F::flag_cache, 1 },
    { 124, "LOAD_FAST",            op::local_idx,  1, F::flag_load | F::flag_push, 0 },
    { 125, "STORE_FAST",           op::local_idx, -1, F::flag_store, 0 },
    { 126, "DELETE_FAST",          op::local_idx,  0, F::flag_del, 0 },
    { 127, "LOAD_FAST_CHECK",      op::local_idx,  1, F::flag_load | F::flag_push, 0 },
    { 128, "POP_JUMP_IF_NOT_NONE", op::jrel,      -1, F::flag_jump | F::flag_cond, 0 },
    { 129, "POP_JUMP_IF_NONE",     op::jrel,      -1, F::flag_jump | F::flag_cond, 0 },
    { 130, "RAISE_VARARGS",        op::byte,       0, F::flag_stop, 0 },
    { 131, "GET_AWAITABLE",        op::byte,       0, F::flag_none, 0 },
    { 132, "MAKE_FUNCTION",        op::byte,       0, F::flag_none, 0 },
    { 133, "BUILD_SLICE",          op::byte,       0, F::flag_none, 0 },
    { 134, "JUMP_BACKWARD_NO_INTERRUPT", op::jback, 0, F::flag_jump, 0 },
    { 135, "MAKE_CELL",            op::free_idx,   0, F::flag_none, 0 },
    { 136, "LOAD_CLOSURE",         op::free_idx,   1, F::flag_load | F::flag_push, 0 },
    { 137, "LOAD_DEREF",           op::free_idx,   1, F::flag_load | F::flag_push, 0 },
    { 138, "STORE_DEREF",          op::free_idx,  -1, F::flag_store, 0 },
    { 139, "DELETE_DEREF",         op::free_idx,   0, F::flag_del, 0 },
    { 140, "JUMP_BACKWARD",        op::jback,      0, F::flag_jump | F::flag_cache, 1 },
    { 141, "LOAD_SUPER_ATTR",      op::name_idx,  -2, F::flag_load | F::flag_cache, 1 },
    { 142, "CALL_FUNCTION_EX",     op::byte,      -1, F::flag_call, 0 },
    { 143, "LOAD_FAST_AND_CLEAR",  op::local_idx,  1, F::flag_load | F::flag_push, 0 },
    
    { 144, "EXTENDED_ARG",         op::byte,       0, F::flag_none, 0 },
    
    { 145, "LIST_APPEND",          op::byte,      -1, F::flag_none, 0 },
    { 146, "SET_ADD",              op::byte,      -1, F::flag_none, 0 },
    { 147, "MAP_ADD",              op::byte,      -2, F::flag_none, 0 },
    { 149, "COPY_FREE_VARS",       op::byte,       0, F::flag_none, 0 },
    { 150, "YIELD_VALUE",          op::byte,       0, F::flag_yield, 0 },
    { 151, "RESUME",               op::byte,       0, F::flag_none, 0 },
    { 152, "MATCH_CLASS",          op::byte,      -2, F::flag_none, 0 },
    { 155, "FORMAT_SIMPLE",        op::none,       0, F::flag_none, 0 },
    { 156, "FORMAT_WITH_SPEC",     op::none,      -1, F::flag_none, 0 },
    { 160, "LOAD_LOCALS",          op::none,       1, F::flag_push, 0 },
    { 162, "LOAD_FROM_DICT_OR_GLOBALS", op::name_idx, 0, F::flag_load, 0 },
    { 163, "LOAD_FROM_DICT_OR_DEREF",   op::free_idx, 0, F::flag_load, 0 },
    { 164, "SET_FUNCTION_ATTRIBUTE",    op::byte,    -1, F::flag_none, 0 },
    { 171, "CALL",                 op::byte,       0, F::flag_call | F::flag_cache, 3 },
    { 172, "KW_NAMES",             op::const_idx,  0, F::flag_none, 0 },
    { 173, "CALL_INTRINSIC_1",     op::intrinsic1, 0, F::flag_call, 0 },
    { 174, "CALL_INTRINSIC_2",     op::intrinsic2,-1, F::flag_call, 0 },
    { 181, "LIST_EXTEND",          op::byte,      -1, F::flag_none, 0 },
    { 182, "SET_UPDATE",           op::byte,      -1, F::flag_none, 0 },
    { 183, "DICT_MERGE",           op::byte,      -1, F::flag_none, 0 },
    { 184, "DICT_UPDATE",          op::byte,      -1, F::flag_none, 0 },
    
    // Sentinel
    { 0xFF, nullptr, op::none, 0, F::flag_none, 0 }
};

} // namespace opcodes
} // namespace bytecode
} // namespace pyc

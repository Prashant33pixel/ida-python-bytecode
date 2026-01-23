// Python bytecode operand type definitions
// How to interpret instruction arguments

#ifndef PYC_BYTECODE_INSTRUCTION_OPERAND_TYPES_HPP
#define PYC_BYTECODE_INSTRUCTION_OPERAND_TYPES_HPP

#include <cstdint>

namespace pyc {
namespace bytecode {

// Operand types - how to interpret instruction arguments
enum class operand_type : uint8_t {
    none        = 0,    // No operand
    byte        = 1,    // Raw byte value (count, etc.)
    const_idx   = 2,    // Index into co_consts
    name_idx    = 3,    // Index into co_names (globals, attrs)
    local_idx   = 4,    // Index into co_varnames (LOAD_FAST, etc.)
    free_idx    = 5,    // Index into co_cellvars + co_freevars
    jrel        = 6,    // Relative jump (forward)
    jback       = 7,    // Relative jump (backward)
    jabs        = 8,    // Absolute jump (pre-3.10, byte offset)
    compare     = 9,    // Comparison operator (cmp_op)
    intrinsic1  = 10,   // Intrinsic function ID (1 arg) - 3.12+
    intrinsic2  = 11,   // Intrinsic function ID (2 args) - 3.12+
    hasarg      = 12,   // Has argument (generic, display as number)
};

// Check if operand type is a jump
inline bool is_jump_operand(operand_type t) {
    return t == operand_type::jrel || 
           t == operand_type::jback || 
           t == operand_type::jabs;
}

// Check if operand type references code object data
inline bool is_index_operand(operand_type t) {
    return t == operand_type::const_idx || 
           t == operand_type::name_idx || 
           t == operand_type::local_idx || 
           t == operand_type::free_idx;
}

} // namespace bytecode
} // namespace pyc

#endif // PYC_BYTECODE_INSTRUCTION_OPERAND_TYPES_HPP

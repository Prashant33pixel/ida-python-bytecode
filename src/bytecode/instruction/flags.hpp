// Python bytecode instruction flags
// Describe instruction behavior and characteristics

#ifndef PYC_BYTECODE_INSTRUCTION_FLAGS_HPP
#define PYC_BYTECODE_INSTRUCTION_FLAGS_HPP

#include <cstdint>

namespace pyc {
namespace bytecode {

// Instruction flags - bit flags describing instruction behavior
enum insn_flag : uint16_t {
    flag_none       = 0x0000,
    flag_jump       = 0x0001,   // Jump/branch instruction
    flag_cond       = 0x0002,   // Conditional (may fall through)
    flag_call       = 0x0004,   // Call instruction
    flag_ret        = 0x0008,   // Return instruction
    flag_stop       = 0x0010,   // Stops execution (return, raise, reraise)
    flag_push       = 0x0020,   // Pushes to stack
    flag_pop        = 0x0040,   // Pops from stack
    flag_load       = 0x0080,   // Load operation
    flag_store      = 0x0100,   // Store operation
    flag_del        = 0x0200,   // Delete operation
    flag_except     = 0x0400,   // Exception handling
    flag_yield      = 0x0800,   // Generator yield
    flag_cache      = 0x1000,   // Has inline cache entries (3.11+)
};

// Combine flags (enum | enum)
inline constexpr uint16_t operator|(insn_flag a, insn_flag b) {
    return static_cast<uint16_t>(a) | static_cast<uint16_t>(b);
}

// Combine flags (uint16_t | enum) - for chaining
inline constexpr uint16_t operator|(uint16_t a, insn_flag b) {
    return a | static_cast<uint16_t>(b);
}

// Check if flags contain specific flag
inline bool has_flag(uint16_t flags, insn_flag f) {
    return (flags & static_cast<uint16_t>(f)) != 0;
}

// Check if instruction is unconditional jump
inline bool is_unconditional_jump(uint16_t flags) {
    return has_flag(flags, flag_jump) && !has_flag(flags, flag_cond);
}

// Check if instruction terminates basic block
inline bool terminates_block(uint16_t flags) {
    return has_flag(flags, flag_ret) || 
           has_flag(flags, flag_stop) || 
           is_unconditional_jump(flags);
}

// Check if instruction has control flow continuation
inline bool has_flow(uint16_t flags) {
    return !terminates_block(flags);
}

} // namespace bytecode
} // namespace pyc

#endif // PYC_BYTECODE_INSTRUCTION_FLAGS_HPP

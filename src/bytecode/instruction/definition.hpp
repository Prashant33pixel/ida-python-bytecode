// Python bytecode instruction definition structure
// Contains all metadata needed to decode and display an instruction

#ifndef PYC_BYTECODE_INSTRUCTION_DEFINITION_HPP
#define PYC_BYTECODE_INSTRUCTION_DEFINITION_HPP

#include <cstdint>
#include "operand_types.hpp"
#include "flags.hpp"

namespace pyc {
namespace bytecode {

// Instruction definition - describes a single Python opcode
struct insn_def {
    uint8_t         opcode;         // Opcode number
    const char*     mnemonic;       // Human-readable name
    operand_type    op_type;        // How to interpret the operand
    int8_t          stack_effect;   // Simplified stack effect
    uint16_t        flags;          // Instruction behavior flags
    uint8_t         cache_entries;  // Number of CACHE instructions following (3.11+)
};

// Lookup instruction definition by opcode in a table
// Returns nullptr if opcode not found
inline const insn_def* find_insn_def(const insn_def* table, uint8_t opcode) {
    if (!table) return nullptr;
    for (const insn_def* def = table; def->mnemonic != nullptr; ++def) {
        if (def->opcode == opcode)
            return def;
    }
    return nullptr;
}

// Get instruction size including cache entries
inline int get_insn_total_size(const insn_def* def, int base_size) {
    if (!def) return base_size;
    return base_size + def->cache_entries * 2;
}

} // namespace bytecode
} // namespace pyc

#endif // PYC_BYTECODE_INSTRUCTION_DEFINITION_HPP

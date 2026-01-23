// Opcode table selection and lookup
// Provides unified access to version-specific opcode tables

#ifndef PYC_BYTECODE_OPCODES_TABLE_HPP
#define PYC_BYTECODE_OPCODES_TABLE_HPP

#include <cstdint>
#include "../instruction/definition.hpp"
#include "../../core/version/version.hpp"

namespace pyc {
namespace bytecode {
namespace opcodes {

// Get opcode table for a specific version family
const insn_def* get_table(version::family f);

// Global state for current version's opcode table
void set_current_table(version::family f);
version::family get_current_family();
const insn_def* get_current_insn_def(uint8_t opcode);

// Alias for set_current_table (for compatibility)
inline void set_version_family(version::family f) {
    set_current_table(f);
}

// Check if opcode has an argument for current version
bool opcode_has_arg(uint8_t opcode);

// Get instruction size for current version
int get_insn_size(uint8_t opcode);

// Get number of cache entries for instruction (3.11+)
int get_cache_count(uint8_t opcode);

} // namespace opcodes
} // namespace bytecode
} // namespace pyc

#endif // PYC_BYTECODE_OPCODES_TABLE_HPP

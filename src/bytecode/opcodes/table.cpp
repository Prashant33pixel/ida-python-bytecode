// Opcode table selection and lookup implementation
// Provides unified access to version-specific opcode tables

#include "table.hpp"
#include "opcodes_27.hpp"
#include "opcodes_36.hpp"
#include "opcodes_312.hpp"
#include "opcodes_314.hpp"

namespace pyc {
namespace bytecode {
namespace opcodes {

// Global state
static const insn_def* g_current_table = nullptr;
static version::family g_current_family = version::family::unknown;

const insn_def* get_table(version::family f) {
    switch (f) {
        case version::family::py2x:
            return table_27;
        case version::family::py30_35:
            return table_36_310;  // Close enough for basic ops
        case version::family::py36_310:
            return table_36_310;
        case version::family::py311_313:
            return table_311_313;
        case version::family::py314_plus:
            return table_314;
        default:
            return table_314;
    }
}

void set_current_table(version::family f) {
    g_current_family = f;
    g_current_table = get_table(f);
}

version::family get_current_family() {
    return g_current_family;
}

const insn_def* get_current_insn_def(uint8_t opcode) {
    return find_insn_def(g_current_table, opcode);
}

bool opcode_has_arg(uint8_t opcode) {
    switch (g_current_family) {
        case version::family::py2x:
        case version::family::py30_35:
            // Pre-wordcode: opcodes >= 90 have arguments
            return opcode >= 90;
        default:
            // Wordcode (3.6+): all instructions have arg byte (even if 0)
            return true;
    }
}

int get_insn_size(uint8_t opcode) {
    const insn_def* def = get_current_insn_def(opcode);
    int base_size;
    
    switch (g_current_family) {
        case version::family::py2x:
        case version::family::py30_35:
            // Pre-wordcode: 1 byte for no-arg, 3 bytes for arg
            base_size = opcode_has_arg(opcode) ? 3 : 1;
            break;
        default:
            // Wordcode: always 2 bytes
            base_size = 2;
            break;
    }
    
    // Add cache entries for 3.11+ (each cache is 2 bytes)
    if (def && version::has_caches(g_current_family)) {
        base_size += def->cache_entries * 2;
    }
    
    return base_size;
}

int get_cache_count(uint8_t opcode) {
    if (!version::has_caches(g_current_family))
        return 0;
    
    const insn_def* def = get_current_insn_def(opcode);
    return def ? def->cache_entries : 0;
}

} // namespace opcodes
} // namespace bytecode
} // namespace pyc

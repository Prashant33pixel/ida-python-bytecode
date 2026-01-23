// Global constants and netnode definitions
// Used for storing metadata in IDA database

#ifndef PYC_CORE_CONFIG_CONSTANTS_HPP
#define PYC_CORE_CONFIG_CONSTANTS_HPP

#include <cstdint>

namespace pyc {
namespace config {

// Netnode names
constexpr const char* NODE_NAME = "$ PYC";
constexpr const char* CODE_NODE_PREFIX = "$ PYC_CO_";

// Netnode indices for main PYC node
enum node_idx : uint32_t {
    NODE_VERSION_MAJOR  = 1,
    NODE_VERSION_MINOR  = 2,
    NODE_VERSION_FAMILY = 3,
    NODE_HAS_CACHES     = 4,
    NODE_WORD_SIZE      = 5,
};

// Netnode indices for code object nodes
enum code_node_idx : uint32_t {
    CO_ARGCOUNT         = 1,
    CO_POSONLYARGCOUNT  = 2,
    CO_KWONLYARGCOUNT   = 3,
    CO_NLOCALS          = 4,
    CO_STACKSIZE        = 5,
    CO_FLAGS            = 6,
    CO_FIRSTLINENO      = 7,
    CO_CODE_SIZE        = 8,
    // Blob indices (supval)
    CO_CONSTS_BLOB      = 0,    // Serialized constants
    CO_NAMES_BLOB       = 1,    // Null-separated names
    CO_VARNAMES_BLOB    = 2,    // Null-separated varnames
    CO_FREEVARS_BLOB    = 3,    // Null-separated freevars
    CO_CELLVARS_BLOB    = 4,    // Null-separated cellvars
};

// Memory layout constants
constexpr uint32_t CODE_BASE_ADDRESS = 0x10000;
constexpr uint32_t CODE_ALIGNMENT    = 0x100;

} // namespace config
} // namespace pyc

#endif // PYC_CORE_CONFIG_CONSTANTS_HPP

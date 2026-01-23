// Python code object structure
// Represents a parsed code object from marshal data

#ifndef PYC_MARSHAL_CODE_OBJECT_CODE_OBJECT_HPP
#define PYC_MARSHAL_CODE_OBJECT_CODE_OBJECT_HPP

#include <cstdint>
#include <cstddef>

namespace pyc {
namespace marshal {

// Code object flags (co_flags)
enum code_flag : uint32_t {
    CO_OPTIMIZED            = 0x0001,
    CO_NEWLOCALS            = 0x0002,
    CO_VARARGS              = 0x0004,
    CO_VARKEYWORDS          = 0x0008,
    CO_NESTED               = 0x0010,
    CO_GENERATOR            = 0x0020,
    CO_NOFREE               = 0x0040,
    CO_COROUTINE            = 0x0080,
    CO_ITERABLE_COROUTINE   = 0x0100,
    CO_ASYNC_GENERATOR      = 0x0200,
    // Python 3.10+
    CO_HAS_DOCSTRING        = 0x0400,
    // Internal use
    CO_FUTURE_DIVISION      = 0x20000,
    CO_FUTURE_ABSOLUTE_IMPORT = 0x40000,
    CO_FUTURE_WITH_STATEMENT = 0x80000,
    CO_FUTURE_PRINT_FUNCTION = 0x100000,
    CO_FUTURE_UNICODE_LITERALS = 0x200000,
    CO_FUTURE_BARRY_AS_BDFL = 0x400000,
    CO_FUTURE_GENERATOR_STOP = 0x800000,
    CO_FUTURE_ANNOTATIONS   = 0x1000000,
};

// Parsed code object structure
struct code_object {
    // Code object metadata
    uint32_t argcount;
    uint32_t posonlyargcount;   // Python 3.8+
    uint32_t kwonlyargcount;
    uint32_t nlocals;
    uint32_t stacksize;
    uint32_t flags;
    
    // Bytecode
    const uint8_t* code;
    size_t code_size;
    
    // Object table indices (into marshal value array)
    uint32_t consts_idx;
    uint32_t names_idx;
    uint32_t varnames_idx;
    uint32_t freevars_idx;
    uint32_t cellvars_idx;
    
    // Source information
    uint32_t filename_idx;
    uint32_t name_idx;
    uint32_t qualname_idx;      // Python 3.3+
    uint32_t firstlineno;
    uint32_t linetable_idx;
    uint32_t exceptiontable_idx; // Python 3.11+
    
    // Tracking
    uint32_t parent_idx;        // Index of parent code object (-1 for module)
    uint32_t depth;             // Nesting depth
};

} // namespace marshal
} // namespace pyc

#endif // PYC_MARSHAL_CODE_OBJECT_CODE_OBJECT_HPP

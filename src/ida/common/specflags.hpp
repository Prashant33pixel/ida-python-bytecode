// Operand specflags for IDA
// Used to differentiate operand types during output

#ifndef PYC_IDA_COMMON_SPECFLAGS_HPP
#define PYC_IDA_COMMON_SPECFLAGS_HPP

#include <cstdint>

namespace pyc {
namespace ida {

// Operand specflags for type differentiation
enum specflag : uint8_t {
    SPEC_NONE   = 0,
    SPEC_CONST  = 1,    // Index into co_consts
    SPEC_NAME   = 2,    // Index into co_names
    SPEC_LOCAL  = 3,    // Index into co_varnames
    SPEC_FREE   = 4,    // Index into co_cellvars + co_freevars
    SPEC_CMP    = 5,    // Comparison operator
    SPEC_BINOP  = 6,    // Binary operator
    SPEC_COMMON_CONST = 7,  // Common constant (3.14+): AssertionError, etc.
    SPEC_LOCAL_PAIR   = 8,  // Two packed 4-bit local indices (3.13+)
};

} // namespace ida
} // namespace pyc

#endif // PYC_IDA_COMMON_SPECFLAGS_HPP

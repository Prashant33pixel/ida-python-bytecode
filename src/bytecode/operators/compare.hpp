// Python comparison operator definitions
// Names for COMPARE_OP operands

#ifndef PYC_BYTECODE_OPERATORS_COMPARE_HPP
#define PYC_BYTECODE_OPERATORS_COMPARE_HPP

#include <cstdint>

namespace pyc {
namespace bytecode {
namespace operators {

// Get name for comparison operator
// Python 3.12+ encodes comparison in bits 5-7 (use >> 5)
// Python < 3.12 uses direct index
const char* get_cmp_op_name(uint8_t op, bool is_py312_plus);

} // namespace operators
} // namespace bytecode
} // namespace pyc

#endif // PYC_BYTECODE_OPERATORS_COMPARE_HPP

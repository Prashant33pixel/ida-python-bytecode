// Python comparison operator definitions
// Names for COMPARE_OP operands

#ifndef PYC_BYTECODE_OPERATORS_COMPARE_HPP
#define PYC_BYTECODE_OPERATORS_COMPARE_HPP

#include <cstdint>

namespace pyc {
namespace bytecode {
namespace operators {

// Get name for comparison operator
// Python 3.12: comparison in bits 4-7 (>> 4)
// Python 3.13+: comparison in bits 5-7 (>> 5)
// Python < 3.12: direct index
const char* get_cmp_op_name(uint8_t op, uint8_t py_major, uint8_t py_minor);

} // namespace operators
} // namespace bytecode
} // namespace pyc

#endif // PYC_BYTECODE_OPERATORS_COMPARE_HPP

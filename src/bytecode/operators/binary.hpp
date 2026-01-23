// Python binary operator definitions (3.11+)
// Names for BINARY_OP operands

#ifndef PYC_BYTECODE_OPERATORS_BINARY_HPP
#define PYC_BYTECODE_OPERATORS_BINARY_HPP

#include <cstdint>

namespace pyc {
namespace bytecode {
namespace operators {

// Get name for binary operator (BINARY_OP opcode in 3.11+)
const char* get_binary_op_name(uint8_t op);

} // namespace operators
} // namespace bytecode
} // namespace pyc

#endif // PYC_BYTECODE_OPERATORS_BINARY_HPP

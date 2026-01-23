// Python 3.12+ intrinsic function definitions
// Names for CALL_INTRINSIC_1 and CALL_INTRINSIC_2 opcodes

#ifndef PYC_BYTECODE_INSTRUCTION_INTRINSICS_HPP
#define PYC_BYTECODE_INSTRUCTION_INTRINSICS_HPP

#include <cstdint>

namespace pyc {
namespace bytecode {

// Get name for CALL_INTRINSIC_1 intrinsic function
const char* get_intrinsic_1_name(uint8_t id);

// Get name for CALL_INTRINSIC_2 intrinsic function
const char* get_intrinsic_2_name(uint8_t id);

} // namespace bytecode
} // namespace pyc

#endif // PYC_BYTECODE_INSTRUCTION_INTRINSICS_HPP

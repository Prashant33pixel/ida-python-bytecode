// IDA register definitions for Python bytecode
// Virtual registers used by the processor module

#ifndef PYC_IDA_COMMON_REGISTERS_HPP
#define PYC_IDA_COMMON_REGISTERS_HPP

namespace pyc {
namespace ida {

// Register definitions
enum reg {
    REG_SP,     // Stack pointer (virtual)
    REG_PC,     // Program counter (virtual)  
    REG_CS,     // Virtual code segment
    REG_DS,     // Virtual data segment
    REG_COUNT
};

// Register names for IDA
extern const char* const register_names[];

} // namespace ida
} // namespace pyc

#endif // PYC_IDA_COMMON_REGISTERS_HPP

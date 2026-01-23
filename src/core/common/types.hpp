// Core type definitions for Python Bytecode Processor Module
// Common types used throughout the codebase

#ifndef PYC_CORE_COMMON_TYPES_HPP
#define PYC_CORE_COMMON_TYPES_HPP

#include <cstdint>
#include <cstddef>

namespace pyc {

// Platform ID for Python bytecode processor
constexpr uint32_t PLFM_PYC = 0x8042;

// Extended argument opcode (common to all Python versions)
constexpr uint8_t EXTENDED_ARG = 144;

// Threshold for opcodes with arguments in pre-wordcode versions
constexpr uint8_t HAVE_ARGUMENT = 90;

} // namespace pyc

#endif // PYC_CORE_COMMON_TYPES_HPP

// Platform detection and configuration
// Handles platform-specific defines and settings

#ifndef PYC_CORE_COMMON_PLATFORM_HPP
#define PYC_CORE_COMMON_PLATFORM_HPP

// Platform detection is handled by CMake, but we include
// this header for any platform-specific adjustments needed

namespace pyc {
namespace platform {

// Endianness - Python bytecode uses little-endian
constexpr bool is_little_endian = true;

// Byte sizes for code/data segments
constexpr int code_bits_per_byte = 8;
constexpr int data_bits_per_byte = 8;

} // namespace platform
} // namespace pyc

#endif // PYC_CORE_COMMON_PLATFORM_HPP

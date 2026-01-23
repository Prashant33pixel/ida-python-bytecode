// Python version family definitions
// Groups Python versions with similar bytecode characteristics

#ifndef PYC_CORE_VERSION_VERSION_HPP
#define PYC_CORE_VERSION_VERSION_HPP

#include <cstdint>

namespace pyc {
namespace version {

// Version family - groups similar Python versions for opcode handling
enum class family : uint8_t {
    unknown     = 0,
    py2x        = 1,    // Python 2.7
    py30_35     = 2,    // Python 3.0-3.5 (variable-width instructions)
    py36_310    = 3,    // Python 3.6-3.10 (wordcode, no caches)
    py311_313   = 4,    // Python 3.11-3.13 (wordcode with inline caches)
    py314_plus  = 5,    // Python 3.14+ (completely renumbered opcodes)
};

// Determine version family from major.minor version
inline family get_family(uint8_t major, uint8_t minor) {
    if (major == 2)
        return family::py2x;
    if (major == 3) {
        if (minor <= 5)
            return family::py30_35;
        if (minor <= 10)
            return family::py36_310;
        if (minor <= 13)
            return family::py311_313;
        return family::py314_plus;
    }
    return family::unknown;
}

// Check if version uses wordcode format (2 bytes per instruction)
inline bool uses_wordcode(family f) {
    return f >= family::py36_310;
}

// Check if version has inline cache entries
inline bool has_caches(family f) {
    return f >= family::py311_313;
}

// Get instruction base size for a version family
inline int get_insn_base_size(family f) {
    return uses_wordcode(f) ? 2 : 0;  // 0 means variable
}

} // namespace version
} // namespace pyc

#endif // PYC_CORE_VERSION_VERSION_HPP

// Python magic number definitions
// Maps magic numbers to Python versions

#ifndef PYC_CORE_VERSION_MAGIC_HPP
#define PYC_CORE_VERSION_MAGIC_HPP

#include <cstdint>
#include <cstddef>

namespace pyc {
namespace version {

// Magic number entry describing a Python version
struct magic_entry {
    uint16_t    magic;          // First 2 bytes of magic number
    uint8_t     major;          // Python major version
    uint8_t     minor;          // Python minor version
    uint8_t     header_size;    // Size of pyc header in bytes
    uint8_t     word_size;      // Instruction width (1=variable, 2=wordcode)
    bool        has_caches;     // Has inline caches (3.11+)
    const char* name;           // Human-readable version string
};

// Complete magic number table
extern const magic_entry magic_table[];

// Number of entries in magic table (excluding sentinel)
extern const size_t magic_table_size;

// Lookup magic number in table
// Returns nullptr if not found
const magic_entry* lookup_magic(uint16_t magic);

// Get the default/fallback version entry (for raw marshal files)
const magic_entry* get_default_version();

} // namespace version
} // namespace pyc

#endif // PYC_CORE_VERSION_MAGIC_HPP

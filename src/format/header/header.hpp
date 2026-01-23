// Python .pyc file header structures
// Version-specific header formats

#ifndef PYC_FORMAT_HEADER_HEADER_HPP
#define PYC_FORMAT_HEADER_HEADER_HPP

#include <cstdint>

namespace pyc {
namespace format {

#pragma pack(push, 1)

// Python 2.x - 3.2 header (8 bytes)
struct header_v1 {
    uint32_t magic;      // Magic number + 0x0d0a
    uint32_t mtime;      // Source modification time
};

// Python 3.3 - 3.6 header (12 bytes)
struct header_v2 {
    uint32_t magic;      // Magic number + 0x0d0a
    uint32_t mtime;      // Source modification time
    uint32_t src_size;   // Source file size
};

// Python 3.7+ header (16 bytes)
struct header_v3 {
    uint32_t magic;      // Magic number + 0x0d0a
    uint32_t flags;      // Bit field (0=timestamp, 1=hash-unchecked, 3=hash-checked)
    union {
        struct {
            uint32_t mtime;      // Source modification time
            uint32_t src_size;   // Source file size
        } timestamp;
        uint64_t source_hash;    // SipHash of source (when flags & 1)
    };
};

#pragma pack(pop)

// Get header size for a given Python version
inline int get_header_size(uint8_t major, uint8_t minor) {
    if (major == 2) return 8;
    if (minor <= 2) return 8;
    if (minor <= 6) return 12;
    return 16;
}

// Check if magic bytes indicate a valid .pyc file
inline bool is_valid_magic(uint32_t magic) {
    // Check CRLF marker in upper 16 bits
    uint16_t marker = magic >> 16;
    return marker == 0x0D0A || marker == 0x0A0D;
}

// Extract version magic from full magic number
inline uint16_t extract_version_magic(uint32_t magic) {
    return magic & 0xFFFF;
}

} // namespace format
} // namespace pyc

#endif // PYC_FORMAT_HEADER_HEADER_HPP

// Python marshal type codes
// Used for parsing serialized Python objects

#ifndef PYC_MARSHAL_TYPES_TYPE_CODES_HPP
#define PYC_MARSHAL_TYPES_TYPE_CODES_HPP

#include <cstdint>

namespace pyc {
namespace marshal {

// High bit indicates FLAG_REF - object should be added to reference table
constexpr uint8_t FLAG_REF = 0x80;

// Marshal type codes
enum type_code : uint8_t {
    // Singleton types
    TYPE_NULL           = '0',  // 0x30 - NULL object (internal marker)
    TYPE_NONE           = 'N',  // 0x4E - None
    TYPE_FALSE          = 'F',  // 0x46 - False
    TYPE_TRUE           = 'T',  // 0x54 - True
    TYPE_STOPITER       = 'S',  // 0x53 - StopIteration
    TYPE_ELLIPSIS       = '.',  // 0x2E - Ellipsis (...)
    
    // Numeric types
    TYPE_INT            = 'i',  // 0x69 - 32-bit signed integer
    TYPE_INT64          = 'I',  // 0x49 - 64-bit signed integer (Python 2)
    TYPE_FLOAT          = 'f',  // 0x66 - Float as ASCII string (Python 2)
    TYPE_BINARY_FLOAT   = 'g',  // 0x67 - Float as 8-byte IEEE 754
    TYPE_COMPLEX        = 'x',  // 0x78 - Complex as ASCII (Python 2)
    TYPE_BINARY_COMPLEX = 'y',  // 0x79 - Complex as two 8-byte floats
    TYPE_LONG           = 'l',  // 0x6C - Arbitrary precision integer
    
    // String types
    TYPE_STRING         = 's',  // 0x73 - Byte string (length + data)
    TYPE_INTERNED       = 't',  // 0x74 - Interned string (added to intern list)
    TYPE_STRINGREF      = 'R',  // 0x52 - Reference to interned string
    TYPE_ASCII          = 'a',  // 0x61 - ASCII string (Python 3)
    TYPE_ASCII_INTERNED = 'A',  // 0x41 - Interned ASCII string
    TYPE_SHORT_ASCII    = 'z',  // 0x7A - Short ASCII (length < 256)
    TYPE_SHORT_ASCII_INTERNED = 'Z', // 0x5A - Short interned ASCII
    TYPE_UNICODE        = 'u',  // 0x75 - Unicode string (UTF-8)
    TYPE_SMALL_TUPLE    = ')',  // 0x29 - Tuple with length < 256
    
    // Container types
    TYPE_TUPLE          = '(',  // 0x28 - Tuple
    TYPE_LIST           = '[',  // 0x5B - List
    TYPE_DICT           = '{',  // 0x7B - Dict
    TYPE_SET            = '<',  // 0x3C - Set (Python 2.6+)
    TYPE_FROZENSET      = '>',  // 0x3E - Frozenset
    
    // Code object
    TYPE_CODE           = 'c',  // 0x63 - Code object
    
    // Reference types (marshal version 3+)
    TYPE_REF            = 'r',  // 0x72 - Reference to previous object
    
    // Unknown/reserved
    TYPE_UNKNOWN        = '?',  // 0x3F - Unknown type
};

// Get type name string
const char* get_type_name(uint8_t type);

// Check if type is a string type
inline bool is_string_type(uint8_t type) {
    type &= ~FLAG_REF;
    return type == TYPE_STRING || type == TYPE_INTERNED || 
           type == TYPE_ASCII || type == TYPE_ASCII_INTERNED ||
           type == TYPE_SHORT_ASCII || type == TYPE_SHORT_ASCII_INTERNED ||
           type == TYPE_UNICODE;
}

// Check if type is a short string (1-byte length)
inline bool is_short_string(uint8_t type) {
    type &= ~FLAG_REF;
    return type == TYPE_SHORT_ASCII || type == TYPE_SHORT_ASCII_INTERNED;
}

} // namespace marshal
} // namespace pyc

#endif // PYC_MARSHAL_TYPES_TYPE_CODES_HPP

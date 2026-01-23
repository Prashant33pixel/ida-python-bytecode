// Python 3.12+ intrinsic function name tables
// Implementation of CALL_INTRINSIC_* name lookup

#include "intrinsics.hpp"

namespace pyc {
namespace bytecode {

// CALL_INTRINSIC_1 intrinsics (single argument)
static const char* const intrinsic_1_names[] = {
    "INTRINSIC_1_INVALID",
    "INTRINSIC_PRINT",
    "INTRINSIC_IMPORT_STAR",
    "INTRINSIC_STOPITERATION_ERROR",
    "INTRINSIC_ASYNC_GEN_WRAP",
    "INTRINSIC_UNARY_POSITIVE",
    "INTRINSIC_LIST_TO_TUPLE",
    "INTRINSIC_TYPEVAR",
    "INTRINSIC_PARAMSPEC",
    "INTRINSIC_TYPEVARTUPLE",
    "INTRINSIC_SUBSCRIPT_GENERIC",
    "INTRINSIC_TYPEALIAS",
    nullptr
};

// CALL_INTRINSIC_2 intrinsics (two arguments)  
static const char* const intrinsic_2_names[] = {
    "INTRINSIC_2_INVALID",
    "INTRINSIC_PREP_RERAISE_STAR",
    "INTRINSIC_TYPEVAR_WITH_BOUND",
    "INTRINSIC_TYPEVAR_WITH_CONSTRAINTS",
    "INTRINSIC_SET_FUNCTION_TYPE_PARAMS",
    nullptr
};

const char* get_intrinsic_1_name(uint8_t id) {
    if (id < 12 && intrinsic_1_names[id])
        return intrinsic_1_names[id];
    return "INTRINSIC_UNKNOWN";
}

const char* get_intrinsic_2_name(uint8_t id) {
    if (id < 5 && intrinsic_2_names[id])
        return intrinsic_2_names[id];
    return "INTRINSIC_UNKNOWN";
}

} // namespace bytecode
} // namespace pyc

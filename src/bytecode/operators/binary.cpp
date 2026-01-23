// Python binary operator implementation (3.11+)

#include "binary.hpp"

namespace pyc {
namespace bytecode {
namespace operators {

static const char* const binary_ops[] = {
    "+",    // 0 NB_ADD
    "&",    // 1 NB_AND
    "//",   // 2 NB_FLOOR_DIVIDE
    "<<",   // 3 NB_LSHIFT
    "@",    // 4 NB_MATRIX_MULTIPLY
    "*",    // 5 NB_MULTIPLY
    "%",    // 6 NB_REMAINDER
    "|",    // 7 NB_OR
    "**",   // 8 NB_POWER
    ">>",   // 9 NB_RSHIFT
    "-",    // 10 NB_SUBTRACT
    "/",    // 11 NB_TRUE_DIVIDE
    "^",    // 12 NB_XOR
    "+=",   // 13 NB_INPLACE_ADD
    "&=",   // 14 NB_INPLACE_AND
    "//=",  // 15 NB_INPLACE_FLOOR_DIVIDE
    "<<=",  // 16 NB_INPLACE_LSHIFT
    "@=",   // 17 NB_INPLACE_MATRIX_MULTIPLY
    "*=",   // 18 NB_INPLACE_MULTIPLY
    "%=",   // 19 NB_INPLACE_REMAINDER
    "|=",   // 20 NB_INPLACE_OR
    "**=",  // 21 NB_INPLACE_POWER
    ">>=",  // 22 NB_INPLACE_RSHIFT
    "-=",   // 23 NB_INPLACE_SUBTRACT
    "/=",   // 24 NB_INPLACE_TRUE_DIVIDE
    "^=",   // 25 NB_INPLACE_XOR
};

const char* get_binary_op_name(uint8_t op) {
    if (op < sizeof(binary_ops) / sizeof(binary_ops[0]))
        return binary_ops[op];
    return "??";
}

} // namespace operators
} // namespace bytecode
} // namespace pyc

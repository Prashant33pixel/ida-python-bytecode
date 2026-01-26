// Python comparison operator implementation

#include "compare.hpp"

namespace pyc {
namespace bytecode {
namespace operators {

static const char* const cmp_ops[] = {
    "<", "<=", "==", "!=", ">", ">="
};

const char* get_cmp_op_name(uint8_t op, uint8_t py_major, uint8_t py_minor) {
    uint8_t cmp_idx;
    if (py_major == 3 && py_minor == 12) {
        // Python 3.12 only: comparison op in bits 4-7
        // Changed under GH-100923
        cmp_idx = op >> 4;
    } else if (py_major > 3 || (py_major == 3 && py_minor >= 13)) {
        // Python 3.13+: comparison op in bits 5-7
        cmp_idx = op >> 5;
    } else {
        // Python < 3.12: direct index
        cmp_idx = op;
    }
    if (cmp_idx < 6)
        return cmp_ops[cmp_idx];
    return "??";
}

} // namespace operators
} // namespace bytecode
} // namespace pyc

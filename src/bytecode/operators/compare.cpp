// Python comparison operator implementation

#include "compare.hpp"

namespace pyc {
namespace bytecode {
namespace operators {

static const char* const cmp_ops[] = {
    "<", "<=", "==", "!=", ">", ">="
};

const char* get_cmp_op_name(uint8_t op, bool is_py312_plus) {
    uint8_t cmp_idx;
    if (is_py312_plus) {
        // Python 3.12+ encodes comparison op in bits 5-7
        // Lower bits contain flags (bit 4 = invert result)
        cmp_idx = op >> 5;
    } else {
        // Python < 3.12 uses direct index
        cmp_idx = op;
    }
    if (cmp_idx < 6)
        return cmp_ops[cmp_idx];
    return "??";
}

} // namespace operators
} // namespace bytecode
} // namespace pyc
